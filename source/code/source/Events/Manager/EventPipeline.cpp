#include "stdafx.h"
#include "EventPipeline.h"
#include <Events/EventsApi.h>
#include "Event.h"
#include "AsyncProvider.h"
#include <playfab/PlayFabErrors.h>

using namespace PlayFab::EventsModels;

namespace PlayFab
{
namespace EventManager
{

// XAsync provider for a long running background operation that periodically polls and consumes events from
// an EventPipeline's buffer. When an event batch is ready, it is written using the appropriate PlayFab API
// (depending on the EventPipelineType).
//
// The operation will continue to poll until it is explicitly terminated with Terminate or the associated
// EventPipeline is destroyed. If Terminate is called, the provider will write any remaining events, wait for
// in flight batches to complete, and then invoke a TerminationCallback to indicate that it is safe to shutdown
// without events being lost. Note that the write result for each event will still be delivered normally via the
// return AsyncOp even when the Provider is terminated.
class WriteEventsProvider : public Provider
{
public:
    WriteEventsProvider(SharedPtr<EventPipeline> pipeline, SharedPtr<HttpClient const> httpClient, SharedPtr<AuthTokens const> tokens, const TaskQueue& queue);
    WriteEventsProvider(const WriteEventsProvider& source) = delete;
    WriteEventsProvider& operator=(const WriteEventsProvider& source) = delete;
    ~WriteEventsProvider();

    HRESULT Terminate(TerminationCallback callback);

private:
    // Provider overrides
    HRESULT DoWork(TaskQueue&& queue) override;
    HRESULT Cancel(TaskQueue&& queue) override;

private:
    void SendBatch(Vector<EventContext>&& batch, const TaskQueue& queue);

    WeakPtr<EventPipeline> m_pipeline;
    EventPipelineType const m_pipelineType; // redundant with m_pipeline->pipelineType but we need this to write events even if m_pipline is expired

    EventsAPI const m_eventsAPI;

    TaskQueue const m_taskQueue;
    XAsyncBlock m_asyncBlock;
    TerminationCallback m_terminationCallback;

    std::atomic<size_t> m_batchesInFlight{ 0 };
    std::atomic<bool> m_terminated{ false };
    Vector<EventContext> m_batch;
    Clock::time_point m_batchCreationTime;
};

SharedPtr<EventPipeline> EventPipeline::Make(
    EventPipelineType type,
    SharedPtr<HttpClient const> httpClient,
    SharedPtr<AuthTokens const> tokens,
    const TaskQueue& queue
)
{
    auto pipeline = SharedPtr<EventPipeline>(new (Allocator<EventPipeline>{}.allocate(1)) EventPipeline(type));

    // Create and start WriteEventsProvider. EventPipeline has a non-owning pointer to it - it's lifetime is automatically
    // by the Provider class. The background operation will run until its explicitly terminated or the pipeline its consuming
    // events from is destroyed.
    pipeline->m_writeEventsProvider = MakeUnique<WriteEventsProvider>(pipeline, std::move(httpClient), std::move(tokens), queue).release();
    Provider::Run(UniquePtr<Provider>{ pipeline->m_writeEventsProvider });

    return pipeline;
}

EventPipeline::EventPipeline(EventPipelineType type) :
    pipelineType{ type },
    m_buffer{ settings.bufferSize }
{
}

AsyncOp<String> EventPipeline::IntakeEvent(SharedPtr<Event const> event)
{
    auto eventAsyncContext = MakeShared<AsyncOpContext<String>>();
    EventContext eventContext{ std::move(event), eventAsyncContext };

    auto putBufferResult = m_buffer.TryPut(std::move(eventContext));
    if (Failed(putBufferResult))
    {
        return putBufferResult.hr;
    }

    return eventAsyncContext;
}

HRESULT EventPipeline::Terminate(TerminationCallback callback)
{
    RETURN_IF_FAILED(m_buffer.Terminate());
    return m_writeEventsProvider->Terminate(std::move(callback));
}

WriteEventsProvider::WriteEventsProvider(SharedPtr<EventPipeline> pipeline, SharedPtr<HttpClient const> httpClient, SharedPtr<AuthTokens const> tokens, const TaskQueue& queue) :
    Provider{ &m_asyncBlock },
    m_pipeline{ pipeline },
    m_pipelineType{ pipeline->pipelineType },
    m_eventsAPI{ std::move(httpClient), std::move(tokens) },
    m_taskQueue{ queue.DeriveWorkerQueue() },
    m_asyncBlock{ m_taskQueue.GetHandle(), nullptr, nullptr }
{
}

WriteEventsProvider::~WriteEventsProvider()
{
    // Invoking m_terminationCallback here rather than in XAsync callback because the provider may be destroyed prior to
    // the XAsync callback being invoked. XAsync framework aggressively cleans up the provider as soon it can, and because the operation
    // has no result payload, cleanup happens immediately after DoWork (prior to the completion callback being invoked in some cases)

    if (m_terminationCallback)
    {
        m_terminationCallback();
    }
}

HRESULT WriteEventsProvider::Terminate(TerminationCallback callback)
{
    if (m_terminated)
    {
        // Calling Terminate twice is not allowed
        return E_PLAYFAB_EVENTMANAGERTERMINATED;
    }

    m_terminationCallback = std::move(callback);
    XAsyncCancel(&m_asyncBlock);
    return S_OK;
}

HRESULT WriteEventsProvider::DoWork(TaskQueue&& queue)
{
    // XAsync should never call XAsyncOp::DoWork after XAsyncOp::Cancel
    assert(!m_terminated);

    SharedPtr<EventPipeline> pipeline = m_pipeline.lock();
    if (!pipeline)
    {
        // If the pipeline has been destroyed, cancel the task and return E_PENDING. WriteEventsProvider::Cancel 
        // ensures pending events & batches in flight complete before completing the XAsync operation
        XAsyncCancel(&m_asyncBlock);
        return E_PENDING;
    }

    m_batch.reserve(pipeline->settings.maxItemsInBatch);

    if (m_batchesInFlight >= pipeline->settings.maxBatchesInFlight)
    {
        // do not take new events from m_buffer if batches currently in flight are at the maximum allowed number
        // and are not sent out (or received an error) yet
        RETURN_IF_FAILED(Schedule(pipeline->settings.waitTimeInMs));
        return E_PENDING;
    }

    auto getFromBufferResult = pipeline->m_buffer.TryTake();
    if (Succeeded(getFromBufferResult))
    {
        // Successfully took event from buffer. Add it to current batch
        m_batch.emplace_back(getFromBufferResult.ExtractPayload());

        // If it's the first event in an incomplete batch then set the batch creation time
        if (m_batch.size() == 1)
        {
            m_batchCreationTime = Clock::now();
        }

        // Send batch if its full
        if (m_batch.size() >= pipeline->settings.maxItemsInBatch)
        {
            SendBatch(std::move(m_batch), queue);
        }

        RETURN_IF_FAILED(Schedule(0));
        return E_PENDING;
    }

    if (!m_batch.empty())
    {
        // check if the batch wait time expired
        std::chrono::seconds batchAge = std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - m_batchCreationTime);
        if (batchAge.count() >= (int32_t)pipeline->settings.maxBatchWaitTimeInSeconds)
        {
            // batch wait time elapsed, send incomplete batch
            SendBatch(std::move(m_batch), queue);
            RETURN_IF_FAILED(Schedule(0));
            return E_PENDING;
        }
    }

    RETURN_IF_FAILED(Schedule(pipeline->settings.waitTimeInMs));
    return E_PENDING;
}

HRESULT WriteEventsProvider::Cancel(TaskQueue&& queue)
{
    m_terminated.store(true);

    SharedPtr<EventPipeline> pipeline = m_pipeline.lock();
    if (pipeline)
    {
        // Get any remaining events from buffer before sending final batch. Ignore limits defined by PipelineSettings
        // during termination
        for(;;)
        {
            auto getFromBufferResult = pipeline->m_buffer.TryTake();
            if (Succeeded(getFromBufferResult))
            {
                m_batch.emplace_back(getFromBufferResult.ExtractPayload());
            }
            else
            {
                break;
            }
        }
    }

    if (!m_batch.empty())
    {
        SendBatch(std::move(m_batch), queue);
    }
    else if (m_batchesInFlight == 0)
    {
        // If their are no pending events or in batches in flight, complete the XAsync operation
        Provider::Complete(0);
    }
    return S_OK;
}

void WriteEventsProvider::SendBatch(Vector<EventContext>&& batch, const TaskQueue& queue)
{
    // create a WriteEvents API request to send the batch
    PlayFabEventsWriteEventsRequest request{};
    Vector<const PlayFabEventsEventContents*> events(batch.size());

    for (const auto& eventContext : batch)
    {
        events.push_back(&eventContext.event->EventContents());
    }
    request.events = const_cast<PlayFabEventsEventContents**>(events.data()); // TODO PlayFabEventsWriteEventsRequest.events should be const
    request.eventsCount = static_cast<uint32_t>(events.size());

    ++m_batchesInFlight;
    AsyncOp<WriteEventsResponse> writeOperation{ nullptr };

    switch (m_pipelineType)
    {
    case EventPipelineType::PlayStream:
    {
        // call Events API to send the batch
        writeOperation = m_eventsAPI.WriteEvents(request, queue);
        break;
    }
    case EventPipelineType::Telemetry:
    {
        // call Events API to send the batch, bypassing Playstream
        writeOperation = m_eventsAPI.WriteTelemetryEvents(request, queue);
        break;
    }
    default: assert(false); // Unrecogized pipeline type
    }

    writeOperation.Finally([this, batch](Result<WriteEventsResponse> result)
    {
        // Complete asyncOpContexts for each event in the batch
        if (Succeeded(result))
        {
            auto& resultPayload{ result.Payload() };

            // returned "assignedEventIds" order is guaranteed to be in the same order of events written
            for (size_t eventIndex = 0; eventIndex < batch.size(); ++eventIndex)
            {
                if (eventIndex < resultPayload.assignedEventIdsCount)
                {
                    batch[eventIndex].writeEventAsyncOp->Complete(Result<String>{ resultPayload.assignedEventIds[eventIndex] });
                }
                else
                {
                    // No eventId was assigned, complete with empty string
                    batch[eventIndex].writeEventAsyncOp->Complete(String{});
                }
            }
        }
        else
        {
            for (const auto& eventContext : batch)
            {
                eventContext.writeEventAsyncOp->Complete(result.hr);
            }
        }

        if (--m_batchesInFlight == 0 && m_terminated)
        {
            Provider::Complete(0);
        }
    });
}

} // namespace EventManager
} // namespace PlayFab

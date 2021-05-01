#pragma once

#include <Events/EventsApi.h>
#include "Event.h"
#include "EventBuffer.h"
#include "EventManagerApi.h"

namespace PlayFab
{
namespace EventManager
{

// TODO allow these to be configurable
struct EventPipelineSettings
{
    static constexpr size_t defaultBufferSize{ 256 };
    static constexpr size_t defaultMaxItemsInBatch{ 5 };
    static constexpr size_t defaultMaxBatchWaitTimeInSeconds{ 3 };
    static constexpr size_t defaultMaxBatchesInFlight{ 16 };
    static constexpr uint32_t defaultWaitTimeInMs{ 10 };

    // The minimum size of EventBuffer, in bytes. The actually allocated size will be a power of 2 that is equal or greater than this value.
    size_t bufferSize{ defaultBufferSize };

    // The max number of items (events) a batch can hold before it is sent out.
    size_t maxItemsInBatch{ defaultMaxItemsInBatch };

    // The max wait time before a batch must be sent out even if it's still incomplete, in seconds.
    size_t maxBatchWaitTimeInSeconds{ defaultMaxBatchWaitTimeInSeconds };

    // The max number of batches currently "in flight".
    size_t maxBatchesInFlight{ defaultMaxBatchesInFlight };

    // The wait time between WriteEventsProvider polls. When the event buffer is empty or maxBatchesInFlight
    // has been reached, the pipeline will delay for this interval before trying again.
    uint32_t waitTimeInMs{ defaultWaitTimeInMs };
};

// Producer-consumer Event pipeline. Events added by clients via IntakeEvent & consumed and written to PlayFab
// by WriteEventsProvider in the background.
class EventPipeline : public std::enable_shared_from_this<EventPipeline>
{
public:
    static SharedPtr<EventPipeline> Make(
        EventPipelineType type,
        SharedPtr<HttpClient const> httpClient,
        SharedPtr<AuthTokens const> tokens,
        const TaskQueue& queue
    );

    EventPipeline(const EventPipeline& source) = delete;
    EventPipeline& operator=(const EventPipeline& source) = delete;
    ~EventPipeline() = default;

    EventPipelineSettings settings;
    EventPipelineType const pipelineType;

    AsyncOp<String> IntakeEvent(SharedPtr<Event const> event);
    HRESULT Terminate(TerminationCallback callback);

private:
    EventPipeline(EventPipelineType type);

    EventBuffer m_buffer;
    class WriteEventsProvider* m_writeEventsProvider; // non-owning pointer

    friend class WriteEventsProvider;
};

} // namespace EventManager
} // namespace PlayFab

#include "stdafx.h"
#include "EventManagerApi.h"
#include "EventPipeline.h"
#include "Event.h"

namespace PlayFab
{
namespace EventManager
{

EventManagerAPI::EventManagerAPI(SharedPtr<HttpClient const> httpClient, SharedPtr<AuthTokens const> tokens, const TaskQueue& queue)
{
    m_pipelines.emplace(EventPipelineType::PlayStream, EventPipeline::Make(EventPipelineType::PlayStream, httpClient, tokens, queue));
    m_pipelines.emplace(EventPipelineType::Telemetry, EventPipeline::Make(EventPipelineType::Telemetry, httpClient, tokens, queue));
}

AsyncOp<String> EventManagerAPI::WriteEvent(SharedPtr<Event const> event) const
{
    // Route the event to the appropriate pipeline
    EventPipelineType pipelineType{};

    switch (event->eventType)
    {
    case PlayFabEventManagerEventType::Default:
    case PlayFabEventManagerEventType::Lightweight:
    {
        // route lightweight (and default) events to PlayFab, bypassing PlayStream
        pipelineType = EventPipelineType::Telemetry;
        break;
    }
    case PlayFabEventManagerEventType::Heavyweight:
    {
        pipelineType = EventPipelineType::PlayStream;
        break;
    }
    default:
    {
        // Unexpected event type
        return E_UNEXPECTED;
    }
    }

    auto pipelineEntry = m_pipelines.find(pipelineType);
    if (pipelineEntry == m_pipelines.end())
    {
        return E_UNEXPECTED;
    }

    return pipelineEntry->second->IntakeEvent(event);
}

HRESULT EventManagerAPI::Terminate(TerminationCallback callback) const
{
    struct TerminationContext
    {
        std::atomic<size_t> pendingTerminations;
        TerminationCallback callback;
    };

    auto context = MakeShared<TerminationContext>();
    context->pendingTerminations = m_pipelines.size();
    context->callback = std::move(callback);

    for (auto& pipelineEntry : m_pipelines)
    {
        HRESULT hr = pipelineEntry.second->Terminate([context](void)
        {
            if (--context->pendingTerminations == 0 && context->callback != nullptr)
            {
                context->callback();
            }
        });

        RETURN_IF_FAILED(hr);
    }

    return S_OK;
}

} // namespace EventManager
} // namespace PlayFab

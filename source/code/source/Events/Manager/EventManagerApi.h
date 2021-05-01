#pragma once

#include <Events/EventsDataModels.h>
#include "HttpClient.h"
#include "TaskQueue.h"

namespace PlayFab
{
struct AuthTokens;

namespace EventManager
{

class Event;
class EventPipeline;

enum class EventPipelineType
{
    PlayStream, // PlayFab (PlayStream) event pipeline
    Telemetry // PlayFab event pipeline, bypasses PlayStream
};

using TerminationCallback = std::function<void()>;

// Thick logic for managing and uploading PlayFab events. Events will be batched and uploaded in the background
// on the provided queue. To ensure no events are lost, Terminate should be called prior to destroying the EventManagerAPI.
class EventManagerAPI
{
public:
    EventManagerAPI(SharedPtr<HttpClient const> httpClient, SharedPtr<AuthTokens const> tokens, const TaskQueue& queue);

    AsyncOp<String> WriteEvent(SharedPtr<Event const> event) const;
    HRESULT Terminate(TerminationCallback callback) const;

private:
    UnorderedMap<EventPipelineType, SharedPtr<EventPipeline>> m_pipelines;
};

} // namespace EventManager
} // namespace PlayFab

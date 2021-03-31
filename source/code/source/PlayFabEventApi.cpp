#include "stdafx.h"

#ifndef DISABLE_PLAYFABENTITY_API

#include "PlayFabEventApi.h"

namespace PlayFab
{
    PlayFabEventAPI::PlayFabEventAPI(bool threadedEventPipeline) : 
        eventRouter(std::make_shared<PlayFabEventRouter>(threadedEventPipeline))
    {
    }

    SharedPtr<IPlayFabEventRouter> PlayFabEventAPI::GetEventRouter() const
    {
        return this->eventRouter;
    }

    void PlayFabEventAPI::EmitEvent(UniquePtr<const IPlayFabEvent> event, const PlayFabEmitEventCallback callback) const
    {
        auto eventRequest = MakeShared<PlayFabEmitEventRequest>();
        SharedPtr<const IPlayFabEvent> sharedGenericEvent = std::move(event);
        eventRequest->event = std::dynamic_pointer_cast<const PlayFabEvent>(sharedGenericEvent);
        eventRequest->callback = callback;

        this->eventRouter->RouteEvent(eventRequest);
    }

    void PlayFabEventAPI::EmitEvent(UniquePtr<const IPlayFabEvent> event, std::function<void(SharedPtr<const IPlayFabEvent>, SharedPtr<const IPlayFabEmitEventResponse>)> callback) const
    {
        auto eventRequest = MakeShared<PlayFabEmitEventRequest>();
        SharedPtr<const IPlayFabEvent> sharedGenericEvent = std::move(event);
        eventRequest->event = std::dynamic_pointer_cast<const PlayFabEvent>(sharedGenericEvent);
        eventRequest->stdCallback = callback;

        this->eventRouter->RouteEvent(eventRequest);
    }
    
    void PlayFabEventAPI::Update()
    {
        this->eventRouter->Update();
    }
}

#endif

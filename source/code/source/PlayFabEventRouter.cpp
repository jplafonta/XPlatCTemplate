#include "stdafx.h"

#ifndef DISABLE_PLAYFABENTITY_API

#include "PlayFabEventRouter.h"

namespace PlayFab
{
    const UnorderedMap<EventPipelineKey, SharedPtr<IPlayFabEventPipeline>>& IPlayFabEventRouter::GetPipelines() const
    {
        return this->pipelines;
    }

    PlayFabEventRouter::PlayFabEventRouter(bool threadedEventPipeline)
    {
        this->pipelines.emplace(EventPipelineKey::PlayFabPlayStream, std::make_shared<PlayFabEventPipeline>(std::make_shared<PlayFabEventPipelineSettings>(PlayFabEventPipelineType::PlayFabPlayStream, threadedEventPipeline)));
        this->pipelines.emplace(EventPipelineKey::PlayFabTelemetry, std::make_shared<PlayFabEventPipeline>(std::make_shared<PlayFabEventPipelineSettings>(PlayFabEventPipelineType::PlayFabTelemetry, threadedEventPipeline)));
    }

    void PlayFabEventRouter::RouteEvent(SharedPtr<const IPlayFabEmitEventRequest> request) const
    {
        // only events based on PlayFabEmitEventRequest are supported by default pipelines
        const PlayFabEmitEventRequest* pfRequestPtr = dynamic_cast<const PlayFabEmitEventRequest*>(request.get());
        if (pfRequestPtr != nullptr)
        {
            for (auto& pipelineEntry : this->pipelines)
            {
                switch (pfRequestPtr->event->eventType)
                {
                    case PlayFabEventType::Default:
                    case PlayFabEventType::Lightweight:
                    {
                        // route lightweight (and default) events to PlayFab, bypassing PlayStream
                        if (pipelineEntry.first == EventPipelineKey::PlayFabTelemetry)
                        {
                            pipelineEntry.second->IntakeEvent(request);
                        }
                        break;
                    }
                    case PlayFabEventType::Heavyweight:
                    {
                        // route heavyweight events to PlayFab pipeline only
                        if (pipelineEntry.first == EventPipelineKey::PlayFabPlayStream)
                        {
                            pipelineEntry.second->IntakeEvent(request);
                        }
                        break;
                    }
                    default:
                    {
                        // do not route unsupported types of events
                    }
                }
            }
        }
    }
    
    void PlayFabEventRouter::Update()
    {
        for (std::pair<EventPipelineKey, SharedPtr<IPlayFabEventPipeline>> pipeline : this->pipelines)
        {
            pipeline.second->Update();
        }
    }
}

#endif
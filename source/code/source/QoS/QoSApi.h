#pragma once

#include "QoSDataModels.h"
#include "Multiplayer/MultiplayerApi.h"
#include "Events/EventsApi.h"

namespace PlayFab
{

namespace QoS
{

class QoSAPI
{
public:
    QoSAPI(SharedPtr<HttpClient const> httpClient, SharedPtr<AuthTokens const> tokens);

    AsyncOp<Measurements> GetMeasurements(uint32_t pingIterations, uint32_t timeoutMs, const TaskQueue& queue) const;

private:
    EventsAPI const m_eventsApi;
    MultiplayerAPI const m_multiplayerApi;

    AsyncOp<void> GetServers(const TaskQueue& queue) const;
    AsyncOp<Measurements> PingServers(uint32_t pingIterations, uint32_t timeoutMs, const TaskQueue& queue) const;

private:
    // Cached QoS servers
    mutable Vector<Server> m_servers;
};

} // namespace QoS
} // namespace PlayFab

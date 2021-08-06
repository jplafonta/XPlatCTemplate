#pragma once

#include "QoSDataModels.h"
#include "MultiplayerServer/MultiplayerServer.h"
#include "PlayStream/PlayStream.h"

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
    PlayStreamAPI const m_playStreamApi;
    MultiplayerServerAPI const m_multiplayerServerApi;

    AsyncOp<void> GetServers(const TaskQueue& queue) const;
    AsyncOp<Measurements> PingServers(uint32_t pingIterations, uint32_t timeoutMs, const TaskQueue& queue) const;

private:
    // Cached QoS servers
    mutable Vector<Server> m_servers;
};

} // namespace QoS
} // namespace PlayFab

#pragma once

#if defined (PLAYFAB_PLATFORM_WINDOWS) || defined (PLAYFAB_PLATFORM_XBOX)

#include "QoS.h"
#include "QoSResult.h"
#include "QoSSocket.h"
#include <Events/EventsDataModels.h>

#include <chrono>
#include <future>
#include <unordered_map>

namespace PlayFab
{
    class PlayFabEventsInstanceAPI;
    class PlayFabMultiplayerInstanceAPI;

    namespace QoS
    {
        class PlayFabQoSApi
        {
        public:
            PlayFabQoSApi();

            // Runs a QoS operation asynchronously. The operation pings a set of regions and returns a result with average response times.
            std::future<QoSResult> GetQoSResultAsync(unsigned int numThreads, unsigned int timeoutMs = DEFAULT_TIMEOUT_MS);

            // Runs a QoS operation synchronously. The operation pings a set of regions and returns a result with average response times.
            QoSResult GetQoSResult(unsigned int numThreads, unsigned int timeoutMs = DEFAULT_TIMEOUT_MS);

        private:
            SharedPtr<PlayFabEventsInstanceAPI> eventsApi;
            SharedPtr<PlayFabMultiplayerInstanceAPI> multiplayerApi;

            Vector<String> GetPingList(unsigned int serverCount);
            void InitializeAccumulatedPingResults(UnorderedMap<String, PingResult>& accumulatedPingResults);
            int SetupSockets(Vector<SharedPtr<QoSSocket>>& sockets, unsigned int numThreads, unsigned int timeoutMs);
            void InitializeAsyncPingResults(Vector<std::future<PingResult>>& asyncPingResults);
            void PingServers(const Vector<String>& pings, Vector<std::future<PingResult>>& asyncPingResults, const Vector<SharedPtr<QoSSocket>>& sockets, UnorderedMap<String, PingResult>& accumulatedPingResults, unsigned int timeoutMs);
            void UpdateAccumulatedPingResult(const PingResult& result, const String& region, UnorderedMap<String, PingResult>& accumulatedPingResults, unsigned int timeoutMs);
            QoSResult GetResult(unsigned int numThreads, unsigned int timeoutMs);

            void PingThunderheadForServerList();
            void SendResultsToPlayFab(const QoSResult& result);

            static PingResult GetQoSResultForRegion(SharedPtr<QoSSocket> socket);

        private:
            const int numOfPingIterations = NUM_OF_PING_ITERATIONS; // Number of pings to do to each server, to calculate an average latency.
            const std::chrono::milliseconds threadWaitTimespan = std::chrono::milliseconds(THREAD_WAIT_MS);

            UnorderedMap<String, String> regionMap;
            bool listQosServersCompleted;
        };
    }
}
#endif // defined (PLAYFAB_PLATFORM_WINDOWS) || defined (PLAYFAB_PLATFORM_XBOX)
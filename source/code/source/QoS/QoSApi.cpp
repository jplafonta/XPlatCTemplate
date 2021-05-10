#include "stdafx.h"
#include "QoSApi.h"
#include "QoSSocket.h"
#include <Multiplayer/MultiplayerDataModels.h>
#include <Events/EventsDataModels.h>

using namespace PlayFab::MultiplayerModels;
using namespace PlayFab::EventsModels;

namespace PlayFab
{
namespace QoS
{

QoSAPI::QoSAPI(SharedPtr<HttpClient const> httpClient, SharedPtr<AuthTokens const> tokens) :
    m_eventsApi{ httpClient, tokens },
    m_multiplayerApi{ httpClient, tokens }
{
}

AsyncOp<Measurements> QoSAPI::GetMeasurements(uint32_t pingIterations, uint32_t timeoutMs, const TaskQueue& queue) const
{
    TaskQueue workerQueue{ queue.DeriveWorkerQueue() };

    return GetServers(workerQueue).Then([this, pingIterations, timeoutMs, workerQueue](Result<void> result) -> AsyncOp<Measurements>
    {
        RETURN_IF_FAILED(result.hr);
        return PingServers(pingIterations, timeoutMs, workerQueue);
    }
    ).Then([this, workerQueue](Result<Measurements> result)
    {
        // Upload result to PlayFab
        JsonValue eventJson{ rapidjson::kObjectType };
        JsonUtils::ObjectAddMember(eventJson, "ErrorCode", static_cast<uint64_t>(result.hr));

        if (Succeeded(result))
        {
            auto& measurements{ result.Payload() };
            JsonValue regionResultsJson{ rapidjson::kArrayType };

            for (auto i = 0u; i < measurements.regionResultsCount; ++i)
            {
                auto& regionResult{ measurements.regionResults[i] };
                JsonValue regionResultJson{ rapidjson::kObjectType };

                JsonUtils::ObjectAddMember(regionResultJson, "Region", regionResult->region);
                JsonUtils::ObjectAddMember(regionResultJson, "LatencyMs", regionResult->averageLatencyMs);
                JsonUtils::ObjectAddMember(regionResultJson, "SuccessfulPingCount", regionResult->successfulPingCount);
                JsonUtils::ObjectAddMember(regionResultJson, "FailedPingCount", regionResult->failedPingCount);
                regionResultsJson.PushBack(regionResultJson, JsonUtils::allocator);
            }

            JsonUtils::ObjectAddMember(eventJson, "RegionResults", std::move(regionResultsJson));
        }

        PlayFabEventsWriteEventsRequest request{};
        PlayFabEventsEventContents event{};
        Vector<PlayFabEventsEventContents*> events{ &event };

        JsonStringBuffer stringBuffer{ &JsonUtils::allocator };
        JsonWriter writer{ stringBuffer };
        eventJson.Accept(writer);
        auto eventJsonString = stringBuffer.GetString();

        event.name = "qos_result";
        event.eventNamespace = "playfab.servers";
        event.payload.stringValue = eventJsonString;
        request.events = events.data();
        request.eventsCount = static_cast<uint32_t>(events.size());

        return m_eventsApi.WriteTelemetryEvents(request, workerQueue).Then([qosResult = std::move(result) ](Result<WriteEventsResponse> result)
        {
            UNREFERENCED_PARAMETER(result); // TODO
            // Do we care to wait on this result before returning qosResult to client?
            return qosResult;
        });
    });
}

AsyncOp<void> QoSAPI::GetServers(const TaskQueue& queue) const
{
    if (!m_servers.empty())
    {
        // If we have a cached server list don't retrieve it again
        return S_OK;
    }

    ListQosServersForTitleRequest request{};
    return m_multiplayerApi.ListQosServersForTitle(request, queue).Then([this](Result<ListQosServersForTitleResponse> result)
    {
        if (Succeeded(result))
        {
            auto& resultModel{ result.Payload() };
            for (auto i = 0u; i < resultModel.qosServersCount; ++i)
            {
                auto& server = resultModel.qosServers[i];
                m_servers.emplace_back(Server{ server->region, server->serverUrl });
            }
        }
        return Result<void>{ result.hr, result.errorMessage.data() };
    });
}

AsyncOp<Measurements> QoSAPI::PingServers(uint32_t pingIterations, uint32_t timeoutMs, const TaskQueue& queue) const
{
    if (m_servers.empty())
    {
        // This means we've successfully queried the QoS servers but there were't any. Return an empty set of measurements
        return Result<Measurements>{ Measurements{} };
    }

    struct PingServersContext
    {
        TaskQueue queue;
        std::atomic<size_t> pendingPings;
        std::mutex mutex;
        UnorderedMap<String, RegionResult> regionResults;
        SharedPtr<AsyncOpContext<Measurements>> asyncOpContext;
    };

    auto context = SharedPtr<PingServersContext>();
    context->queue = queue;
    context->pendingPings = m_servers.size() * pingIterations;
    context->asyncOpContext = MakeShared<AsyncOpContext<Measurements>>();

    for (auto i = 0u; i < pingIterations; ++i)
    {
        for (const auto& server : m_servers)
        {
            queue.RunWork([timeoutMs, context, server = Server{ server }]
            {
                // TODO should we pre-allocate/reuse sockets? if so how many?
                auto makeSocketResult = QoSSocket::MakeAndConfigure(timeoutMs);
                if (Succeeded(makeSocketResult))
                {
                    auto socket = makeSocketResult.ExtractPayload();
                    auto pingResult = socket->PingServer(server.address.data());

                    std::lock_guard<std::mutex> lock{ context->mutex };
                    auto& regionResult = context->regionResults.try_emplace(server.address, server.address).first->second;
                    regionResult.AddPingResult(std::move(pingResult));
                }
                else
                {
                    // TODO how to handle socket init failure? When does this actually happen
                    assert(false);
                }

                if (--context->pendingPings == 0)
                {
                    // If this is the last ping, complete the asyncOp
                    context->asyncOpContext->Complete(Measurements{ context->regionResults });
                }
            });
        }
    }

    return context->asyncOpContext;
}

} // namespace QoS
} // namespace PlayFab

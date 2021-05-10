#include "stdafx.h"
#include "QoSDataModels.h"

namespace PlayFab
{
namespace QoS
{

RegionResult::RegionResult(const String& _region) :
    PlayFabQoSRegionResult{},
    m_region{ _region }
{
    region = m_region.data();
}

RegionResult::RegionResult(const RegionResult& src) :
    PlayFabQoSRegionResult{ src },
    m_region{ src.region }
{
    region = m_region.data();
}

RegionResult& RegionResult::operator=(const RegionResult& src)
{
    m_region = src.m_region;
    region = m_region.data();
    averageLatencyMs = src.averageLatencyMs;
    successfulPingCount = src.successfulPingCount;
    failedPingCount = src.failedPingCount;
    return *this;
}

void RegionResult::AddPingResult(Result<uint32_t> pingResult)
{
    // TODO need to make sure timeouts are handled correctly here
    if (Succeeded(pingResult))
    {
        ++successfulPingCount;
        m_totalLatencyMs += pingResult.Payload();
        averageLatencyMs = m_totalLatencyMs / successfulPingCount;
    }
    else
    {
        ++failedPingCount;
    }
}

Measurements::Measurements() : PlayFabQoSMeasurements {}
{
}

Measurements::Measurements(const UnorderedMap<String, RegionResult>& regionResultsMap) :
    PlayFabQoSMeasurements{},
    m_regionResults{ SortRegionResults(regionResultsMap) }
{
    regionResults = m_regionResults.Empty() ? nullptr : m_regionResults.Data();
    regionResultsCount = static_cast<uint32_t>(m_regionResults.Size());
}

Measurements::Measurements(const Measurements& src) :
    PlayFabQoSMeasurements{ src },
    m_regionResults{ src.m_regionResults }
{
    regionResults = m_regionResults.Empty() ? nullptr : m_regionResults.Data();
}

Vector<RegionResult> Measurements::SortRegionResults(const UnorderedMap<String, RegionResult>& regionResultsMap)
{
    Vector<RegionResult> regionResults;
    regionResults.reserve(regionResultsMap.size());

    std::transform(regionResultsMap.begin(), regionResultsMap.end(), std::back_inserter(regionResults), [](const auto& mapEntry)
    {
        return mapEntry.second;
    });

    std::sort(regionResults.begin(), regionResults.end(), [](const RegionResult& r, const RegionResult& l)
    {
        return r.averageLatencyMs < l.averageLatencyMs && r.successfulPingCount > 0;
    });

    return regionResults;
}

} // namespace QoS
} // namespace PlayFab

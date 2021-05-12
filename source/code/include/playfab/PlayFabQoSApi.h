// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

#include <playfab/PlayFabEntity.h>
#include <httpClient/async.h>

extern "C"
{

/// <summary>
/// The result of pinging a QoS server.
/// </summary>
typedef struct PlayFabQoSRegionResult
{
    /// <summary>
    /// The region the server is locationed in.
    /// </summary>
    const char* region;

    /// <summary>
    /// Average latency of successful server pings.
    /// </summary>
    uint32_t averageLatencyMs;

    /// <summary>
    /// Number of successful pings used to compute average latency.
    /// </summary>
    uint32_t successfulPingCount;

    /// <summary>
    /// Number of failed ping attempts.
    /// </summary>
    uint32_t failedPingCount;

} PlayFabQoSRegionResult;

/// <summary>
/// QoS measurements for multiple regions.
/// </summary>
typedef struct PlayFabQoSMeasurements
{
    /// <summary>
    /// List of QoS measurements for different regions.
    /// </summary>
    PlayFabQoSRegionResult const* const* regionResults;

    /// <summary>
    /// Count of regionResults.
    /// </summary>
    uint32_t regionResultsCount;

} PlayFabQoSMeasurements;

/// <summary>
/// Take QoS measurements and upload them to PlayFab. The operation first queries for the QoS servers, pings each of them
/// to determine response time, and finally uploads that result to PlayFab. The QoS measuresments are also returned
/// to the caller.
/// </summary>
/// <param name="entityHandle">PlayFabEntityHandle returned from a auth call.</param>
/// <param name="pingIterations">The number of times to ping each server to compute average latency.</param>
/// <param name="timeoutMs">The timeout (in milliseconds) for each ping attempt.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// If successful, call <see cref="PlayFabQoSGetMeasurementsGetResult"/> to get the result.
/// </remarks>
HRESULT PlayFabQoSGetMeasurmentsAsync(
    _In_ PlayFabEntityHandle entityHandle,
    _In_ uint32_t pingIterations,
    _In_ uint32_t timeoutMs,
    _Inout_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Gets the result of a successful PlayFabQoSGetMeasurmentsAsync call.
/// </summary>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <param name="resultHandle">Opaque handle to the result object.</param>
/// <param name="result">Pointer to the result object.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// The lifetime of the result object is tied to the result handle. When the result is no longer needed, call
/// PlayFabResultCloseHandle to release the result object.
/// </remarks>
HRESULT PlayFabQoSGetMeasurementsGetResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* resultHandle,
    _Outptr_ PlayFabQoSMeasurements** result
) noexcept;

}

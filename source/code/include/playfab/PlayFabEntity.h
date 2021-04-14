// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

#include <httpClient/pal.h>
#include <httpClient/async.h>
#include <playfab/PlayFabAuthenticationDataModels.h>

extern "C"
{

/// <summary>
/// Handle to an authenticated Entity (Player, Server, etc.). Contains the auth tokens needed to make PlayFab service
/// calls. When no longer needed, the Entity handle must be closed with PlayFabEntityCloseHandle.
/// </summary>
typedef struct PlayFabEntity* PlayFabEntityHandle;

/// <summary>
/// Get the result from a PlayFab authentication API. See PlayFab*AuthApi.h for the various authentication options.
/// </summary>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <param name="entityHandle">Entity handle which can be used to call other PlayFab APIs.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// If the auth call fails, entityHandle with be null. Otherwise, the handle must be closed with PlayFabEntityCloseHandle
/// when it is no longer needed.
/// </remarks>
HRESULT PlayFabGetAuthResult(
    _Inout_ XAsyncBlock* async,
    _Out_ PlayFabEntityHandle* entityHandle
) noexcept;

/// <summary>
/// Duplicates a PlayFabEntityHandle.
/// </summary>
/// <param name="entityHandle">Entity handle to duplicate.</param>
/// <param name="duplicatedEntityHandle">The duplicated handle.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Both the duplicated handle and the original handle need to be closed with PlayFabEntityCloseHandle when they
/// are no longer needed.
/// </remarks>
HRESULT PlayFabEntityDuplicateHandle(
    _In_ PlayFabEntityHandle entityHandle,
    _Out_ PlayFabEntityHandle* duplicatedEntityHandle
) noexcept;

/// <summary>
/// Closes a PlayFabEntityHandle.
/// </summary>
/// <param name="entityHandle">Entity handle to close.</param>
/// <returns>Result code for this API operation.</returns>
void PlayFabEntityCloseHandle(
    _In_ PlayFabEntityHandle entityHandle
) noexcept;

/// <summary>
/// Method to exchange a legacy AuthenticationTicket for an EntityToken or to refresh a still valid Entity Token. The entityHandle
/// will be updated with the returned EntityToken and it will be used in future PlayFab calls.
/// </summary>
/// <param name="entityHandle">Existing PlayFabEntityHandle returned from an auth call.</param>
/// <param name="request">Populated request object.</param>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XAsyncGetStatus"/> to get the status of the operation.
/// </remarks>
HRESULT PlayFabEntityGetEntityTokenAsync(
    _In_ PlayFabEntityHandle entityHandle,
    _In_ const PlayFabAuthenticationGetEntityTokenRequest* request,
    _Inout_ XAsyncBlock* async
) noexcept;

}

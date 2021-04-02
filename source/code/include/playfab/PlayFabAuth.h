// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

#include <httpClient/pal.h>
#include <httpClient/async.h>

extern "C"
{

/// <summary>
/// Handle to an authenticated entity (Player, Server, etc.). Contains the auth info needed to make PlayFab service
/// calls. When no longer needed, auth handle must be closed with PlayFabAuthenticatedEntityCloseHandle.
/// </summary>
typedef struct PlayFabAuthContext* PlayFabAuthContextHandle;

/// <summary>
/// Get the result from a PlayFab authentication API. See PlayFab*AuthApi.h for the various authentication options.
/// </summary>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <param name="authHandle">AuthContext handle which can be used to call other PlayFab APIs.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// If the auth call fails, authHandle with be null. Otherwise, the handle must be closed with PlayFabAuthenticatedEntityCloseHandle
/// when it is no longer needed.
/// </remarks>
HRESULT PlayFabAuthGetAuthResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabAuthContextHandle* authHandle
) noexcept;

/// <summary>
/// Duplicates a PlayFabAuthenticatedEntityHandle.
/// </summary>
/// <param name="authHandle">Authenticated entity handle to duplicate.</param>
/// <param name="duplicatedAuthHandle">The duplicated handle.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Both the duplicated handle and the original handle need to be closed with PlayFabAuthenticatedEntityCloseHandle when they
/// are no longer needed.
/// </remarks>
HRESULT PlayFabAuthContextDuplicateHandle(
    _In_ PlayFabAuthContextHandle authHandle,
    _Out_ PlayFabAuthContextHandle* duplicatedAuthHandle
) noexcept;

/// <summary>
/// Closes a PlayFabAuthenticatedEntityHandle.
/// </summary>
/// <param name="authHandle">Authenticated entity handle to close.</param>
/// <returns>Result code for this API operation.</returns>
void PlayFabAuthContextCloseHandle(
    _In_ PlayFabAuthContextHandle authHandle
) noexcept;

}

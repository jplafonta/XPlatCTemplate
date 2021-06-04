// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

#include <httpClient/pal.h>
#include <httpClient/async.h>
#include <playfab/PlayFabSharedDataModels.h>
#include <playfab/PlayFabAuthenticationDataModels.h>

extern "C"
{

/// <summary>
/// Handle to an authenticated Entity (Player, Server, etc.). Contains the auth tokens needed to make PlayFab service
/// calls. When no longer needed, the Entity handle must be closed with PlayFabEntityCloseHandle.
/// </summary>
typedef struct PlayFabEntity* PlayFabEntityHandle;

/// <summary>
/// PlayFab EntityToken and its expiration time. Used internally to authenticate PlayFab service calls.
/// </summary>
typedef struct PlayFabEntityToken
{
    /// <summary>
    /// The token used to set X-EntityToken for all entity based API calls.
    /// </summary>
    const char* token;

    /// <summary>
    /// (Optional) The time the token will expire, if it is an expiring token, in UTC.
    /// </summary>
    time_t const* expiration;

} PlayFabEntityToken;

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
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XAsyncGetStatus"/> to get the status of the operation.
/// </remarks>
HRESULT PlayFabEntityGetEntityTokenAsync(
    _In_ PlayFabEntityHandle entityHandle,
    _Inout_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the PlayFabId (master_player_account Id) for an Entity.
/// </summary>
/// <param name="entityHandle">PlayFabEntityHandle returned from a auth call.</param>
/// <param name="playFabId">Returned pointer to the playFabId. Valid until the Entity object is cleaned up.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabEntityGetPlayFabId(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_ const char** playFabId
) noexcept;

/// <summary>
/// Get the Entity Id for an Entity.
/// </summary>
/// <param name="entityHandle">PlayFabEntityHandle returned from a auth call.</param>
/// <param name="entityId">Returned pointer to the entityId. Valid until the Entity object is cleaned up.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabEntityGetEntityId(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_ const char** entityId
) noexcept;

/// <summary>
/// Get the Entity type for an entity.
/// </summary>
/// <param name="entityHandle">PlayFabEntityHandle returned from a auth call.</param>
/// <param name="entityId">Returned pointer to the entityType. Valid until the Entity object is cleaned up.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabEntityGetEntityType(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_ const char** entityType
) noexcept;

/// <summary>
/// Get the Entity token.
/// </summary>
/// <param name="entityHandle">PlayFabEntityHandle returned from a auth call.</param>
/// <param name="entityToken">Returned pointer to the entityToken. The pointer is valid until the Entity object is cleaned up, though
/// the token may expire before then.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabEntityGetCachedEntityToken(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_ const PlayFabEntityToken** entityToken
) noexcept;

/// <summary>
/// Get combined player info. Will be null if combined player info was not requested requested during login (see <see cref="PlayFabClientGetPlayerCombinedInfoRequestParams"/>).
/// </summary>
/// <param name="entityHandle">PlayFabEntityHandle returned from a auth call.</param>
/// <param name="playerCombinedInfo">Returned pointer to player combined info. Valid until the Entity object is cleaned up.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Note that the returned data is only guaranteed to be up to date as of the login request - it will not be automatically refreshed.
/// To get updated combined player data, call <see cref="PlayFabClientGetPlayerCombinedInfoAsync"/>
/// </remarks>
HRESULT PlayFabEntityGetPlayerCombinedInfo(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_result_maybenull_ const PlayFabGetPlayerCombinedInfoResultPayload** playerCombinedInfo
) noexcept;

/// <summary>
/// Get last login time (prior to the login that resulted in this entityHandle). lastLoginTime will be set to null if entity has no previous login.
/// </summary>
/// <param name="entityHandle">PlayFabEntityHandle returned from a auth call.</param>
/// <param name="lastLoginTime">Returned pointer to the last login time. Valid until the Entity object is cleaned up.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabEntityGetLastLoginTime(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_result_maybenull_ const time_t** lastLoginTime
) noexcept;

/// <summary>
/// Get UserSettings, if applicable. If unavailable, userSettings will be set to null.
/// </summary>
/// <param name="entityHandle">PlayFabEntityHandle returned from a auth call.</param>
/// <param name="lastLoginTime">Returned pointer to the UserSettings. Valid until the Entity object is cleaned up.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabEntityGetUserSettings(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_result_maybenull_ const PlayFabUserSettings** userSettings
) noexcept;

/// <summary>
/// Get experimentation treatments for a user at the time of login. If unavailable, treatmentAssignment will be set to null.
/// </summary>
/// <param name="entityHandle">PlayFabEntityHandle returned from a auth call.</param>
/// <param name="lastLoginTime">Returned pointer to TreatmentAssignment. Valid until the Entity object is cleaned up.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabEntityGetTreatmentAssignment(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_result_maybenull_ const PlayFabTreatmentAssignment** treatmentAssignment
) noexcept;

}

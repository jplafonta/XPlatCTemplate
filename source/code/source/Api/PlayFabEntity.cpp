#include "stdafx.h"
#include <playfab/PlayFabEntity.h>
#include "Entity.h"
#include "ApiAsyncProviders.h"

using namespace PlayFab;

HRESULT PlayFabGetAuthResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabEntityHandle* entityHandle
) noexcept
{
    return XAsyncGetResult(async, nullptr, sizeof(PlayFabEntityHandle), entityHandle, nullptr);
}

HRESULT PlayFabEntityDuplicateHandle(
    _In_ PlayFabEntityHandle entityHandle,
    _Out_ PlayFabEntityHandle* duplicatedEntityHandle
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(duplicatedEntityHandle);

    *duplicatedEntityHandle = MakeUnique<PlayFabEntity>(*entityHandle).release();
    return S_OK;
}

void PlayFabEntityCloseHandle(
    _In_ PlayFabEntityHandle entityHandle
) noexcept
{
    UniquePtr<PlayFabEntity>{ entityHandle };
}

HRESULT PlayFabEntityRegisterTokenRefreshedCallback(
    _In_ PlayFabEntityHandle entityHandle,
    _In_ PlayFabEntityTokenRefreshedCallback* callback,
    _In_opt_ void* context,
    _Out_ PlayFabRegistrationToken* token
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(callback);
    RETURN_HR_INVALIDARG_IF_NULL(token);

    *token = entityHandle->entity->TokenRefreshedCallbacks.Register([callback, context](const EntityToken& newToken)
        {
            callback(&newToken, context);
        });

    return S_OK;
}

HRESULT PlayFabEntityUnregisterTokenRefreshedCallback(
    _In_ PlayFabEntityHandle entityHandle,
    _In_ PlayFabRegistrationToken token
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);

    entityHandle->entity->TokenRefreshedCallbacks.Unregister(token);
    return S_OK;
}

HRESULT PlayFabEntityGetEntityTokenAsync(
    _In_ PlayFabEntityHandle entityHandle,
    _In_ const PlayFabAuthenticationGetEntityTokenRequest* request,
    _Inout_ XAsyncBlock* async
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);

    auto provider = MakeAuthProvider(async, __FUNCTION__, std::bind(&Entity::GetEntityToken, entityHandle->entity.get(), *request, std::placeholders::_1));
    return Provider::Run(UniquePtr<Provider>(provider.release()));
}

HRESULT PlayFabEntityGetPlayFabId(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_ const char** playFabId
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(playFabId);

    *playFabId = entityHandle->entity->PlayFabId().data();
    return S_OK;
}

HRESULT PlayFabEntityGetEntityId(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_ const char** entityId
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(entityId);

    *entityId = entityHandle->entity->EntityId().data();
    return S_OK;
}


HRESULT PlayFabEntityGetEntityType(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_ const char** entityType
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(entityType);

    *entityType = entityHandle->entity->EntityType().data();
    return S_OK;
}

HRESULT PlayFabEntityGetCachedEntityToken(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_ const PlayFabEntityToken** entityToken
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(entityToken);

    *entityToken = &entityHandle->entity->EntityToken();
    return S_OK;
}

HRESULT PlayFabEntityGetPlayerCombinedInfo(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_result_maybenull_ const PlayFabGetPlayerCombinedInfoResultPayload** playerCombinedInfo
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(playerCombinedInfo);

    *playerCombinedInfo = entityHandle->entity->PlayerCombinedInfo(); 
    return S_OK;
}

HRESULT PlayFabEntityGetLastLoginTime(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_result_maybenull_ const time_t** lastLoginTime
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(lastLoginTime);

    *lastLoginTime = entityHandle->entity->LastLoginTime();
    return S_OK;
}

HRESULT PlayFabEntityGetUserSettings(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_result_maybenull_ const PlayFabUserSettings** userSettings
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(userSettings);

    *userSettings = entityHandle->entity->UserSettings();
    return S_OK;
}

HRESULT PlayFabEntityGetTreatmentAssignment(
    _In_ PlayFabEntityHandle entityHandle,
    _Outptr_result_maybenull_ const PlayFabTreatmentAssignment** treatmentAssignment
) noexcept 
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(treatmentAssignment);

    *treatmentAssignment = entityHandle->entity->TreatmentAssignment();
    return S_OK;
}

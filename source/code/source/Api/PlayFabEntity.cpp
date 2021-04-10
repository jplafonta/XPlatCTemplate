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

HRESULT PlayFabEntityGetEntityTokenAsync(
    _In_ PlayFabEntityHandle entityHandle,
    _In_ const PlayFabAuthenticationGetEntityTokenRequest* request,
    _Inout_ XAsyncBlock* async
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(request);

    auto provider = MakeProvider(async, std::bind(&Entity::GetEntityToken, entityHandle->entity.get(), *request, std::placeholders::_1));
    return Provider::Run(UniquePtr<Provider>(provider.release()));
}

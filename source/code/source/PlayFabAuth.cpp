#include "stdafx.h"
#include <playfab/PlayFabAuth.h>
#include "AuthContext.h"

using namespace PlayFab;

HRESULT PlayFabAuthGetAuthResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabAuthContextHandle* authHandle
) noexcept
{
    return XAsyncGetResult(async, nullptr, sizeof(PlayFabAuthContextHandle), authHandle, nullptr);
}

HRESULT PlayFabAuthContextDuplicateHandle(
    _In_ PlayFabAuthContextHandle authHandle,
    _Out_ PlayFabAuthContextHandle* duplicatedAuthHandle
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(authHandle);
    RETURN_HR_INVALIDARG_IF_NULL(duplicatedAuthHandle);

    *duplicatedAuthHandle = MakeUnique<PlayFabAuthContext>(*authHandle).release();
    return S_OK;
}

void PlayFabAuthContextCloseHandle(
    _In_ PlayFabAuthContextHandle authHandle
) noexcept
{
    UniquePtr<PlayFabAuthContext>{ authHandle };
}

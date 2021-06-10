#include "stdafx.h"
#include "GlobalState.h"
#include "BaseModel.h"
#include <httpClient/httpClient.h>

using namespace PlayFab;

STDAPI PlayFabMemSetFunctions(
    _In_opt_ PlayFabMemAllocFunction* memAllocFunc,
    _In_opt_ PlayFabMemFreeFunction* memFreeFunc
) noexcept
{
    RETURN_IF_FAILED(PlayFab::Detail::SetMemoryHooks(memAllocFunc, memFreeFunc));

    // Try to set the memory hooks for libHttpClient as well. If it has already be initialized, there is nothing we can do
    HRESULT hr = HCMemSetFunctions(memAllocFunc, memFreeFunc);
    if (FAILED(hr) && hr != E_HC_ALREADY_INITIALISED)
    {
        return hr;
    }

    return S_OK;
}

STDAPI PlayFabMemGetFunctions(
    _Out_ PlayFabMemAllocFunction** memAllocFunc,
    _Out_ PlayFabMemFreeFunction** memFreeFunc
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(memAllocFunc);
    RETURN_HR_INVALIDARG_IF_NULL(memFreeFunc);

    auto& hooks = PlayFab::Detail::GetMemoryHooks();
    *memAllocFunc = hooks.alloc;
    *memFreeFunc = hooks.free;

    return S_OK;
}

HRESULT PlayFabInitialize(
    _In_z_ const char* titleId,
    _In_opt_ XTaskQueueHandle backgroundQueue,
    _Outptr_ PlayFabStateHandle* stateHandle
) noexcept
{
    return PlayFabGlobalState::Create(titleId, nullptr, backgroundQueue, stateHandle);
}

HRESULT PlayFabAdminInitialize(
    _In_z_ const char* titleId,
    _In_z_ const char* secretKey,
    _In_opt_ XTaskQueueHandle backgroundQueue,
    _Outptr_ PlayFabStateHandle* stateHandle
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(secretKey);
    return PlayFabGlobalState::Create(titleId, secretKey, backgroundQueue, stateHandle);
}

HRESULT PlayFabCleanupAsync(
    _In_ PlayFabStateHandle stateHandle,
    _In_ XAsyncBlock* async
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(stateHandle);
    return stateHandle->CleanupAsync(async);
}

HRESULT PlayFabResultDuplicateHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabResultHandle* duplicatedHandle
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(resultHandle);
    RETURN_HR_INVALIDARG_IF_NULL(duplicatedHandle);

    *duplicatedHandle = MakeUnique<PlayFabResult>(*resultHandle).release();
    return S_OK;
}

void PlayFabResultCloseHandle(
    _In_ PlayFabResultHandle resultHandle
) noexcept
{
    UniquePtr<PlayFabResult>{ resultHandle };
}

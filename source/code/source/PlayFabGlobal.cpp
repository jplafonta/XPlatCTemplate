#include "stdafx.h"
#include "GlobalState.h"
#include <httpClient/httpClient.h>

STDAPI XblMemSetFunctions(
    _In_opt_ PlayFabMemAllocFunction* memAllocFunc,
    _In_opt_ PlayFabMemFreeFunction* memFreeFunc
) noexcept
{
    // Require that the hooks be set together
    if ((!memAllocFunc && memFreeFunc) || (memAllocFunc && !memFreeFunc))
    {
        return E_INVALIDARG;
    }

    PlayFab::Detail::SetMemoryHooks(memAllocFunc, memFreeFunc);

    // Try to set the memory hooks for libHttpClient as well. If it has already be initialized, there is nothing we can do
    HRESULT hr = HCMemSetFunctions(memAllocFunc, memFreeFunc);
    if (FAILED(hr) && hr != E_HC_ALREADY_INITIALISED)
    {
        return hr;
    }

    return S_OK;
}

STDAPI XblMemGetFunctions(
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
    _Outptr_ PlayFabStateHandle* stateHandle
) noexcept
{
    return PlayFabGlobalState::Create(stateHandle);
}

HRESULT PlayFabCleanupAsync(
    _In_ PlayFabStateHandle stateHandle,
    _In_ XAsyncBlock* async
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(stateHandle);
    return stateHandle->CleanupAsync(async);
}
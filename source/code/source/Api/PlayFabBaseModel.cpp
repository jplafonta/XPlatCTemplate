#include "stdafx.h"
#include <playfab/PlayFabBaseModel.h>
#include "BaseModel.h"

using namespace PlayFab;

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

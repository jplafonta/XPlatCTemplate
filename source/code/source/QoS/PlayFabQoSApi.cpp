#include "stdafx.h"
#include <playfab/PlayFabQoSApi.h>
#include "QoSApi.h"
#include "ApiAsyncProviders.h"
#include "Entity.h"

using namespace PlayFab;
using namespace PlayFab::QoS;

HRESULT PlayFabQoSGetMeasurmentsAsync(
    _In_ PlayFabEntityHandle entityHandle,
    _In_ uint32_t pingIterations,
    _In_ uint32_t timeoutMs,
    _Inout_ XAsyncBlock* async
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);

    auto provider = MakeProvider(async, __FUNCTION__, std::bind(&QoSAPI::GetMeasurements, &entityHandle->entity->QoSAPI, pingIterations, timeoutMs, std::placeholders::_1));
    return Provider::Run(UniquePtr<Provider>(provider.release()));
}

HRESULT PlayFabQoSGetMeasurementsGetResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* resultHandle,
    _Outptr_ PlayFabQoSMeasurements** result
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(result);

    RETURN_IF_FAILED(XAsyncGetResult(async, nullptr, sizeof(PlayFabResultHandle), resultHandle, nullptr));
    *result = (PlayFabQoSMeasurements*)(std::dynamic_pointer_cast<Measurements>((*resultHandle)->result).get());

    return S_OK;
}
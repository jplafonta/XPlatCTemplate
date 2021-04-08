// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "stdafx.h"
#include "AsyncProvider.h"

namespace PlayFab
{

Provider::Provider(_In_ XAsyncBlock* async) noexcept
    : m_async{ async }
{
}

HRESULT Provider::Run(_In_ UniquePtr<Provider>&& provider) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(provider->m_async);
    RETURN_IF_FAILED(XAsyncBegin(provider->m_async, provider.get(), nullptr, nullptr, XAsyncProvider));
    provider.release();
    return S_OK;
}

HRESULT Provider::Begin(TaskQueue&&)
{
    return Schedule(0);
}

AsyncOp<size_t> Provider::DoWork(TaskQueue&&)
{
    return Result<size_t>(size_t(0));
}

HRESULT Provider::GetResult(void*, size_t)
{
    assert(false);
    return E_UNEXPECTED;
}

HRESULT Provider::Schedule(uint32_t delay) const
{
    return XAsyncSchedule(m_async, delay);
}

void Provider::Complete(size_t resultSize)
{
    XAsyncComplete(m_async, S_OK, resultSize);
}

void Provider::Fail(HRESULT hr)
{
    XAsyncComplete(m_async, hr, 0);
}
HRESULT CALLBACK Provider::XAsyncProvider(_In_ XAsyncOp op, _Inout_ const XAsyncProviderData* data) noexcept
{
    auto provider{ static_cast<Provider*>(data->context) };
    switch (op)
    {
    case XAsyncOp::Begin:
        try
        {
            return provider->Begin(data->async->queue);
        }
        catch (...)
        {
            return CurrentExceptionToHR();
        }
    case XAsyncOp::DoWork:
        try
        {
            provider->DoWork(data->async->queue).Finally([data](Result<size_t> asyncResult)
            {
                XAsyncComplete(data->async, asyncResult.hr, asyncResult.Payload());
            });
        }
        catch (...)
        {
            return CurrentExceptionToHR();
        }
    case XAsyncOp::GetResult:
        try
        {
            return provider->GetResult(data->buffer, data->bufferSize);
        }
        catch (...)
        {
            return CurrentExceptionToHR();
        }
    case XAsyncOp::Cancel:
    {
        // Cancelation for libHttpClient requests not supported, so just wait until they complete
        return S_OK;
    }
    case XAsyncOp::Cleanup:
    {
        // Cleanup should only fail in catostrophic cases. Can't pass result to client 
        // at this point so die with exception.

        // Re-take ownership of provider, it will be destroyed here
        UniquePtr<Provider>{ provider };
        return S_OK;
    }
    default:
    {
        assert(false);
        return S_OK;
    }
    }
}

} // PlayFab

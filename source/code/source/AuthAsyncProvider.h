#pragma once

#include "AsyncProvider.h"
#include "AuthContext.h"

namespace PlayFab
{

// Helper class used to implement XAsync Providers for PlayFab auth calls.
template<typename AuthApiT, typename RequestT>
class AuthCallProvider: public Provider
{
public:
    using AuthCallT = AsyncOp<SharedPtr<AuthContext>> (AuthApiT::*)(const RequestT&, const TaskQueue&) const;

    AuthCallProvider(XAsyncBlock* async, const AuthApiT& api, AuthCallT authCall, const RequestT& request)
        : Provider{ async },
        m_api{ api },
        m_call{ authCall },
        m_request{ request }
    {
    }

protected:
    // Choosing to kick of async operation during Begin so that we don't have to copy the AuthApiT object
    HRESULT Begin(TaskQueue&& queue) override
    {
        (m_api.*m_call)(m_request, queue).Finally([this](Result<SharedPtr<AuthContext>> result)
        {
            if (Succeeded(result))
            {
                this->m_authContext = result.ExtractPayload();
                this->Complete(sizeof(PlayFabAuthContextHandle));
            }
            else
            {
                this->Fail(result.hr);
            }
        });

        return S_OK;
    }

    HRESULT GetResult(void* buffer, size_t bufferSize) override
    {
        assert(bufferSize == sizeof(PlayFabAuthContextHandle));
        auto contextPtr = static_cast<PlayFabAuthContext**>(buffer);
        *contextPtr = MakeUnique<PlayFabAuthContext>(m_authContext).release();
        return S_OK;
    }

private:
    const AuthApiT& m_api;
    AuthCallT m_call;
    const RequestT& m_request;
    SharedPtr<AuthContext> m_authContext;
};

}

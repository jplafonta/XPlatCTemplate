#pragma once

#include "AsyncProvider.h"
#include "AuthContext.h"
#include "PlayFabError.h"

namespace PlayFab
{

// Helper class used to implement XAsync Providers for PlayFab auth calls.
template<typename AuthApiT, typename RequestT>
class AuthCallProvider: public Provider
{
public:
    using AuthCallT = void (AuthApiT::*)(const RequestT&, const TaskQueue&, ProcessApiCallback<SharedPtr<AuthContext>>, const ErrorCallback) const;

    AuthCallProvider(XAsyncBlock* async, const AuthApiT& api, AuthCallT authCall, const RequestT& request)
        : Provider{ async },
        m_api{ api },
        m_call{ authCall },
        m_request{ request }
    {
    }

protected:
    HRESULT Begin(TaskQueue&& queue) override
    {
        (m_api.*m_call)(m_request, queue, 
            [this](const SharedPtr<AuthContext>& authContext)
            {
                this->m_authContext = authContext;
                this->Complete(sizeof(PlayFabAuthContextHandle));
            },
            [this](const PlayFabError& /*error*/)
            {
                // TODO translate error to HRESULT
                this->Fail(E_FAIL);
            }
            );
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
    // Consider copying the API, using SharedPtr, or storing a std::bind function
    const AuthApiT& m_api;
    AuthCallT m_call;
    const RequestT& m_request;
    SharedPtr<AuthContext> m_authContext;
};

}

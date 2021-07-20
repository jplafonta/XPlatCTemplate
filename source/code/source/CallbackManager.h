#pragma once

#include <playfab/PlayFabGlobal.h>

namespace PlayFab
{

// Class to help registering, unregistering, and invoking client callbacks
template<typename CallbackT>
class CallbackManager
{
public:
    CallbackManager();
    CallbackManager(const CallbackManager&) = delete;
    CallbackManager& operator=(const CallbackManager&) = delete;
    ~CallbackManager() = default;

    PlayFabRegistrationToken Register(CallbackT&& callback);
    void Unregister(PlayFabRegistrationToken token);

    template<typename... Args>
    typename std::enable_if_t<std::is_void_v<std::result_of_t<CallbackT(Args...)>>> Invoke(Args... args) const;

private:
    mutable std::recursive_mutex m_mutex; // recursive to allow unregistration within callback

    Map<PlayFabRegistrationToken, CallbackT> m_callbacks;
    PlayFabRegistrationToken m_nextToken;
};

//------------------------------------------------------------------------------
// Template implementations
//------------------------------------------------------------------------------

namespace Detail
{
// Arbitrary first token thats recognizable when debugging
PlayFabRegistrationToken const kFirstCallbackToken = 0xAAAA000000000000;

}

template<typename CallbackT>
CallbackManager<CallbackT>::CallbackManager() : m_nextToken{ Detail::kFirstCallbackToken }
{
}

template<typename CallbackT>
PlayFabRegistrationToken CallbackManager<CallbackT>::Register(CallbackT&& callback)
{
    std::unique_lock<std::recursive_mutex> lock{ m_mutex };

    assert(callback);

    PlayFabRegistrationToken token{ m_nextToken++ };

    auto res = m_callbacks.emplace(token, std::move(callback));
    assert(res.second);
    UNREFERENCED_PARAMETER(res.second);

    return token;
}

template<typename CallbackT>
void CallbackManager<CallbackT>::Unregister(PlayFabRegistrationToken token)
{
    std::unique_lock<std::recursive_mutex> lock{ m_mutex };

    assert(!(token < Detail::kFirstCallbackToken));
    assert(token < m_nextToken);

    m_callbacks.erase(token);
}

template<typename CallbackT> template<typename... Args>
typename std::enable_if_t<std::is_void_v<std::result_of_t<CallbackT(Args...)>>> CallbackManager<CallbackT>::Invoke(Args... args) const
{
    std::unique_lock<std::recursive_mutex> lock{ m_mutex };

    // Copy callbacks so that reentrant unregistration doesn't break iteration
    auto callbacks{ m_callbacks };

    for (auto& pair : callbacks)
    {
        pair.second(args...);
    }
}

}

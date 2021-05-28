#include "stdafx.h"
#include "AuthTokens.h"

#define AS_STRING(charPtr) (charPtr ? String{ charPtr } : String{})

namespace PlayFab
{

EntityToken::EntityToken(const EntityTokenResponse& tokenResponse) :
    PlayFabEntityToken{},
    m_token{ AS_STRING(tokenResponse.entityToken) },
    m_expiration{ tokenResponse.tokenExpiration ? *tokenResponse.tokenExpiration : StdExtra::optional<time_t>{} }
{
    token = m_token.empty() ? nullptr : m_token.data();
    expiration = m_expiration ? m_expiration.operator->() : nullptr;
}

EntityToken::EntityToken(const AuthenticationModels::GetEntityTokenResponse& tokenResponse) :
    PlayFabEntityToken{},
    m_token{ tokenResponse.entityToken },
    m_expiration{ tokenResponse.tokenExpiration }
{
    token = m_token.empty() ? nullptr : m_token.data();
    expiration = m_expiration ? m_expiration.operator->() : nullptr;
}

EntityToken::EntityToken(const EntityToken& src) :
    m_token{ src.m_token },
    m_expiration{ src.m_expiration }
{
    token = m_token.empty() ? nullptr : m_token.data();
    expiration = m_expiration ? m_expiration.operator->() : nullptr;
}

EntityToken::EntityToken(EntityToken&& src) :
    PlayFabEntityToken{ src },
    m_token{ std::move(src.m_token) },
    m_expiration{ src.m_expiration }
{
    expiration = m_expiration ? m_expiration.operator->() : nullptr;
}

AuthTokens::AuthTokens(const LoginResult& result) :
    m_sessionTicket{ result.sessionTicket }
{
    // Can this ever be missing from a LoginResult?
    assert(result.entityToken);
    m_entityTokens.emplace_front(*result.entityToken);
    m_entityToken = &m_entityTokens.front();
}

AuthTokens::AuthTokens(const ClientModels::RegisterPlayFabUserResult& result) : 
    m_sessionTicket{ result.sessionTicket }
{
    // Can this ever be missing from a RegisterPlayFabUserResult?
    assert(result.entityToken);
    m_entityTokens.emplace_front(*result.entityToken);
    m_entityToken = &m_entityTokens.front();
}

AuthTokens::AuthTokens(const AuthenticationModels::GetEntityTokenResponse& result)
{
    m_entityTokens.emplace_front(result);
    m_entityToken = &m_entityTokens.front();
}

EntityToken const& AuthTokens::EntityToken() const
{
    assert(m_entityToken);
    return *m_entityToken;
}

String const& AuthTokens::SessionTicket() const
{
    return m_sessionTicket;
}

void AuthTokens::UpdateEntityToken(const AuthenticationModels::GetEntityTokenResponse& getEntityTokenResponse)
{
    // Very unlikely that multiple threads will be updating token at once, but could theoretically happen
    // if title calls GetEntityToken multiple times simultatneously
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_entityTokens.emplace_front(getEntityTokenResponse);
    m_entityToken = &m_entityTokens.front();
}

}

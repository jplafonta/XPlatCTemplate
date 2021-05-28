#pragma once

#include <playfab/PlayFabEntity.h>
#include <Client/ClientDataModels.h>
#include <Authentication/AuthenticationDataModels.h>

namespace PlayFab
{

class EntityToken : public PlayFabEntityToken
{
public:
    EntityToken(const EntityTokenResponse& tokenResponse);
    EntityToken(const AuthenticationModels::GetEntityTokenResponse& tokenResponse);
    EntityToken(const EntityToken& src);
    EntityToken(EntityToken&& src);
    ~EntityToken() = default;

private:
    String m_token;
    StdExtra::optional<time_t> m_expiration;
};

// Auth tokens needed by to make PlayFab service calls
class AuthTokens
{
public:
    AuthTokens(const LoginResult& loginResult);
    AuthTokens(const ClientModels::RegisterPlayFabUserResult& registerUserResult);
    AuthTokens(const AuthenticationModels::GetEntityTokenResponse& getEntityTokenResponse);

    PlayFab::EntityToken const& EntityToken() const;
    String const& SessionTicket() const;

    void UpdateEntityToken(const AuthenticationModels::GetEntityTokenResponse& getEntityTokenResponse);

private:
    std::atomic<PlayFab::EntityToken const*> m_entityToken;
    List<PlayFab::EntityToken> m_entityTokens;
    String m_sessionTicket;
    std::mutex m_mutex;
};

}

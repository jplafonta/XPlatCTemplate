#pragma once

#include <Client/ClientDataModels.h>
#include <Server/ServerDataModels.h>
#include <Authentication/AuthenticationDataModels.h>

namespace PlayFab
{

class AuthContext
{
public:
    AuthContext(const ClientModels::LoginResult& loginResult);
    AuthContext(const ClientModels::RegisterPlayFabUserResult& registerUserResult);
    AuthContext(const ServerModels::ServerLoginResult& loginResult);
    AuthContext(const AuthenticationModels::GetEntityTokenResponse& getEntityTokenResponse);

private:
    // DisableAdvertising is provided for completeness, but changing it is not suggested
    // Disabling this may prevent your advertising-related PlayFab marketplace partners from working correctly
    bool m_disableAdvertising; 
    String m_playFabId; // Master_Player_Entity Id for the Player that logged in
    String m_clientSessionTicket; // Client session ticket that is used as an authentication token in many PlayFab API methods.
    String m_advertisingIdType; // Set this to the appropriate AD_TYPE_X constant below
    String m_advertisingIdValue; // Set this to corresponding device value
    String m_entityId; // Entity Id for the active entity
    String m_entityType; // Entity Type for the active entity
    String m_entityToken; // User's entity token. Entity tokens are required by all Entity API methods.
};

}

struct PlayFabAuthContext
{
    PlayFabAuthContext(PlayFab::SharedPtr<PlayFab::AuthContext> context_) : context{ context_ } {}
    PlayFabAuthContext(const PlayFabAuthContext&) = default;
    ~PlayFabAuthContext() = default;

    PlayFab::SharedPtr<PlayFab::AuthContext> context;
};

#include "stdafx.h"
#include "AuthContext.h"

namespace PlayFab
{

AuthContext::AuthContext(const ClientModels::LoginResult& result) :
    m_playFabId{ result.playFabId },
    m_clientSessionTicket{ result.sessionTicket },
    m_entityId{ result.entityToken->entity->id },
    m_entityType{ result.entityToken->entity->type },
    m_entityToken{ result.entityToken->entityToken }
{
}

AuthContext::AuthContext(const ClientModels::RegisterPlayFabUserResult& result) :
    m_playFabId{ result.playFabId },
    m_clientSessionTicket{ result.sessionTicket },
    m_entityId{ result.entityToken->entity->id },
    m_entityType{ result.entityToken->entity->type },
    m_entityToken{ result.entityToken->entityToken }
{
}


AuthContext::AuthContext(const ServerModels::ServerLoginResult& result) :
    m_playFabId{ result.playFabId },
    m_clientSessionTicket{ result.sessionTicket },
    m_entityId{ result.entityToken->entity->id },
    m_entityType{ result.entityToken->entity->type },
    m_entityToken{ result.entityToken->entityToken }
{
}

AuthContext::AuthContext(const AuthenticationModels::GetEntityTokenResponse& result) :
    m_entityId{ result.entity->id },
    m_entityType{ result.entity->type },
    m_entityToken{ result.entityToken }
{
}

}

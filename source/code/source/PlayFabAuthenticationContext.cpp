#include "stdafx.h"

#include "PlayFabAuthenticationContext.h"

namespace PlayFab
{
    PlayFabAuthenticationContext::PlayFabAuthenticationContext()
    {
        ForgetAllCredentials();
    }

    void PlayFabAuthenticationContext::ForgetAllCredentials()
    {
#if !defined(DISABLE_PLAYFABCLIENT_API)
        playFabId.clear();
        clientSessionTicket.clear();
        advertisingIdType.clear();
        advertisingIdValue.clear();
#endif
        entityId.clear();
        entityType.clear();
        entityToken.clear();
    }

    void SetIfNotNull(const String& input, String& output)
    {
        if (!input.empty())
        {
            output = input;
        }
    }

    void PlayFabAuthenticationContext::HandlePlayFabLogin(
        const String& _playFabId,
        const String& _clientSessionTicket,
        const String& _entityId,
        const String& _entityType,
        const String& _entityToken
    )
    {
        SetIfNotNull(_playFabId, playFabId);
        SetIfNotNull(_clientSessionTicket, clientSessionTicket);
        SetIfNotNull(_entityId, entityId);
        SetIfNotNull(_entityType, entityType);
        SetIfNotNull(_entityToken, entityToken);
    }

    bool PlayFabAuthenticationContext::IsClientLoggedIn()
    {
        return !clientSessionTicket.empty();
    }

    bool PlayFabAuthenticationContext::IsEntityLoggedIn()
    {
        return !entityToken.empty();
    }
}

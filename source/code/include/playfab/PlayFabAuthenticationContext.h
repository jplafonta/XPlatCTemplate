#pragma once

#include <string>

namespace PlayFab
{
    /// <summary>
    /// Container for PlayFab authentication credentials data.
    /// </summary>
    class PlayFabAuthenticationContext
    {
#if !defined(DISABLE_PLAYFABCLIENT_API)
    public: // Client-only variables should only be visible when appropriate
#else
    private: // But, static library memory size and alloc issues mean it always needs to exist
#endif
        // DisableAdvertising is provided for completeness, but changing it is not suggested
        // Disabling this may prevent your advertising-related PlayFab marketplace partners from working correctly
        bool disableAdvertising;
        String playFabId; // Master_Player_Entity Id for the Player that logged in
        String clientSessionTicket; // Client session ticket that is used as an authentication token in many PlayFab API methods.
        String advertisingIdType; // Set this to the appropriate AD_TYPE_X constant below
        String advertisingIdValue; // Set this to corresponding device value

    public:
        String entityId; // Entity Id for the active entity
        String entityType; // Entity Type for the active entity
        String entityToken; // User's entity token. Entity tokens are required by all Entity API methods.

        PlayFabAuthenticationContext();
        PlayFabAuthenticationContext(const PlayFabAuthenticationContext& other) = delete;
        PlayFabAuthenticationContext(PlayFabAuthenticationContext&& other) = delete;
        PlayFabAuthenticationContext& operator=(const PlayFabAuthenticationContext& other) = delete;
        PlayFabAuthenticationContext& operator=(PlayFabAuthenticationContext&& other) = delete;
        ~PlayFabAuthenticationContext() = default;

        void HandlePlayFabLogin(const String& _playFabId, const String& _clientSessionTicket, const String& _entityId, const String& _entityType, const String& _entityToken);
        void ForgetAllCredentials();
        bool IsClientLoggedIn();
        bool IsEntityLoggedIn();
    };
}

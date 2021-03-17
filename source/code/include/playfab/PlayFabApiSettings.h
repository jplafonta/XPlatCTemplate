#pragma once

#include <string>
#include <map>

namespace PlayFab
{
    /// <summary>
    /// The settings that can be used (optionally) by instance versions of PlayFab APIs.
    /// </summary>
    class PlayFabApiSettings
    {
#if defined(ENABLE_PLAYFABSERVER_API) || defined(ENABLE_PLAYFABADMIN_API)
    public: // Server-only variables should only be visible when appropriate
#else
    private: // But, static library memory size and alloc issues mean it always needs to exist
#endif
        String developerSecretKey; // Developer secret key. These keys can be used in development environments.

    public:
        Map<String, String> requestGetParams;

        String baseServiceHost; // The base for a PlayFab service host
        String titleId; // You must set this value for PlayFabSdk to work properly (found in the Game Manager for your title, at the PlayFab Website)

        PlayFabApiSettings();
        PlayFabApiSettings(const PlayFabApiSettings& other) = delete;
        PlayFabApiSettings(PlayFabApiSettings&& other) = delete;
        PlayFabApiSettings& operator=(const PlayFabApiSettings& other) = delete;
        PlayFabApiSettings& operator=(PlayFabApiSettings&& other) = delete;
        ~PlayFabApiSettings() = default;

        String GetUrl(const String& urlPath) const;
    };
}

#include <stdafx.h>

#include <playfab/PlayFabApiSettings.h>
#include <playfab/PlayFabSettings.h>

namespace PlayFab
{
    PlayFabApiSettings::PlayFabApiSettings() :
        baseServiceHost(PlayFabSettings::productionEnvironmentURL)
    {
        requestGetParams["sdk"] = PlayFabSettings::versionString;

        // Don't let PlayFabSettings::staticSettings pull titleId from itself
        if (PlayFabSettings::staticSettings != nullptr)
        {
            titleId = PlayFabSettings::staticSettings->titleId;
        }
    }

    String PlayFabApiSettings::GetUrl(const char* urlPath) const
    {
        Stringstream fullUrl;
        fullUrl << "https://" << titleId << baseServiceHost << urlPath;

        bool firstParam = true;
        for (auto const& paramPair : requestGetParams)
        {
            if (firstParam)
            {
                fullUrl << "?";
                firstParam = false;
            }
            else
            {
                fullUrl << "&";
            }
            fullUrl << paramPair.first;
            fullUrl << "=";
            fullUrl << paramPair.second;
        }

        return fullUrl.str();
    }
}

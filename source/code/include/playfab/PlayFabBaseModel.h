#pragma once

#include <playfab/InternalMemory.h>
#include <playfab/PlayFabPlatformMacros.h>
#include <playfab/PlayFabPlatformTypes.h>
#include <playfab/PlayFabPlatformUtils.h>
#include <playfab/StdOptional.h>
#include <JsonUtils.h>

#include <assert.h>
#include <functional>
#include <list>
#include <memory>

namespace PlayFab
{
    class PlayFabAuthenticationContext;

    template<typename ResType> using ProcessApiCallback = std::function<void(const ResType& result)>;

    /// <summary>
    /// Base class for all PlayFab Models
    /// Provides interface for converting to/from json
    /// </summary>
    struct PlayFabBaseModel
    {
        virtual ~PlayFabBaseModel() {}
        virtual void FromJson(const JsonValue& input) = 0;
        virtual JsonValue ToJson() const = 0;
    };

    /// <summary>
    /// Base class for all PlayFab Requests
    /// Adds a parameter that controls how requests are threaded
    /// </summary>
    struct PlayFabRequestCommon : public PlayFabBaseModel
    {
        /// <summary>
        /// An optional authentication context, it can used in multi-user scenarios
        /// </summary>
        SharedPtr<PlayFabAuthenticationContext> authenticationContext;
    };

    /// <summary>
    /// Base class for all PlayFab Results
    /// </summary>
    struct PlayFabResultCommon : public PlayFabBaseModel
    {
        JsonValue Request;

        PlayFabResultCommon() = default;
        PlayFabResultCommon(const PlayFabResultCommon& src)
        {
            JsonUtils::FromJson(src.Request, Request);
        }
    };

    /// <summary>
    /// Base class for all PlayFab Login method Results
    /// </summary>
    struct PlayFabLoginResultCommon : public PlayFabResultCommon
    {
        /// <summary>
        /// An authentication context returned by Login methods, it can used in multi-user scenarios
        /// </summary>
        SharedPtr<PlayFabAuthenticationContext> authenticationContext;
    };
}

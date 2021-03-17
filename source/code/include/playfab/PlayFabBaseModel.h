#pragma once

#include <playfab/PlayFabPlatformMacros.h>
#include <playfab/PlayFabPlatformTypes.h>
#include <playfab/PlayFabJsonHeaders.h>
#include <playfab/PlayFabPlatformTypes.h>
#include <playfab/PlayFabPlatformUtils.h>
#include <playfab/StdOptional.h>

#include <assert.h>
#include <functional>
#include <list>
#include <memory>

namespace PlayFab
{
    class PlayFabAuthenticationContext;

    template<typename ResType> using ProcessApiCallback = std::function<void(const ResType& result, void* customData)>;

    /// <summary>
    /// Base class for all PlayFab Models
    /// Provides interface for converting to/from json
    /// </summary>
    struct PlayFabBaseModel
    {
        virtual ~PlayFabBaseModel() {}
        virtual void FromJson(const Json::Value& input) = 0;
        virtual Json::Value ToJson() const = 0;
    };

    /// <summary>
    /// Base class for all PlayFab Requests
    /// Adds a parameter that controls how requests are threaded
    /// </summary>
    struct PlayFabRequestCommon : public PlayFabBaseModel {
        /// <summary>
        /// An optional authentication context, it can used in multi-user scenarios
        /// </summary>
        SharedPtr<PlayFabAuthenticationContext> authenticationContext;
    };

    /// <summary>
    /// Base class for all PlayFab Results
    /// </summary>
    struct PlayFabResultCommon : public PlayFabBaseModel {
        Json::Value Request;
    };

    /// <summary>
    /// Base class for all PlayFab Login method Results
    /// </summary>
    struct PlayFabLoginResultCommon : public PlayFabResultCommon {
        /// <summary>
        /// An authentication context returned by Login methods, it can used in multi-user scenarios
        /// </summary>
        SharedPtr<PlayFabAuthenticationContext> authenticationContext;
    };

    // Utilities for [de]serializing time_t to/from json
    inline void ToJsonUtilT(const time_t input, Json::Value& output)
    {
        output = Json::Value(TimeTToIso8601String(input));
    }
    inline void FromJsonUtilT(const Json::Value& input, time_t& output)
    {
        if (input == Json::Value::null)
        {
            return;
        }
        output = Iso8601StringToTimeT(input.asString());
    }

    inline void ToJsonUtilT(const StdExtra::optional<time_t>& input, Json::Value& output)
    {
        if (input.isNull())
        {
            output = Json::Value();
        }
        else
        {
            ToJsonUtilT(static_cast<time_t>(input), output);
        }
    }

    inline void FromJsonUtilT(const Json::Value& input, StdExtra::optional<time_t>& output)
    {
        if (input == Json::Value::null)
        {
            output.setNull();
        }
        else
        {
            time_t outputVal = {};
            FromJsonUtilT(input, outputVal);
            output = outputVal;
        }
    }

    inline void ToJsonUtilT(const List<time_t>& input, Json::Value& output)
    {
        if (input.size() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            int index = 0;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                ToJsonUtilT(*iter, eachOutput);
                output[index++] = eachOutput;
            }
        }
    }

    inline void FromJsonUtilT(const Json::Value& input, List<time_t>& output)
    {
        output.clear();
        if (input == Json::Value::null || !input.isArray())
        {
            return;
        }

        time_t eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilT(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    inline void ToJsonUtilT(const Map<String, time_t>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            ToJsonUtilT(iter->second, eachOutput);
            output[iter->first] = eachOutput;
        }
    }

    inline void FromJsonUtilT(const Json::Value& input, Map<String, time_t>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        time_t eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilT(*iter, eachOutput);
            output[iter.key().asString()] = eachOutput;
        }
    }

    // Utilities for [de]serializing EnumType to/from json
    template <typename EnumType> inline void ToJsonUtilE(const StdExtra::optional<EnumType>& input, Json::Value& output)
    {
        if (input.isNull())
        {
            output = Json::Value();
        }
        else
        {
            ToJsonEnum(input, output);
        }
    }

    template <typename EnumType> inline void FromJsonUtilE(const Json::Value& input, StdExtra::optional<EnumType>& output)
    {
        if (input == Json::Value::null)
        {
            output.setNull();
        }
        else
        {
            EnumType outputVal;
            FromJsonEnum(input, outputVal);
            output = outputVal;
        }
    }

    template <typename EnumType> inline void ToJsonUtilE(const List<EnumType>& input, Json::Value& output)
    {
        if (input.size() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            int index = 0;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                ToJsonEnum(*iter, eachOutput);
                output[index++] = eachOutput;
            }
        }
    }

    template <typename EnumType> inline void FromJsonUtilE(const Json::Value& input, List<EnumType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        EnumType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonEnum(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    template <typename EnumType> inline void ToJsonUtilE(const Map<String, EnumType>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            ToJsonEnum(iter->second, eachOutput);
            output[iter->first] = eachOutput;
        }
    }

    template <typename EnumType> inline void FromJsonUtilE(const Json::Value& input, Map<String, EnumType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        EnumType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonEnum(iter, eachOutput);
            output[iter.key()] = eachOutput;
        }
    }

    // Utilities for [de]serializing string to/from json
    inline void ToJsonUtilS(const String& input, Json::Value& output)
    {
        if (input.length() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(input);
        }
    }

    inline void FromJsonUtilS(const Json::Value& input, String& output)
    {
        if (input == Json::Value::null)
        {
            output.clear();
        }
        else
        {
            output = input.asString();
        }
    }

    inline void ToJsonUtilS(const List<String>& input, Json::Value& output)
    {
        if (input.size() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            int index = 0;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                ToJsonUtilS(*iter, eachOutput);
                output[index++] = eachOutput;
            }
        }
    }

    inline void FromJsonUtilS(const Json::Value& input, List<String>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        String eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilS(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    inline void ToJsonUtilS(const Map<String, String>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            ToJsonUtilS(iter->second, eachOutput);
            output[iter->first] = eachOutput;
        }
    }

    inline void FromJsonUtilS(const Json::Value& input, Map<String, String>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        String eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilS(*iter, eachOutput);
            output[iter.key().asString()] = eachOutput;
        }
    }

    // Utilities for [de]serializing PlayFabObjects to/from json
    inline void ToJsonUtilO(const PlayFabBaseModel& input, Json::Value& output)
    {
        output = input.ToJson();
    }

    inline void FromJsonUtilO(const Json::Value& input, PlayFabBaseModel& output)
    {
        output.FromJson(input);
    }

    template <typename ObjectType> inline void ToJsonUtilO(const StdExtra::optional<ObjectType> input, Json::Value& output)
    {
        if (input.isNull())
        {
            output = Json::Value();
        }
        else
        {
            output = static_cast<ObjectType>(input).ToJson();
        }
    }

    template <typename ObjectType> inline void FromJsonUtilO(const Json::Value& input, StdExtra::optional<ObjectType>& output)
    {
        if (input == Json::Value::null)
        {
            output.setNull();
        }
        else
        {
            ObjectType outputTemp;
            outputTemp.FromJson(input);
            output = outputTemp;
        }
    }

    template <typename ObjectType> inline void ToJsonUtilO(const List<ObjectType>& input, Json::Value& output)
    {
        if (input.size() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            int index = 0;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                ToJsonUtilO(*iter, eachOutput);
                output[index++] = eachOutput;
            }
        }
    }

    template <typename ObjectType> inline void FromJsonUtilO(const Json::Value& input, List<ObjectType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        ObjectType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilO(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    template <typename ObjectType> inline void ToJsonUtilO(const Map<String, ObjectType>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            ToJsonUtilO(iter->second, eachOutput);
            output[iter->first] = eachOutput;
        }
    }

    template <typename ObjectType> inline void FromJsonUtilO(const Json::Value& input, Map<String, ObjectType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        ObjectType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilO(*iter, eachOutput);
            output[iter.key().asString()] = eachOutput;
        }
    }

    // Utilities for [de]serializing primitives to/from json
    template <typename PrimitiveType> inline void ToJsonUtilP(const PrimitiveType& input, Json::Value& output)
    {
        output = Json::Value(input);
    }

    inline void FromJsonUtilP(const Json::Value& input, bool& output)
    {
        output = input == Json::Value::null ? false : input.asBool();
    }

    inline void FromJsonUtilP(const Json::Value& input, Int16& output)
    {
        output = input == Json::Value::null ? 0 : static_cast<Int16>(input.asInt());
    }

    inline void FromJsonUtilP(const Json::Value& input, Uint16& output)
    {
        output = input == Json::Value::null ? static_cast<Uint16>(0) : static_cast<Uint16>(input.asInt());
    }

    inline void FromJsonUtilP(const Json::Value& input, Int32& output)
    {
        output = input == Json::Value::null ? 0 : input.asInt();
    }

    inline void FromJsonUtilP(const Json::Value& input, Uint32& output)
    {
        output = input == Json::Value::null ? 0 : input.asUInt();
    }

    inline void FromJsonUtilP(const Json::Value& input, Int64& output)
    {
        output = input == Json::Value::null ? 0 : input.asInt64();
    }

    inline void FromJsonUtilP(const Json::Value& input, Uint64& output)
    {
        output = input == Json::Value::null ? 0 : input.asUInt64();
    }

    inline void FromJsonUtilP(const Json::Value& input, float& output)
    {
        output = input == Json::Value::null ? 0 : input.asFloat();
    }

    inline void FromJsonUtilP(const Json::Value& input, double& output)
    {
        output = input == Json::Value::null ? 0 : input.asDouble();
    }

    template <typename PrimitiveType> inline void ToJsonUtilP(const StdExtra::optional<PrimitiveType>& input, Json::Value& output)
    {
        if (input.isNull())
        {
            output = Json::Value();
        }
        else
        {
            ToJsonUtilP(static_cast<PrimitiveType>(input), output);
        }
    }

    template <typename PrimitiveType> inline void FromJsonUtilP(const Json::Value& input, StdExtra::optional<PrimitiveType>& output)
    {
        if (input == Json::Value::null)
        {
            output.setNull();
        }
        else
        {
            PrimitiveType outputVal;
            FromJsonUtilP(input, outputVal);
            output = outputVal;
        }
    }

    template <typename PrimitiveType> inline void ToJsonUtilP(const List<PrimitiveType>& input, Json::Value& output)
    {
        if (input.size() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            int index = 0;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                ToJsonUtilP(*iter, eachOutput);
                output[index++] = eachOutput;
            }
        }
    }

    template <typename PrimitiveType> inline void FromJsonUtilP(const Json::Value& input, List<PrimitiveType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        PrimitiveType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilP(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    template <typename PrimitiveType> inline void ToJsonUtilP(const Map<String, PrimitiveType>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            ToJsonUtilP(iter->second, eachOutput);
            output[iter->first] = eachOutput;
        }
    }

    template <typename PrimitiveType> inline void FromJsonUtilP(const Json::Value& input, Map<String, PrimitiveType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        PrimitiveType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilP(*iter, eachOutput);
            output[iter.key().asString()] = eachOutput;
        }
    }
}

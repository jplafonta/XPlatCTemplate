#pragma once

#include <playfab/InternalMemory.h>
#include <playfab/StdOptional.h>
#include <EnumTraits.h>

namespace PlayFab
{
    struct PlayFabBaseModel;

    namespace JsonUtils
    {
        // Allocator to be used by rapidjson for all needed allocations
        extern JsonAllocator allocator;

        //------------------------------------------------------------------------------
        // Helpers for serializing to JsonValue
        //------------------------------------------------------------------------------
        JsonValue ToJson(const String& string);

        template <typename FundamentalType>
        JsonValue ToJson(FundamentalType value, typename std::enable_if_t<std::is_fundamental_v<FundamentalType>>* = 0);

        template <typename ModelType>
        JsonValue ToJson(const ModelType& model, typename std::enable_if_t<std::is_base_of_v<PlayFabBaseModel, ModelType>>* = 0);

        template <typename EnumType>
        JsonValue ToJson(EnumType value, typename std::enable_if_t<std::is_enum_v<EnumType>>* = 0);

        // Special case needed for time_t since it is a uint64_t (fundamental type) but gets to be converted to an Iso8601 formatted string JsonValue
        JsonValue ToJsonTime(time_t time);

        //------------------------------------------------------------------------------
        // Helpers for deserializing from JsonValue
        //------------------------------------------------------------------------------
        void FromJson(const JsonValue& input, String& output);

        void FromJson(const JsonValue& input, bool& output);

        void FromJson(const JsonValue& input, Int16& output);

        void FromJson(const JsonValue& input, Uint16& output);

        void FromJson(const JsonValue& input, Int32& output);

        void FromJson(const JsonValue& input, Uint32& output);

        void FromJson(const JsonValue& input, Int64& output);

        void FromJson(const JsonValue& input, Uint64& output);

        void FromJson(const JsonValue& input, float& output);

        void FromJson(const JsonValue& input, double& output);

        template<typename ModelType>
        void FromJson(const JsonValue& input, ModelType& output, typename std::enable_if_t<std::is_base_of_v<PlayFabBaseModel, ModelType>>* = 0);

        template <typename EnumType>
        void FromJson(const JsonValue& jsonValue, EnumType& output, typename std::enable_if_t<std::is_enum_v<EnumType>>* = 0);

        // Deep copies input to output
        void FromJson(const JsonValue& input, JsonValue& output);

        // Special case needed for time_t since it is an Iso8601 formatted JsonString that gets converted to a time_t
        void FromJsonTime(const JsonValue& jsonTime, time_t& output);

        //------------------------------------------------------------------------------
        // Helpers for adding members JsonObjects. 'value' param will be converted to a JsonValue added as a member to 'jsonObject'.
        // A global JsonAllocator will be used for any rapidjson allocations that are needed.
        //------------------------------------------------------------------------------

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, JsonValue&& value);

        void ObjectAddMember(JsonValue& jsonObject, JsonValue&& name, JsonValue&& value);

        // Value will be deep copied and added as member
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const JsonValue& value);

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const T& value);

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue&& name, const T& value);

        // If the optional value is not set, a 'null' JsonValue will be added to the object
        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const StdExtra::optional<T>& value);

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const List<T>& value);

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const Map<String, T>& value);

        void ObjectAddMemberTime(JsonValue& jsonObject, JsonValue::StringRefType name, time_t value);

        void ObjectAddMemberTime(JsonValue& jsonObject, JsonValue::StringRefType name, const StdExtra::optional<time_t>& value);

        void ObjectAddMemberTime(JsonValue& jsonObject, JsonValue::StringRefType name, const List<time_t>& value);

        void ObjectAddMemberTime(JsonValue& jsonObject, JsonValue::StringRefType name, const Map<String, time_t>& value);

        //------------------------------------------------------------------------------
        // Helpers for getting fields from JsonObjects as Cpp types.
        // Unless the output field is optional, these methods will assert if the JsonObject is missing the requested
        // field or if the requested fields are not the expected Json type.
        //------------------------------------------------------------------------------

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, T& output);

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, StdExtra::optional<T>& output);

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, List<T>& output);

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, Map<String, T>& output);

        // Optional string fields are mapped to String rather than optional<String> in client models,
        // so this method allows the field to be missing and leaves the output string unmodified.
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, String& output);

        // Optional JsonValue fields are mapped to JsonValue rather than optional<JsonValue> in client models,
        // so this method allows the field to be missing and leaves output value unmodified.
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, JsonValue& output);

        void ObjectGetMemberTime(const JsonValue& jsonObject, const char* name, time_t& output);

        void ObjectGetMemberTime(const JsonValue& jsonObject, const char* name, StdExtra::optional<time_t>& output);

        void ObjectGetMemberTime(const JsonValue& jsonObject, const char* name, List<time_t>& output);

        void ObjectGetMemberTime(const JsonValue& jsonObject, const char* name, Map<String, time_t>& output);

        //------------------------------------------------------------------------------
        // Template implementations
        //------------------------------------------------------------------------------

        template <typename FundamentalType>
        JsonValue ToJson(FundamentalType value, typename std::enable_if_t<std::is_fundamental_v<FundamentalType>>*)
        {
            return JsonValue{ value };
        }

        template <typename ModelType>
        JsonValue ToJson(const ModelType& model, typename std::enable_if_t<std::is_base_of_v<PlayFabBaseModel, ModelType>>*)
        {
            return model.ToJson();
        }

        template <typename EnumType>
        inline JsonValue ToJson(EnumType value, typename std::enable_if_t<std::is_enum_v<EnumType>>*)
        {
            return JsonValue{ EnumName<EnumType>(value), allocator };
        }

        template<typename ModelType>
        void FromJson(const JsonValue& input, ModelType& output, typename std::enable_if_t<std::is_base_of_v<PlayFabBaseModel, ModelType>>*)
        {
            output.FromJson(input);
        }

        template <typename EnumType>
        void FromJson(const JsonValue& jsonValue, EnumType& output, typename std::enable_if_t<std::is_enum_v<EnumType>>*)
        {
            output = EnumValue<EnumType>(jsonValue.GetString());
        }

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const T& value)
        {
            ObjectAddMember(jsonObject, name, ToJson(value));
        }

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue&& name, const T& value)
        {
            ObjectAddMember(jsonObject, std::move(name), ToJson(value));
        }

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const StdExtra::optional<T>& value)
        {
            if (value)
            {
                ObjectAddMember(jsonObject, name, *value);
            }
            else
            {
                // TODO is it necessary to add a null member for optional fields
                jsonObject.AddMember(name, JsonValue{ rapidjson::kNullType }, allocator);
            }
        }

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const List<T>& list)
        {
            JsonValue member{ rapidjson::kArrayType };
            for (auto& value : list)
            {
                member.PushBack(ToJson(value), allocator);
            }
            ObjectAddMember(jsonObject, name, member);
        }

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const Map<String, T>& map)
        {
            JsonValue member{ rapidjson::kObjectType };
            for (auto& entry : map)
            {
                ObjectAddMember(member, ToJson(entry.first), ToJson(entry.second));
            }
            ObjectAddMember(jsonObject, name, member);
        }

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* memberName, T& output)
        {
            FromJson(jsonObject[memberName], output);
        }

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* memberName, StdExtra::optional<T>& output)
        {
            output.reset();
            if (jsonObject.IsObject())
            {
                auto iter = jsonObject.FindMember(memberName);
                if (iter != jsonObject.MemberEnd())
                {
                    output.emplace();
                    FromJson(iter->value, *output);
                }
            }
        }

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* memberName, List<T>& output)
        {
            output.clear();
            if (jsonObject.IsObject())
            {
                auto iter = jsonObject.FindMember(memberName);
                if (iter != jsonObject.MemberEnd() && iter->value.IsArray())
                {
                    for (auto& value : iter->value.GetArray())
                    {
                        output.emplace_back();
                        FromJson(value, output.back());
                    }
                }
            }
        }

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* memberName, Map<String, T>& output)
        {
            output.clear();
            if (jsonObject.IsObject())
            {
                auto iter = jsonObject.FindMember(memberName);
                if (iter != jsonObject.MemberEnd() && iter->value.IsObject())
                {
                    for (auto& pair : iter->value.GetObject())
                    {
                        auto outputIter = output.emplace(pair.name.GetString(), T{}).first;
                        FromJson(pair.value, outputIter->second);
                    }
                }
            }
        }

    } // namespace JsonUtils
} // namespace PlayFab
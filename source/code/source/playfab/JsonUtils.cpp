#include <stdafx.h>
#include <JsonUtils.h>
#include <playfab/PlayFabPlatformUtils.h>
#include <EnumTraits.h>

namespace PlayFab
{
    namespace JsonUtils
    {
        JsonAllocator allocator{};

        JsonValue ToJson(const String& string)
        {
            if (string.empty())
            {
                return JsonValue{ rapidjson::kNullType };
            }
            return JsonValue{ string.data(), allocator };
        }

        JsonValue ToJsonTime(time_t time)
        {
            return JsonValue{ TimeTToIso8601String(time).data(), allocator };
        }

        void FromJson(const JsonValue& input, String& output)
        {
            output = input.GetString();
        }

        void FromJson(const JsonValue& input, bool& output)
        {
            output = input.GetBool();
        }

        void FromJson(const JsonValue& input, Int16& output)
        {
            output = static_cast<Int16>(input.GetInt());
        }

        void FromJson(const JsonValue& input, Uint16& output)
        {
            output = static_cast<Uint16>(input.GetUint());
        }

        void FromJson(const JsonValue& input, Int32& output)
        {
            output = static_cast<Int32>(input.GetInt());
        }

        void FromJson(const JsonValue& input, Uint32& output)
        {
            output = static_cast<Uint32>(input.GetUint());
        }

        void FromJson(const JsonValue& input, Int64& output)
        {
            output = input.GetInt64();
        }

        void FromJson(const JsonValue& input, Uint64& output)
        {
            output = input.GetUint64();
        }

        void FromJson(const JsonValue& input, float& output)
        {
            output = input.GetFloat();
        }

        void FromJson(const JsonValue& input, double& output)
        {
            output = input.GetDouble();
        }

        void FromJson(const JsonValue& input, JsonValue& output)
        {
            output.CopyFrom(input, allocator); // Deep copy
        }

        void FromJsonTime(const JsonValue& jsonTime, time_t& output)
        {
            output = Iso8601StringToTimeT(jsonTime.GetString());
        }

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, JsonValue&& value)
        {
            jsonObject.AddMember(name, value, allocator);
        }

        void ObjectAddMember(JsonValue& jsonObject, JsonValue&& name, JsonValue&& value)
        {
            jsonObject.AddMember(name, value, allocator);
        }

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const JsonValue& value)
        {
            jsonObject.AddMember(name, JsonValue{}.CopyFrom(value, allocator), allocator);
        }

        void ObjectAddMemberTime(JsonValue& jsonObject, JsonValue::StringRefType name, time_t time)
        {
            ObjectAddMember(jsonObject, name, ToJsonTime(time));
        }

        void ObjectAddMemberTime(JsonValue& jsonObject, JsonValue::StringRefType name, const StdExtra::optional<time_t>& time)
        {
            if (time)
            {
                ObjectAddMemberTime(jsonObject, name, *time);
            }
            else
            {
                // TODO is it necessary to add a null member for optional fields
                jsonObject.AddMember(name, JsonValue{ rapidjson::kNullType }, allocator);
            }
        }

        void ObjectAddMemberTime(JsonValue& jsonObject, JsonValue::StringRefType name, const List<time_t>& timeList)
        {
            JsonValue member{ rapidjson::kArrayType };
            for (auto& time : timeList)
            {
                member.PushBack(ToJsonTime(time), allocator);
            }
            ObjectAddMember(jsonObject, name, member);
        }

        void ObjectAddMemberTime(JsonValue& jsonObject, JsonValue::StringRefType name, const Map<String, time_t>& timeMap)
        {
            JsonValue member{ rapidjson::kObjectType };
            for (auto& entry : timeMap)
            {
                ObjectAddMember(member, ToJson(entry.first), ToJsonTime(entry.second));
            }
            ObjectAddMember(jsonObject, name, member);
        }

        void ObjectGetMember(const JsonValue& jsonObject, const char* memberName, String& output)
        {
            auto iter = jsonObject.FindMember(memberName);
            if (iter != jsonObject.MemberEnd() && iter->value.IsString())
            {
                FromJson(iter->value, output);
            }
        }

        void ObjectGetMember(const JsonValue& jsonObject, const char* memberName, JsonValue& output)
        {
            auto iter = jsonObject.FindMember(memberName);
            if (iter != jsonObject.MemberEnd())
            {
                FromJson(iter->value, output);
            }
        }

        void ObjectGetMemberTime(const JsonValue& jsonObject, const char* memberName, time_t& output)
        {
            FromJsonTime(jsonObject[memberName], output);
        }

        void ObjectGetMemberTime(const JsonValue& jsonObject, const char* memberName, StdExtra::optional<time_t>& output)
        {
            output.reset();
            if (jsonObject.IsObject())
            {
                auto iter = jsonObject.FindMember(memberName);
                if (iter != jsonObject.MemberEnd() && iter->value.IsString())
                {
                    output.emplace();
                    FromJsonTime(iter->value, *output);
                }
            }
        }

        void ObjectGetMemberTime(const JsonValue& jsonObject, const char* memberName, List<time_t>& output)
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
                        FromJsonTime(value, output.back());
                    }
                }
            }
        }

        void ObjectGetMemberTime(const JsonValue& jsonObject, const char* memberName, Map<String, time_t>& output)
        {
            output.clear();
            if (jsonObject.IsObject())
            {
                auto iter = jsonObject.FindMember(memberName);
                if (iter != jsonObject.MemberEnd() && iter->value.IsObject())
                {
                    for (auto& pair : iter->value.GetObject())
                    {
                        auto outputIter = output.emplace(pair.name.GetString(), time_t{}).first;
                        FromJson(pair.value, outputIter->second);
                    }
                }
            }
        }
    }
}
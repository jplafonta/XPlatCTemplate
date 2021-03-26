#include <stdafx.h>
#include <JsonUtils.h>
#include <playfab/PlayFabPlatformUtils.h>
#include <EnumTraits.h>

namespace PlayFab
{
    namespace JsonUtils
    {
        JsonAllocator allocator{};

        JsonValue ToJson(const char* string)
        {
            if (!string)
            {
                return JsonValue{ rapidjson::kNullType };
            }
            return JsonValue{ string, allocator };
        }

        JsonValue ToJson(const PlayFabJsonObject& jsonObject)
        {
            // TODO Seems like there is an extra copy here. Is there is a better way to do this?
            JsonDocument document{ &allocator };
            document.Parse(jsonObject.stringValue);
            JsonValue value;
            value.CopyFrom(document, allocator);
            return value;
        }

        JsonValue ToJson(time_t value, bool convertToIso8601String)
        {
            if (convertToIso8601String)
            {
                return JsonValue{ TimeTToIso8601String(value).data(), allocator };
            }
            return JsonValue{ value };
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

        void FromJson(const JsonValue& input, time_t& output, bool convertFromIso8601String)
        {
            if (convertFromIso8601String)
            {
                output = Iso8601StringToTimeT(input.GetString());
            }
            else
            {
                output = input.GetInt64();
            }
        }

        void FromJson(const JsonValue& input, JsonValue& output)
        {
            output.CopyFrom(input, allocator); // Deep copy
        }

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, JsonValue&& value)
        {
            jsonObject.AddMember(name, value, allocator);
        }

        void ObjectAddMember(JsonValue& jsonObject, JsonValue&& name, JsonValue&& value)
        {
            jsonObject.AddMember(name, value, allocator);
        }

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, time_t value, bool convertToIso8601String)
        {
            ObjectAddMember(jsonObject, name, ToJson(value, convertToIso8601String));
        }

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const time_t* value, bool convertToIso8601String)
        {
            if (value != nullptr)
            {
                ObjectAddMember(jsonObject, name, ToJson(*value, convertToIso8601String));
            }
            else
            {
                ObjectAddMember(jsonObject, name, JsonValue{ rapidjson::kNullType });
            }
        }

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const time_t* array, uint32_t arrayCount, bool convertToIso8601String)
        {
            JsonValue member{ rapidjson::kArrayType };
            for (auto i = 0u; i < arrayCount; ++i)
            {
                member.PushBack(ToJson(array[i], convertToIso8601String), allocator);
            }
            ObjectAddMember(jsonObject, name, std::move(member));
        }

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const Vector<time_t>& vector, bool convertToIso8601String)
        {
            ObjectAddMember(jsonObject, name, vector.data(), static_cast<uint32_t>(vector.size()), convertToIso8601String);
        }

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const PlayFabDateTimeDictionaryEntry* associativeArray, uint32_t arrayCount, bool convertToIso8601String)
        {
            JsonValue member{ rapidjson::kObjectType };
            for (auto i = 0u; i < arrayCount; ++i)
            {
                auto& entry{ associativeArray[i] };
                ObjectAddMember(member, ToJson(entry.key), ToJson(entry.value, convertToIso8601String));
            }
            ObjectAddMember(jsonObject, name, std::move(member));
        }

        //void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const JsonValue& value)
        //{
        //    jsonObject.AddMember(name, JsonValue{}.CopyFrom(value, allocator), allocator);
        //}

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, String& output)
        {
            auto iter = jsonObject.FindMember(name);
            if (iter != jsonObject.MemberEnd() && iter->value.IsString())
            {
                FromJson(iter->value, output);
            }
        }

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, String& output, const char*& outputPtr)
        {
            output.clear();
            auto iter = jsonObject.FindMember(name);
            if (iter != jsonObject.MemberEnd() && iter->value.IsString())
            {
                FromJson(iter->value, output);
            }
            outputPtr = output.empty() ? nullptr : output.data();
        }

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, JsonObject& output, PlayFabJsonObject& outputPtr)
        {
            auto iter = jsonObject.FindMember(name);
            if (iter != jsonObject.MemberEnd())
            {
                FromJson(iter->value, output);
            }
            outputPtr.stringValue = output.StringValue();
        }

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, JsonValue& output)
        {
            auto iter = jsonObject.FindMember(name);
            if (iter != jsonObject.MemberEnd())
            {
                FromJson(iter->value, output);
            }
        }

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, time_t& output, bool convertFromIso8601String)
        {
            auto iter = jsonObject.FindMember(name);
            if (iter != jsonObject.MemberEnd())
            {
                FromJson(iter->value, output, convertFromIso8601String);
            }
        }

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, StdExtra::optional<time_t>& output, bool convertFromIso8601String)
        {
            output.reset();
            if (jsonObject.IsObject())
            {
                auto iter = jsonObject.FindMember(name);
                if (iter != jsonObject.MemberEnd())
                {
                    output.emplace();
                    FromJson(iter->value, *output, convertFromIso8601String);
                }
            }
        }

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, StdExtra::optional<time_t>& output, time_t*& outputPtr, bool convertFromIso8601String)
        {
            ObjectGetMember(jsonObject, name, output, convertFromIso8601String);
            outputPtr = output ? output.operator->() : nullptr;
        }

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, Vector<time_t>& output, time_t*& outputPtr, uint32_t& outputCount, bool convertFromIso8601String)
        {
            output.clear();
            if (jsonObject.IsObject())
            {
                auto iter = jsonObject.FindMember(name);
                if (iter != jsonObject.MemberEnd() && iter->value.IsArray())
                {
                    auto jsonArray{ iter->value.GetArray() };
                    output.reserve(jsonArray.Size());
                    for (auto& value : jsonArray)
                    {
                        output.emplace_back();
                        FromJson(value, output.back(), convertFromIso8601String);
                    }
                }
            }

            outputPtr = output.data();
            outputCount = static_cast<uint32_t>(output.size());
        }
    }
}
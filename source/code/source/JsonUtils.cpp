#include "stdafx.h"
#include "JsonUtils.h"
#include "BaseModel.h"
#include <rapidjson/stream.h>

namespace PlayFab
{
namespace JsonUtils
{
JsonAllocator allocator{};

// Helper class for writing JsonValue directly into a PlayFab::String. Implements rapidjson write-only stream
// concept (see rapidjson/stream.h for details). Avoids additional copy needed when first writing to rapidjons::StringBuffer
class StringOutputStream
{
public:
    using Ch = String::traits_type::char_type;

    StringOutputStream(size_t initialCapacity = kDefaultCapacity)
    {
        m_string.reserve(initialCapacity);
    }

    void Put(Ch c)
    {
        m_string.push_back(c);
    }

    void Flush()
    {
        // no-op
    }

    const String& GetString() const
    {
        return m_string;
    }

    String&& ExtractString()
    {
        return std::move(m_string);
    }

    static const size_t kDefaultCapacity = 256; // use same default capacity as rapidjson::GenericStringBuffer

private:
    String m_string;
};

String WriteToString(const JsonValue& jsonValue)
{
    StringOutputStream stream;
    JsonWriter<StringOutputStream> writer{ stream, &allocator };
    jsonValue.Accept(writer);
    return stream.ExtractString();
}

JsonValue ToJson(const char* string)
{
    if (!string)
    {
        return JsonValue{ rapidjson::kNullType };
    }
    return JsonValue{ string, allocator };
}

JsonValue ToJson(const String& string)
{
    // By design, map empty string to null JsonValue
    if (string.empty())
    {
        return JsonValue{ rapidjson::kNullType };
    }
    return JsonValue{ string.data(), allocator };
}

JsonValue ToJson(const PlayFabJsonObject& jsonObject)
{
    JsonDocument document{ &allocator };
    document.Parse(jsonObject.stringValue);
    return JsonValue{ document, allocator };
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

void FromJson(const JsonValue& input, int16_t& output)
{
    output = static_cast<int16_t>(input.GetInt());
}

void FromJson(const JsonValue& input, uint16_t& output)
{
    output = static_cast<uint16_t>(input.GetUint());
}

void FromJson(const JsonValue& input, int32_t& output)
{
    output = static_cast<int32_t>(input.GetInt());
}

void FromJson(const JsonValue& input, uint32_t& output)
{
    output = static_cast<uint32_t>(input.GetUint());
}

void FromJson(const JsonValue& input, uint64_t& output)
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

void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const StdExtra::optional<time_t>& value, bool convertToIso8601String)
{
    if (value.has_value())
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

void ObjectGetMember(const JsonValue& jsonObject, const char* name, StdExtra::optional<time_t>& output, time_t const*& outputPtr, bool convertFromIso8601String)
{
    ObjectGetMember(jsonObject, name, output, convertFromIso8601String);
    outputPtr = output ? output.operator->() : nullptr;
}

void ObjectGetMember(const JsonValue& jsonObject, const char* name, Vector<time_t>& output, time_t const*& outputPtr, uint32_t& outputCount, bool convertFromIso8601String)
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

} // namespace JsonUtils
} // namespace PlayFab

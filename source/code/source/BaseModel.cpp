#include "stdafx.h"
#include "BaseModel.h"

namespace PlayFab
{

JsonObject::JsonObject(const JsonObject& src) :
    m_string{ src.m_string }
{
    JsonUtils::FromJson(src.m_value, m_value);
}

const char* JsonObject::StringValue() const
{
    return m_string.empty() ? nullptr : m_string.data();
}

void JsonObject::FromJson(const PlayFab::JsonValue& input)
{
    // TODO might be away to avoid copying from string buffer into PlayFab::String
    JsonUtils::FromJson(input, m_value);
    JsonStringBuffer stringBuffer{ &JsonUtils::allocator };
    JsonWriter writer{ stringBuffer };
    m_value.Accept(writer);
    m_string = stringBuffer.GetString();
}

JsonValue JsonObject::ToJson() const
{
    JsonValue output;
    JsonUtils::FromJson(m_value, output);
    return output;
}

}

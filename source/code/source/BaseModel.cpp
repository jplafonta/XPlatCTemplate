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
    JsonUtils::FromJson(input, m_value);
    m_string = JsonUtils::WriteToString(m_value);
}

JsonValue JsonObject::ToJson() const
{
    JsonValue output;
    JsonUtils::FromJson(m_value, output);
    return output;
}

}

#include <stdafx.h>
#include <playfab/PlayFabBaseModel.h>
#include <JsonUtils.h>

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

    inline PointerArray<const char, String>::PointerArray(const PointerArray& src) :
        m_objects(src.m_objects)
    {
        m_pointers.reserve(m_objects.size());
        for (const auto& o : m_objects)
        {
            m_pointers.push_back(o.data());
        }
    }

    inline void PointerArray<const char, String>::FromJson(const JsonValue& input)
    {
        Clear();
        if (input.IsArray())
        {
            auto jsonArray{ input.GetArray() };

            // Reserve vector storage to ensure no reallocation happens while inserting
            m_objects.reserve(jsonArray.Size());
            m_pointers.reserve(jsonArray.Size());

            for (auto& value : jsonArray)
            {
                m_objects.emplace_back();
                JsonUtils::FromJson(value, m_objects.back());
                m_pointers.emplace_back(m_objects.back().data());
            }
        }
    }

    template <>
    inline void AssociativeArray<PlayFabDateTimeDictionaryEntry, void>::FromJson(const JsonValue& input)
    {
        Clear();
        if (input.IsObject())
        {
            m_entries.reserve(input.MemberCount());

            for (auto& pair : input.GetObject())
            {
                auto keysIter = m_keys.emplace(pair.name.GetString()).first;
                m_entries.emplace_back(PlayFabDateTimeDictionaryEntry{ keysIter->data(), time_t{} });
                JsonUtils::FromJson(pair.value, m_entries.back().value, true);
            }
        }
    }
}
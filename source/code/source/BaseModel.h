#pragma once

#include "JsonUtils.h"

namespace PlayFab
{

/// <summary>
/// Base class for any object returned from a public API via a PlayFabResult
/// </summary>
struct ApiResult
{
    virtual ~ApiResult() = default;
};

}

struct PlayFabResult
{
    PlayFabResult(PlayFab::SharedPtr<PlayFab::ApiResult> result_) : result{ result_ } {}
    PlayFabResult(const PlayFabResult&) = default;
    ~PlayFabResult() = default;

    PlayFab::SharedPtr<PlayFab::ApiResult> result;
};

namespace PlayFab
{

/// <summary>
/// Base class for all PlayFab Models
/// Provides interface for converting service Models to/from json
/// </summary>
class BaseModel
{
public:
    virtual ~BaseModel() = default;
    virtual void FromJson(const JsonValue& input) = 0;
    virtual JsonValue ToJson() const = 0;
};

/// <summary>
/// A model that can be (trivially) serialized into a continuous memory buffer.
/// <summary>
class SerializableModel : public BaseModel
{
public:
    virtual size_t SerializedSize() const = 0;
    virtual void Serialize(void* buffer, size_t bufferSize) const = 0;
};

/// <summary>
/// JsonObject with cached string representation
class JsonObject : public BaseModel
{
public:
    JsonObject() = default;
    JsonObject(const JsonObject& src);
    JsonObject(JsonObject&& src) = default;
    JsonObject& operator=(const JsonObject& src) = delete;
    ~JsonObject() = default;

    const char* StringValue() const;

    void FromJson(const PlayFab::JsonValue& input) override;
    PlayFab::JsonValue ToJson() const override;
private:
    PlayFab::JsonValue m_value;
    String m_string;
};

/// <summary>
/// Helper class for managing C-style arrays of pointers to internal objects.
/// By design, the only ways to modify contents after construction is the assignment operator (no way to insert/remove)
/// </summary>
template<typename PointerT, typename ObjectT> class PointerArray
{
public:
    using DataPtr = PointerT const* const*;
    using ConstDataPtr = DataPtr const;

    PointerArray() = default;
    PointerArray(Vector<ObjectT>&& objects);
    PointerArray(const PointerArray& src);
    PointerArray(PointerArray&& src) = default;
    PointerArray& operator=(const PointerArray& src) = delete;
    virtual ~PointerArray() = default;

    bool Empty() const;
    ConstDataPtr Data() const;
    DataPtr Data();
    size_t Size() const;
    void Clear();

protected:
    Vector<PointerT const*> m_pointers;
    Vector<ObjectT> m_objects;
};

/// <summary>
/// PointerArray that supports conversion to/from Json.
/// </summary>
template<typename PointerT, typename ObjectT> class PointerArrayModel : public PointerArray<PointerT, ObjectT>, public BaseModel
{
public:
    PointerArrayModel() = default;
    PointerArrayModel(const PointerArrayModel& src) = default;
    PointerArrayModel(PointerArrayModel&& src) = default;
    PointerArrayModel& operator=(const PointerArrayModel& src) = default;
    PointerArrayModel& operator=(PointerArrayModel&& src) = default;
    virtual ~PointerArrayModel() = default;

    void FromJson(const JsonValue& input) override;
    JsonValue ToJson() const override;
};

/// <summary>
/// Helper class for managing arrays of key value pairs. Key type must always be a string. Supports conversion to/from Json.
/// By design, the only ways to modify contents after construction are FromJson & assignment operator (no way to insert/remove).
/// </summary>
template <typename EntryT, typename ValueT>
class AssociativeArrayModel : public BaseModel
{
public:
    using DataPtr = EntryT const*;
    using ConstDataPtr = DataPtr const;

    AssociativeArrayModel() = default;
    AssociativeArrayModel(const AssociativeArrayModel& src);
    AssociativeArrayModel(AssociativeArrayModel&& src) = default;
    AssociativeArrayModel& operator=(AssociativeArrayModel src);
    AssociativeArrayModel& operator=(AssociativeArrayModel&& src) = default;
    virtual ~AssociativeArrayModel() = default;

    bool Empty() const;
    DataPtr Data();
    ConstDataPtr Data() const;
    size_t Size() const;
    void Clear();

    void FromJson(const JsonValue&) override;
    JsonValue ToJson() const override;

private:
    Vector<EntryT> m_entries;
    Map<String, ValueT> m_map;
};

// Specialization for EntryT where EntryT::value doesn't require storage outside the Entry struct
template<typename EntryT>
class AssociativeArrayModel<EntryT, void> : public BaseModel
{
public:
    using DataPtr = EntryT const*;
    using ConstDataPtr = DataPtr const;

    AssociativeArrayModel() = default;
    AssociativeArrayModel(const AssociativeArrayModel& src);
    AssociativeArrayModel(AssociativeArrayModel&& src) = default;
    AssociativeArrayModel& operator=(AssociativeArrayModel src);
    AssociativeArrayModel& operator=(AssociativeArrayModel&& src) = default;
    virtual ~AssociativeArrayModel() = default;

    bool Empty() const;
    DataPtr Data();
    ConstDataPtr Data() const;
    size_t Size() const;
    void Clear();

    void FromJson(const JsonValue&) override;
    JsonValue ToJson() const override;

private:
    Vector<EntryT> m_entries;
    Set<String> m_keys;
};

//------------------------------------------------------------------------------
// Template implementations
//------------------------------------------------------------------------------

template<typename PointerT, typename ObjectT>
PointerArray<PointerT, ObjectT>::PointerArray(const PointerArray& src) :
    m_objects(src.m_objects)
{
    m_pointers.reserve(m_objects.size());
    for (const auto& o : m_objects)
    {
        m_pointers.push_back((PointerT*)&o);
    }
}

template<typename PointerT, typename ObjectT>
PointerArray<PointerT, ObjectT>::PointerArray(Vector<ObjectT>&& objects) :
    m_objects(std::move(objects))
{
    m_pointers.reserve(m_objects.size());
    for (const auto& o : m_objects)
    {
        m_pointers.push_back((PointerT*)&o);
    }
}

template<typename PointerT, typename ObjectT>
bool PointerArray<PointerT, ObjectT>::Empty() const
{
    return m_pointers.empty();
}

template<typename PointerT, typename ObjectT>
typename PointerArray<PointerT, ObjectT>::ConstDataPtr PointerArray<PointerT, ObjectT>::Data() const
{
    return m_pointers.data();
}

template<typename PointerT, typename ObjectT>
typename PointerArray<PointerT, ObjectT>::DataPtr PointerArray<PointerT, ObjectT>::Data()
{
    return m_pointers.data();
}

template<typename PointerT, typename ObjectT>
size_t PointerArray<PointerT, ObjectT>::Size() const
{
    assert(m_pointers.size() == m_objects.size());
    return m_pointers.size();
}

template<typename PointerT, typename ObjectT>
void PointerArray<PointerT, ObjectT>::Clear()
{
    m_pointers.clear();
    m_objects.clear();
}

template<typename PointerT, typename ObjectT>
void PointerArrayModel<PointerT, ObjectT>::FromJson(const JsonValue& input)
{
    this->Clear();
    if (input.IsArray())
    {
        auto jsonArray{ input.GetArray() };

        // Reserve vector storage to ensure no reallocation happens while inserting
        this->m_objects.reserve(jsonArray.Size());
        this->m_pointers.reserve(jsonArray.Size());

        for (auto& value : jsonArray)
        {
            this->m_objects.emplace_back();
            JsonUtils::FromJson(value, this->m_objects.back());
            this->m_pointers.emplace_back((PointerT*)(&this->m_objects.back()));
        }
    }
}

template<typename PointerT, typename ObjectT>
JsonValue PointerArrayModel<PointerT, ObjectT>::ToJson() const
{
    JsonValue output{ rapidjson::kArrayType };
    for (const auto& pointer : this->m_pointers)
    {
        output.PushBack(JsonUtils::ToJson(pointer), JsonUtils::allocator);
    }
    return output;
}

template <typename EntryT, typename ValueT>
AssociativeArrayModel<EntryT, ValueT>::AssociativeArrayModel(const AssociativeArrayModel& src) :
    m_map{ src.m_map }
{
    m_entries.reserve(m_map.size());
    for (const auto& pair : m_map)
    {
        m_entries.push_back(EntryT{ pair.first.data(), (decltype(EntryT::value))&pair.second });
    }
}

template <typename EntryT, typename ValueT>
AssociativeArrayModel<EntryT, ValueT>& AssociativeArrayModel<EntryT, ValueT>::operator=(AssociativeArrayModel src)
{
    m_entries = std::move(src.m_entries);
    m_map = std::move(src.m_map);
    return *this;
}

template <typename EntryT, typename ValueT>
bool AssociativeArrayModel<EntryT, ValueT>::Empty() const
{
    return m_entries.empty();
}

template <typename EntryT, typename ValueT>
typename AssociativeArrayModel<EntryT, ValueT>::DataPtr AssociativeArrayModel<EntryT, ValueT>::Data()
{
    return m_entries.data();
}

template <typename EntryT, typename ValueT>
typename AssociativeArrayModel<EntryT, ValueT>::ConstDataPtr AssociativeArrayModel<EntryT, ValueT>::Data() const
{
    return m_entries.data();
}

template <typename EntryT, typename ValueT>
size_t AssociativeArrayModel<EntryT, ValueT>::Size() const
{
    return m_entries.size();
}

template <typename EntryT, typename ValueT>
void AssociativeArrayModel<EntryT, ValueT>::Clear()
{
    m_entries.clear();
    m_map.clear();
}

template <typename EntryT, typename ValueT>
void AssociativeArrayModel<EntryT, ValueT>::FromJson(const JsonValue& input)
{
    Clear();
    if (input.IsObject())
    {
        m_entries.reserve(input.MemberCount());

        for (auto& pair : input.GetObject())
        {
            auto mapIter = m_map.emplace(pair.name.GetString(), ValueT{}).first;
            JsonUtils::FromJson(pair.value, mapIter->second);
            m_entries.emplace_back(EntryT{ mapIter->first.data(), (decltype(EntryT::value))&mapIter->second });
        }
    }
}

template <typename EntryT, typename ValueT>
JsonValue AssociativeArrayModel<EntryT, ValueT>::ToJson() const
{
    JsonValue output{ rapidjson::kObjectType };
    for (auto& entry : m_entries)
    {
        JsonUtils::ObjectAddMember(output, JsonUtils::ToJson(entry.key), JsonUtils::ToJson(entry.value));
    }
    return output;
}

template <typename EntryT>
AssociativeArrayModel<EntryT, void>::AssociativeArrayModel(const AssociativeArrayModel& src) :
    m_entries{ src.m_entries },
    m_keys{ src.m_keys }
{
    for (auto& entry : m_entries)
    {
        entry.key = m_keys.find(entry.key)->data();
    }
}

template <typename EntryT>
AssociativeArrayModel<EntryT, void>& AssociativeArrayModel<EntryT, void>::operator=(AssociativeArrayModel src)
{
    m_entries = std::move(src.m_entries);
    m_keys = std::move(src.m_keys);
    return *this;
}

template <typename EntryT>
bool AssociativeArrayModel<EntryT, void>::Empty() const
{
    return m_entries.empty();
}

template <typename EntryT>
typename AssociativeArrayModel<EntryT, void>::DataPtr AssociativeArrayModel<EntryT, void>::Data()
{
    return m_entries.data();
}

template <typename EntryT>
typename AssociativeArrayModel<EntryT, void>::ConstDataPtr AssociativeArrayModel<EntryT, void>::Data() const
{
    return m_entries.data();
}

template <typename EntryT>
size_t AssociativeArrayModel<EntryT, void>::Size() const
{
    return m_entries.size();
}

template <typename EntryT>
void AssociativeArrayModel<EntryT, void>::Clear()
{
    m_entries.clear();
    m_keys.clear();
}

template <typename EntryT>
void AssociativeArrayModel<EntryT, void>::FromJson(const JsonValue& input)
{
    Clear();
    if (input.IsObject())
    {
        m_entries.reserve(input.MemberCount());

        for (auto& pair : input.GetObject())
        {
            auto keysIter = m_keys.emplace(pair.name.GetString()).first;
            m_entries.emplace_back(EntryT{ keysIter->data(), decltype(EntryT::value){} });
            JsonUtils::FromJson(pair.value, m_entries.back().value);
        }
    }
}

template <typename EntryT>
JsonValue AssociativeArrayModel<EntryT, void>::ToJson() const
{
    JsonValue output{ rapidjson::kObjectType };
    for (auto& entry : m_entries)
    {
        JsonUtils::ObjectAddMember(output, JsonUtils::ToJson(entry.key), JsonUtils::ToJson(entry.value));
    }
    return output;
}

inline PointerArray<char, String>::PointerArray(const PointerArray& src) :
    m_objects(src.m_objects)
{
    m_pointers.reserve(m_objects.size());
    for (const auto& o : m_objects)
    {
        m_pointers.push_back(o.data());
    }
}

template<>
inline void PointerArrayModel<char, String>::FromJson(const JsonValue& input)
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
inline AssociativeArrayModel<PlayFabStringDictionaryEntry, String>::AssociativeArrayModel(const AssociativeArrayModel& src) :
    m_map{ src.m_map }
{
    m_entries.reserve(m_map.size());
    for (const auto& pair : m_map)
    {
        m_entries.push_back(PlayFabStringDictionaryEntry{ pair.first.data(), pair.second.data() });
    }
}

template <>
inline void AssociativeArrayModel<PlayFabStringDictionaryEntry, String>::FromJson(const JsonValue& input)
{
    Clear();
    if (input.IsObject())
    {
        m_entries.reserve(input.MemberCount());

        for (auto& pair : input.GetObject())
        {
            auto mapIter = m_map.emplace(pair.name.GetString(), String{}).first;
            JsonUtils::FromJson(pair.value, mapIter->second);
            m_entries.emplace_back(PlayFabStringDictionaryEntry{ mapIter->first.data(), mapIter->second.data() });
        }
    }
}

template <>
inline void AssociativeArrayModel<PlayFabDateTimeDictionaryEntry, void>::FromJson(const JsonValue& input)
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

template <>
inline JsonValue AssociativeArrayModel<PlayFabDateTimeDictionaryEntry, void>::ToJson() const
{
    JsonValue output{ rapidjson::kObjectType };
    for (auto& entry : m_entries)
    {
        JsonUtils::ObjectAddMember(output, JsonUtils::ToJson(entry.key), JsonUtils::ToJson(entry.value, true));
    }
    return output;
}

}

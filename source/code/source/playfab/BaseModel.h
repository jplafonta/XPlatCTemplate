#pragma once

#include <playfab/PlayFabBaseModel_c.h>
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
    class BaseModel
    {
    public:
        virtual ~BaseModel() = default;
        virtual void FromJson(const JsonValue& input) = 0;
        virtual JsonValue ToJson() const = 0;
    };

    /// <summary>
    /// Base class for all PlayFab Requests
    /// </summary>
    class BaseRequest : public BaseModel
    {
    public:
        SharedPtr<PlayFabAuthenticationContext> authenticationContext;
    };

    /// <summary>
    /// Base class for all PlayFab Results
    /// </summary>
    class BaseResult : public BaseModel
    {
    public:
        JsonValue Request;

        BaseResult() = default;
        BaseResult(const BaseResult& src);
    };

    /// <summary>
    /// A result that can (trivially) be serialized into a continuous memory buffer
    /// <summary>
    class SerializableResult : BaseResult
    {
    public:
        virtual size_t RequiredBufferSize() const = 0;
        virtual void Serialize(void* buffer, size_t bufferSize) const = 0;
    };

    /// <summary>
    /// Base class for all PlayFab Login method Results
    /// </summary>
    class PlayFabLoginResultCommon : public BaseResult
    {
    public:
        /// <summary>
        /// An authentication context returned by Login methods, it can used in multi-user scenarios
        /// </summary>
        SharedPtr<PlayFabAuthenticationContext> authenticationContext;
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
    /// Helper class for managing C-style arrays of pointers to PlayFab models.
    /// By design, the only way to populate array is using FromJson (no insert/remove methods)
    /// </summary>
    template<typename PointerT, typename ObjectT> class PointerArray : public BaseModel
    {
    public:
        using DataPtr = PointerT**;
        using ConstDataPtr = const PointerT**;

        PointerArray() = default;
        PointerArray(const PointerArray& src);
        PointerArray(PointerArray&& src) = default;
        PointerArray& operator=(PointerArray src);
        PointerArray& operator=(PointerArray&& src) = default;
        virtual ~PointerArray() = default;

        bool Empty() const;
        ConstDataPtr Data() const;
        DataPtr Data();
        size_t Size() const;
        void Clear();

        void FromJson(const JsonValue& input) override;
        JsonValue ToJson() const override;

    private:
        Vector<PointerT*> m_pointers;
        Vector<ObjectT> m_objects;
    };

    /// <summary>
    /// Helper class for managing arrays of key value pairs. Key type must always be a string.
    /// Currently only have support for populating these arrays using PointerArray::FromJson (no other way to insert)
    /// </summary>
    template <typename EntryT, typename ValueT>
    class AssociativeArray : public BaseModel
    {
    public:
        using DataPtr = EntryT*;
        using ConstDataPtr = const EntryT*;

        AssociativeArray() = default;
        AssociativeArray(const AssociativeArray& src);
        AssociativeArray(AssociativeArray&& src) = default;
        AssociativeArray& operator=(AssociativeArray src);
        AssociativeArray& operator=(AssociativeArray&& src) = default;
        virtual ~AssociativeArray() = default;

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
    class AssociativeArray<EntryT, void> : public BaseModel
    {
    public:
        using DataPtr = EntryT*;
        using ConstDataPtr = const EntryT*;

        AssociativeArray() = default;
        AssociativeArray(const AssociativeArray& src);
        AssociativeArray(AssociativeArray&& src) = default;
        AssociativeArray& operator=(AssociativeArray src);
        AssociativeArray& operator=(AssociativeArray&& src) = default;
        virtual ~AssociativeArray() = default;

        bool Empty() const;
        EntryT* Data();
        const EntryT* Data() const;
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
    PointerArray<PointerT, ObjectT>& PointerArray<PointerT, ObjectT>::operator=(PointerArray src)
    {
        m_pointers = std::move(src.m_pointers);
        m_objects = std::move(src.m_objects);
        return *this;
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
    void PointerArray<PointerT, ObjectT>::FromJson(const JsonValue& input)
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
                m_pointers.emplace_back((PointerT*)(&m_objects.back()));
            }
        }
    }

    template<typename PointerT, typename ObjectT>
    JsonValue PointerArray<PointerT, ObjectT>::ToJson() const
    {
        JsonValue output{ rapidjson::kArrayType };
        for (const auto& pointer : m_pointers)
        {
            output.PushBack(JsonUtils::ToJson(pointer), JsonUtils::allocator);
        }
        return output;
    }

    template <typename EntryT, typename ValueT>
    AssociativeArray<EntryT, ValueT>::AssociativeArray(const AssociativeArray& src) :
        m_map{ src.m_map }
    {
        m_entries.reserve(m_map.size());
        for (const auto& pair : m_map)
        {
            m_entries.push_back(EntryT{ pair.first.data(), (decltype(EntryT::value))&pair.second });
        }
    }

    template <typename EntryT, typename ValueT>
    AssociativeArray<EntryT, ValueT>& AssociativeArray<EntryT, ValueT>::operator=(AssociativeArray src)
    {
        m_entries = std::move(src.m_entries);
        m_map = std::move(src.m_map);
        return *this;
    }

    template <typename EntryT, typename ValueT>
    bool AssociativeArray<EntryT, ValueT>::Empty() const
    {
        return m_entries.empty();
    }

    template <typename EntryT, typename ValueT>
    typename AssociativeArray<EntryT, ValueT>::DataPtr AssociativeArray<EntryT, ValueT>::Data()
    {
        return m_entries.data();
    }

    template <typename EntryT, typename ValueT>
    typename AssociativeArray<EntryT, ValueT>::ConstDataPtr AssociativeArray<EntryT, ValueT>::Data() const
    {
        return m_entries.data();
    }

    template <typename EntryT, typename ValueT>
    size_t AssociativeArray<EntryT, ValueT>::Size() const
    {
        return m_entries.size();
    }

    template <typename EntryT, typename ValueT>
    void AssociativeArray<EntryT, ValueT>::Clear()
    {
        m_entries.clear();
        m_map.clear();
    }

    template <typename EntryT, typename ValueT>
    void AssociativeArray<EntryT, ValueT>::FromJson(const JsonValue& input)
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
    JsonValue AssociativeArray<EntryT, ValueT>::ToJson() const
    {
        JsonValue output{ rapidjson::kObjectType };
        for (auto& entry : m_entries)
        {
            JsonUtils::ObjectAddMember(output, JsonUtils::ToJson(entry.key), JsonUtils::ToJson(entry.value));
        }
        return output;
    }

    template <typename EntryT>
    AssociativeArray<EntryT, void>::AssociativeArray(const AssociativeArray& src) :
        m_entries{ src.m_entries },
        m_keys{ src.m_keys }
    {
        for (auto& entry : m_entries)
        {
            entry.key = m_keys.find(entry.key)->data();
        }
    }

    template <typename EntryT>
    AssociativeArray<EntryT, void>& AssociativeArray<EntryT, void>::operator=(AssociativeArray src)
    {
        m_entries = std::move(src.m_entries);
        m_keys = std::move(src.m_keys);
        return *this;
    }

    template <typename EntryT>
    bool AssociativeArray<EntryT, void>::Empty() const
    {
        return m_entries.empty();
    }

    template <typename EntryT>
    typename AssociativeArray<EntryT, void>::DataPtr AssociativeArray<EntryT, void>::Data()
    {
        return m_entries.data();
    }

    template <typename EntryT>
    typename AssociativeArray<EntryT, void>::ConstDataPtr AssociativeArray<EntryT, void>::Data() const
    {
        return m_entries.data();
    }

    template <typename EntryT>
    size_t AssociativeArray<EntryT, void>::Size() const
    {
        return m_entries.size();
    }

    template <typename EntryT>
    void AssociativeArray<EntryT, void>::Clear()
    {
        m_entries.clear();
        m_keys.clear();
    }

    template <typename EntryT>
    void AssociativeArray<EntryT, void>::FromJson(const JsonValue& input)
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
    JsonValue AssociativeArray<EntryT, void>::ToJson() const
    {
        JsonValue output{ rapidjson::kObjectType };
        for (auto& entry : m_entries)
        {
            JsonUtils::ObjectAddMember(output, JsonUtils::ToJson(entry.key), JsonUtils::ToJson(entry.value));
        }
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

    template<>
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

    template <>
    inline JsonValue AssociativeArray<PlayFabDateTimeDictionaryEntry, void>::ToJson() const
    {
        JsonValue output{ rapidjson::kObjectType };
        for (auto& entry : m_entries)
        {
            JsonUtils::ObjectAddMember(output, JsonUtils::ToJson(entry.key), JsonUtils::ToJson(entry.value, true));
        }
        return output;
    }
}

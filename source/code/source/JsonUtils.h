#pragma once

#include <playfab/PlayFabBaseModel.h>
#include "EnumTraits.h"

namespace PlayFab
{
    // forward declarations
    class BaseModel;
    class JsonObject;
    template<typename PointerT, typename ObjectT> class PointerArray;
    template <typename EntryT, typename ValueT> class AssociativeArray;

    namespace JsonUtils
    {
        namespace Detail
        {
            // There isn't a base class for dictionary entry types, so consider any type with a const char* "key" member and a "value" member
            // a dictionary entry
            template <typename T, typename = void>
            struct IsDictionaryEntry : std::false_type {};

            template <typename T>
            struct IsDictionaryEntry<T, std::enable_if_t<std::is_same_v<decltype(T::key), const char*> && std::is_same_v<decltype(T::value), decltype(T::value)>>> : std::true_type {};
        }

        // Allocator to be used by rapidjson for all needed allocations
        extern JsonAllocator allocator;

        //------------------------------------------------------------------------------
        // Helpers for serializing to JsonValue
        //------------------------------------------------------------------------------

        JsonValue ToJson(const char* string);

        JsonValue ToJson(const PlayFabJsonObject& jsonObject);

        JsonValue ToJson(time_t value, bool convertToIso8601String = false);

        // This template method is specialized for each model type. Disabled for types with specializations below
        template <typename T, typename std::enable_if_t<!std::is_enum_v<T> && !std::is_fundamental_v<T> && !std::is_pointer_v<T> && !std::is_base_of_v<BaseModel, T>>* = 0>
        JsonValue ToJson(const T& value);

        // Specialization for fundamental types
        template <typename FundamentalT, typename std::enable_if_t<std::is_fundamental_v<FundamentalT>>* = 0>
        JsonValue ToJson(FundamentalT value);

        // Specialization for enum types
        template <typename EnumT, typename std::enable_if_t<std::is_enum_v<EnumT>>* = 0>
        JsonValue ToJson(EnumT value);

        // Specialization for Model types
        template <typename ModelT, typename std::enable_if_t<std::is_base_of_v<BaseModel, ModelT>>* = 0>
        JsonValue ToJson(const ModelT& value);

        // Specialization for pointers. Returns JsonValue{ kNullType } if pointer is null and appropriately converts to JsonValue otherwise
        template <typename PtrT, typename std::enable_if_t<std::is_pointer_v<PtrT>>* = 0>
        JsonValue ToJson(const PtrT value);

        //------------------------------------------------------------------------------
        // Helpers for deserializing from JsonValue
        //------------------------------------------------------------------------------
        void FromJson(const JsonValue& input, String& output);

        void FromJson(const JsonValue& input, bool& output);

        void FromJson(const JsonValue& input, int16_t& output);

        void FromJson(const JsonValue& input, uint16_t& output);

        void FromJson(const JsonValue& input, int32_t& output);

        void FromJson(const JsonValue& input, uint32_t& output);

        void FromJson(const JsonValue& input, uint64_t& output);

        void FromJson(const JsonValue& input, float& output);

        void FromJson(const JsonValue& input, double& output);

        // Special case needed for time_t since it is an Iso8601 formatted JsonString that gets converted to a time_t
        void FromJson(const JsonValue& input, time_t& output, bool convertFromIso8601String = false);

        template<typename ModelType, typename std::enable_if_t<std::is_base_of_v<BaseModel, ModelType>>* = 0>
        void FromJson(const JsonValue& input, ModelType& output);

        template <typename EnumT, typename std::enable_if_t<std::is_enum_v<EnumT>>* = 0>
        void FromJson(const JsonValue& jsonValue, EnumT& output);

        // Deep copies input to output
        void FromJson(const JsonValue& input, JsonValue& output);

        //------------------------------------------------------------------------------
        // Helpers for adding members JsonObjects. 'value' param will be converted to a JsonValue added as a member to 'jsonObject'.
        // A global JsonAllocator will be used for any rapidjson allocations that are needed.
        //------------------------------------------------------------------------------

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, JsonValue&& value);

        void ObjectAddMember(JsonValue& jsonObject, JsonValue&& name, JsonValue&& value);

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const T& value);

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, time_t value, bool convertToIso8601String = false);

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const time_t* value, bool convertToIso8601String = false);

        template <typename T, typename std::enable_if_t<!Detail::IsDictionaryEntry<T>::value>* = 0>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const T* array, uint32_t arrayCount);

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const time_t* array, uint32_t arrayCount, bool convertToIso8601String = false);

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const Vector<T>& vector);

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const Vector<time_t>& vector, bool convertToIso8601String = false);

        template <typename EntryT, typename std::enable_if_t<Detail::IsDictionaryEntry<EntryT>::value>* = 0>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const EntryT* associativeArray, uint32_t arrayCount);

        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const PlayFabDateTimeDictionaryEntry* associativeArray, uint32_t arrayCount, bool convertToIso8601String = false);

        //------------------------------------------------------------------------------
        // Helpers for getting fields from JsonObjects as Cpp types.
        // Unless the output field is optional, rapidjson will assert if the JsonObject is missing the requested
        // field or if the requested fields are not the expected Json type.
        //
        // Some of the methods also have output parameters where a pointer to the retreived value & (in the case of collections) the count is returned.
        //------------------------------------------------------------------------------

        template <typename T, typename std::enable_if_t<!std::is_same_v<T, time_t>>* = 0>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, T& output);

        template <typename T, typename PointerT>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, T& output, PointerT*& outputPtr);

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, JsonObject& output, PlayFabJsonObject& outputPtr);

        // Optional string fields are mapped to String rather than optional<String> in client models,
        // so this method allows the field to be missing and leaves the output string unmodified.
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, String& output);

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, String& output, const char*& outputPtr);

        // Optional JsonValue fields are mapped to JsonValue rather than optional<JsonValue> in client models,
        // so this method allows the field to be missing and leaves output value unmodified.
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, JsonValue& output);

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, time_t& output, bool convertFromIso8601String = false);

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, StdExtra::optional<T>& output);

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, StdExtra::optional<time_t>& output, bool convertFromIso8601String = false);

        template <typename T, typename PointerT>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, StdExtra::optional<T>& output, PointerT*& outputPtr);

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, StdExtra::optional<time_t>& output, time_t*& outputPtr, bool convertFromIso8601String = false);

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, Vector<T>& output, T*& outputPtr, uint32_t& outputCount);

        void ObjectGetMember(const JsonValue& jsonObject, const char* name, Vector<time_t>& output, time_t*& outputPtr, uint32_t& outputCount, bool convertFromIso8601String = false);

        template <typename ObjectT, typename PointerT>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, PointerArray<PointerT, ObjectT>& output, PointerT**& outputPtr, uint32_t& outputCount);

        template <typename EntryT, typename ValueT>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, AssociativeArray<EntryT, ValueT>& output, EntryT*& outputPtr, uint32_t& outputCount);

        //------------------------------------------------------------------------------
        // Template implementations
        //------------------------------------------------------------------------------

        template <typename T, typename std::enable_if_t<!std::is_enum_v<T> && !std::is_fundamental_v<T> && !std::is_pointer_v<T>>*>
        JsonValue ToJson(const T& value)
        {
            static_assert(false, "Specialization missing for " __FUNCSIG__);
        }

        template <typename FundamentalT, typename std::enable_if_t<std::is_fundamental_v<FundamentalT>>*>
        JsonValue ToJson(FundamentalT value)
        {
            return JsonValue{ value };
        }

        template <typename EnumT, typename std::enable_if_t<std::is_enum_v<EnumT>>*>
        inline JsonValue ToJson(EnumT value)
        {
            return JsonValue{ EnumName<EnumT>(value), allocator };
        }

        template <typename ModelT, typename std::enable_if_t<std::is_base_of_v<BaseModel, ModelT>>*>
        JsonValue ToJson(const ModelT& value)
        {
            return value.ToJson();
        }

        template <typename PtrT, typename std::enable_if_t<std::is_pointer_v<PtrT>>*>
        JsonValue ToJson(const PtrT value)
        {
            if (value != nullptr)
            {
                return ToJson(*value);
            }
            return JsonValue{ rapidjson::kNullType };
        }

        template<typename ModelType, typename std::enable_if_t<std::is_base_of_v<BaseModel, ModelType>>*>
        void FromJson(const JsonValue& input, ModelType& output)
        {
            output.FromJson(input);
        }

        template <typename EnumT, typename std::enable_if_t<std::is_enum_v<EnumT>>*>
        void FromJson(const JsonValue& jsonValue, EnumT& output)
        {
            output = EnumValue<EnumT>(jsonValue.GetString());
        }

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const T& value)
        {
            ObjectAddMember(jsonObject, name, ToJson(value));
        }

        template <typename T, typename std::enable_if_t<!Detail::IsDictionaryEntry<T>::value>*>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const T* array, uint32_t arrayCount)
        {
            JsonValue member{ rapidjson::kArrayType };
            for (auto i = 0u; i < arrayCount; ++i)
            {
                member.PushBack(ToJson(array[i]), allocator);
            }
            ObjectAddMember(jsonObject, name, std::move(member));
        }

        template <typename T>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const Vector<T>& vector)
        {
            ObjectAddMember(jsonObject, name, vector.data(), static_cast<uint32_t>(vector.size()));
        }

        template <typename EntryT, typename std::enable_if_t<Detail::IsDictionaryEntry<EntryT>::value>*>
        void ObjectAddMember(JsonValue& jsonObject, JsonValue::StringRefType name, const EntryT* associativeArray, uint32_t arrayCount)
        {
            JsonValue member{ rapidjson::kObjectType };
            for (auto i = 0u; i < arrayCount; ++i)
            {
                auto& entry{ associativeArray[i] };
                ObjectAddMember(member, ToJson(entry.key), ToJson(entry.value));
            }
            ObjectAddMember(jsonObject, name, std::move(member));
        }

        template <typename T, typename std::enable_if_t<!std::is_same_v<T, time_t>>*>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, T& output)
        {
            FromJson(jsonObject[name], output);
        }

        template <typename T, typename PointerT>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, T& output, PointerT*& outputPtr)
        {
            static_assert(std::is_convertible_v<T*, PointerT*>, "T must be a PointerT");

            FromJson(jsonObject[name], output);
            outputPtr = (PointerT*)&output;
        }

        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, StdExtra::optional<T>& output)
        {
            output.reset();
            if (jsonObject.IsObject())
            {
                auto iter = jsonObject.FindMember(name);
                if (iter != jsonObject.MemberEnd())
                {
                    output.emplace();
                    FromJson(iter->value, *output);
                }
            }
        }

        template <typename T, typename PointerT>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, StdExtra::optional<T>& output, PointerT*& outputPtr)
        {
            static_assert(std::is_convertible_v<T*, PointerT*>, "T must be a PointerT");

            ObjectGetMember(jsonObject, name, output);
            outputPtr = output ? output.operator->() : nullptr;
        }



        template <typename T>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, Vector<T>& output, T*& outputPtr, uint32_t& outputCount)
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
                        FromJson(value, output.back());
                    }
                }
            }

            outputPtr = output.data();
            outputCount = static_cast<uint32_t>(output.size());
        }

        template <typename T, typename PointerT>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, PointerArray<PointerT, T>& output, PointerT**& outputPtr, uint32_t& outputCount)
        {
            output.Clear();
            if (jsonObject.IsObject())
            {
                auto iter = jsonObject.FindMember(name);
                if (iter != jsonObject.MemberEnd())
                {
                    output.FromJson(iter->value);
                }
            }

            outputPtr = output.Data();
            outputCount = static_cast<uint32_t>(output.Size());
        }

        template <typename EntryT, typename ValueT>
        void ObjectGetMember(const JsonValue& jsonObject, const char* name, AssociativeArray<EntryT, ValueT>& output, EntryT*& outputPtr, uint32_t& outputCount)
        {
            output.Clear();
            if (jsonObject.IsObject())
            {
                auto iter = jsonObject.FindMember(name);
                if (iter != jsonObject.MemberEnd())
                {
                    output.FromJson(iter->value);
                }
            }

            outputPtr = output.Data();
            outputCount = static_cast<uint32_t>(output.Size());
        }

    } // namespace JsonUtils
} // namespace PlayFab
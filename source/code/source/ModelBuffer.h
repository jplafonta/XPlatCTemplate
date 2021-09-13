#pragma once

#include <playfab/cpp/PFModelWrapperHelpers.h>

namespace PlayFab
{

class ModelBuffer
{
public:
    ModelBuffer(void* buffer, size_t bufferSize);
    ModelBuffer(const ModelBuffer&) = delete;
    ModelBuffer(ModelBuffer&&) = default;
    ~ModelBuffer() = default;

    template<typename T>
    T* Alloc(size_t n);

    template<typename T, typename std::enable_if_t<std::is_fundamental_v<T> || std::is_enum_v<T>>* = 0>
    T* CopyTo(const T* input);

    template<typename T>
    typename T::ModelType* CopyTo(typename T::ModelType const* input);

    const char* CopyTo(const char* input);

    template<typename T, typename std::enable_if_t<std::is_fundamental_v<T> || std::is_enum_v<T>>* = 0>
    T* CopyToArray(const T* input, size_t inputCount);

    template<typename T>
    typename T::ModelType const** CopyToArray(typename T::ModelType const* const* input, size_t inputCount);

    const char** CopyToArray(const char* const* input, size_t inputCount);

    template<typename T, typename std::enable_if_t<Detail::IsDictionaryEntry<T>::value && (std::is_fundamental_v<decltype(T::value)> || std::is_enum_v<decltype(T::value)>)>* = 0>
    T* CopyToDictionary(const T* input, size_t inputCount);

    template<typename InternalModelWrapperT>
    typename InternalModelWrapperT::DictionaryEntryType* CopyToDictionary(const typename InternalModelWrapperT::DictionaryEntryType* input, size_t inputCount);

    PFStringDictionaryEntry* CopyToDictionary(PFStringDictionaryEntry const* input, size_t inputCount);

private:
#if _DEBUG
    char* const m_buffer;
    size_t const m_bufferSize;
#endif
    void* m_bufferPtr;
    size_t m_remaining;
};

//------------------------------------------------------------------------------
// Template implementations
//------------------------------------------------------------------------------

template<typename T>
T* ModelBuffer::Alloc(size_t n)
{
    size_t const size = sizeof(T) * n;
    if (std::align(alignof(T), size, m_bufferPtr, m_remaining))
    {
        T* result = reinterpret_cast<T*>(m_bufferPtr);
        m_bufferPtr = static_cast<char*>(m_bufferPtr) + size;
        m_remaining -= size;
        return result;
    }
    assert(false);
    return nullptr;
}

template<typename T, typename std::enable_if_t<std::is_fundamental_v<T> || std::is_enum_v<T>>*>
T* ModelBuffer::CopyTo(const T* input)
{
    if (input)
    {
        auto output = Alloc<T>(1);
        assert(output);
        *output = *input;
        return output;
    }
    else
    {
        return nullptr;
    }
}

template<typename T>
typename T::ModelType* ModelBuffer::CopyTo(const typename T::ModelType* input)
{
    if (input)
    {
        auto output = Alloc<typename T::ModelType>(1);
        assert(output);
        T::Copy(*input, *output, *this);
        return output;
    }
    else
    {
        return nullptr;
    }
}

template<typename T, typename std::enable_if_t<std::is_fundamental_v<T> || std::is_enum_v<T>>*>
T* ModelBuffer::CopyToArray(const T* input, size_t inputCount)
{
    if (input && inputCount)
    {
        auto output = Alloc<T>(inputCount);
        assert(output);
        std::memcpy(output, input, sizeof(T) * inputCount);
        return output;
    }
    else
    {
        return nullptr;
    }
}

template<typename T>
typename T::ModelType const** ModelBuffer::CopyToArray(typename T::ModelType const* const* input, size_t inputCount)
{
    if (input && inputCount)
    {
        auto output = Alloc<typename T::ModelType const*>(inputCount);
        assert(output);
        for (size_t i = 0; i < inputCount; ++i)
        {
            output[i] = CopyTo<T>(input[i]);
        }
        return output;
    }
    else
    {
        return nullptr;
    }
}

template<typename T, typename std::enable_if_t<Detail::IsDictionaryEntry<T>::value && (std::is_fundamental_v<decltype(T::value)> || std::is_enum_v<decltype(T::value)>)>*>
T* ModelBuffer::CopyToDictionary(const T* input, size_t inputCount)
{
    if (input && inputCount)
    {
        auto output = Alloc<T>(inputCount);
        assert(output);
        for (size_t i = 0; i < inputCount; ++i)
        {
            output[i].key = CopyTo(input[i].key);
            output[i].value = input[i].value;
        }
        return output;
    }
    else
    {
        return nullptr;
    }
}

template<typename InternalModelWrapperT>
typename InternalModelWrapperT::DictionaryEntryType* ModelBuffer::CopyToDictionary(const typename InternalModelWrapperT::DictionaryEntryType* input, size_t inputCount)
{
    if (input && inputCount)
    {
        auto output = Alloc<typename InternalModelWrapperT::DictionaryEntryType>(inputCount);
        assert(output);
        for (size_t i = 0; i < inputCount; ++i)
        {
            output[i].key = CopyTo(input[i].key);
            output[i].value = CopyTo<InternalModelWrapperT>(input[i].value);
        }
        return output;
    }
    else
    {
        return nullptr;
    }
}

}

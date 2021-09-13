#include "stdafx.h"
#include "ModelBuffer.h"

namespace PlayFab
{

ModelBuffer::ModelBuffer(void* buffer, size_t bufferSize) :
#if _DEBUG
    m_buffer{ static_cast<char*>(buffer) },
    m_bufferSize{ bufferSize },
#endif
    m_bufferPtr{ buffer },
    m_remaining{ bufferSize }
{
}

const char* ModelBuffer::CopyTo(const char* input)
{
    if (input)
    {
        size_t bytesNeeded = std::strlen(input) + 1; // null terminator
        auto stringCopy = Alloc<char>(bytesNeeded);
        return std::strcpy(stringCopy, input);
    }
    else
    {
        return nullptr;
    }
}

const char** ModelBuffer::CopyToArray(const char* const* input, size_t inputCount)
{
    if (input && inputCount)
    {
        auto output = Alloc<const char*>(inputCount);
        assert(output);
        for (size_t i = 0; i < inputCount; ++i)
        {
            output[i] = CopyTo(input[i]);
        }
        return output;
    }
    else
    {
        return nullptr;
    }
}

PFStringDictionaryEntry* ModelBuffer::CopyToDictionary(PFStringDictionaryEntry const* input, size_t inputCount)
{
    if (input && inputCount)
    {
        auto output = Alloc<PFStringDictionaryEntry>(inputCount);
        assert(output);
        for (size_t i = 0; i < inputCount; ++i)
        {
            output[i].key = CopyTo(input[i].key);
            output[i].value = CopyTo(input[i].value);
        }
        return output;
    }
    else
    {
        return nullptr;
    }
}

}
#include "stdafx.h"

namespace PlayFab
{
namespace Detail
{

// Default memory functions
_Ret_maybenull_ _Post_writable_byte_size_(size) void* DefaultAlloc(size_t size, uint32_t /* tag */) noexcept
{
    return operator new(size);
}

void DefaultFree(_In_ _Post_invalid_ void* pointer, uint32_t /* tag */) noexcept
{
    operator delete(pointer);
}

MemoryHooks& GetMemoryHooks()
{
    // The memory hooks are a function level static so we have guarantees about initialization order. If we make them a
    // global static, the initialization of other globals that require allocation may cause a crash
    static MemoryHooks s_hooks{ DefaultAlloc, DefaultFree };
    return s_hooks;
}

HRESULT SetMemoryHooks(PlayFabMemAllocFunction* memAllocFunc, PlayFabMemFreeFunction* memFreeFunc)
{
    // Memhooks can't be null
    RETURN_HR_INVALIDARG_IF_NULL(memAllocFunc);
    RETURN_HR_INVALIDARG_IF_NULL(memFreeFunc);

    auto& hooks = GetMemoryHooks();

    // Only allow hooks to be set once
    if (hooks.alloc != DefaultAlloc || hooks.free != DefaultFree)
    {
        TRACE_ERROR("Memory Hooks can only be set once!");
        return E_FAIL;
    }
    else
    {
        hooks.alloc = memAllocFunc;
        hooks.free = memFreeFunc;
    }

    return S_OK;
}

}

constexpr uint32_t playFabMemoryTag = 3302021;

void* Alloc(size_t size)
{
    void* pointer = Detail::GetMemoryHooks().alloc(size, playFabMemoryTag);

    assert(pointer);
    return pointer;
}

void Free(void* pointer) noexcept
{
    if (pointer)
    {
        Detail::GetMemoryHooks().free(pointer, playFabMemoryTag);
    }
}

void* JsonAllocator::Malloc(size_t size)
{
    return PlayFab::Alloc(size);
}

void* JsonAllocator::Realloc(void* originalPtr, size_t originalSize, size_t newSize)
{
    void* newPtr = nullptr;
    if (newSize > 0)
    {
        newPtr = PlayFab::Alloc(newSize);
        memcpy(newPtr, originalPtr, (originalSize < newSize ? originalSize : newSize));
    }
    Free(originalPtr);
    return newPtr;
}

void JsonAllocator::Free(void* ptr)
{
    PlayFab::Free(ptr);
}

}
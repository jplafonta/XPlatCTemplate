#pragma once

#include <memory>
#include <unordered_map>
#include <set>
#include <queue>
#include <unordered_set>
#include <map>
#include <playfab/PlayFabJsonHeaders.h>

namespace PlayFab
{

//------------------------------------------------------------------------------
// Allocator
//------------------------------------------------------------------------------
template<class T>
struct Allocator
{
    using value_type = T;
    using is_always_equal = std::true_type;

    Allocator() = default;
    template<class U>
    Allocator(Allocator<U> const&) {}

    T* allocate(size_t n);
    void deallocate(_In_opt_ void* p, size_t) noexcept;
};

template<class T>
bool operator==(Allocator<T> const&, Allocator<T> const&);
template<class T>
bool operator!=(Allocator<T> const&, Allocator<T> const&);

//------------------------------------------------------------------------------
// Deleter
//------------------------------------------------------------------------------
template<class T>
struct Deleter
{
    void operator()(T* ptr) const noexcept;
};

//------------------------------------------------------------------------------
// rapidjson Allocator
//------------------------------------------------------------------------------
struct JsonAllocator
{
public:
    static const bool kNeedFree = true;

    void* Malloc(size_t size)
    {
        return malloc(size);
    }
    void* Realloc(void* originalPtr, size_t originalSize, size_t newSize)
    {
        void* newPtr = nullptr;
        if (newSize > 0)
        {
            newPtr = Malloc(newSize);
            memcpy(newPtr, originalPtr, (originalSize < newSize ? originalSize : newSize));
        }
        Free(originalPtr);
        return newPtr;
    }
    static void Free(void* ptr)
    {
        free(ptr);
    }
};

//------------------------------------------------------------------------------
// Smart Pointers
//------------------------------------------------------------------------------
template<class T>
using SharedPtr = std::shared_ptr<T>;

template<class T>
using WeakPtr = std::weak_ptr<T>;

template<class T, class TDeleter = Deleter<T>>
using UniquePtr = std::unique_ptr<T, TDeleter>;

//------------------------------------------------------------------------------
// Factories
//------------------------------------------------------------------------------
template<class T, class... TArgs>
SharedPtr<T> MakeShared(TArgs&&... args);

template<class T, class... TArgs>
UniquePtr<T> MakeUnique(TArgs&&... args);

//------------------------------------------------------------------------------
// Template implementations
//------------------------------------------------------------------------------
template<class T>
T* Allocator<T>::allocate(size_t n)
{
    if (n > (static_cast<size_t>(-1) / sizeof(T)))
    {
        throw std::bad_alloc{};
    }

    auto rawPtr = malloc(n * sizeof(T)); // Bug 15586224: how to ensure alignment?
    return static_cast<T*>(rawPtr);
}

template<class T>
void Allocator<T>::deallocate(_In_opt_ void* p, size_t) noexcept
{
    free(p);
}

template<class T>
bool operator==(Allocator<T> const&, Allocator<T> const&)
{
    return true;
}

template<class T>
bool operator!=(Allocator<T> const&, Allocator<T> const&)
{
    return false;
}

template<class T>
void Deleter<T>::operator()(T* ptr) const noexcept
{
    ptr->~T(); // destroy
    Allocator<T> alloc{};
    alloc.deallocate((void*)ptr, 1);
}

template<class T, class... TArgs>
SharedPtr<T> MakeShared(TArgs&&... args)
{
    return std::allocate_shared<T>(Allocator<T>{}, std::forward<TArgs>(args)...);
}

template<class T, class... TArgs>
UniquePtr<T> MakeUnique(TArgs&&... args)
{
    // allocate
    Allocator<T> alloc{};
    auto mem = alloc.allocate(1);

    // construct (failure here only requires deallocation)
    auto obj = new (mem) T(std::forward<TArgs>(args)...);

    // destruction is required from here on
    // pass ownership to correctly typed smart pointer
    UniquePtr<T> ptr{ obj };

    // success!
    return ptr;
}

template<class T>
using Vector = std::vector<T, Allocator<T>>;

template<class K, class V, class LESS = std::less<K>>
using Map = std::map<K, V, LESS, Allocator<std::pair<K const, V>>>;

template<class K, class LESS = std::less<K>>
using Set = std::set<K, LESS, Allocator<K>>;

template<class K, class V, class HASH = std::hash<K>, class EQUAL = std::equal_to<K>>
using UnorderedMap = std::unordered_map<K, V, HASH, EQUAL, Allocator<std::pair<K const, V>>>;

template<class K, class HASH = std::hash<K>, class EQUAL = std::equal_to<K>>
using UnorderedSet = std::unordered_set<K, HASH, EQUAL, Allocator<K>>;

template<class C, class TRAITS = std::char_traits<C>>
using BasicString = std::basic_string<C, TRAITS, Allocator<C>>;
using String = BasicString<char>;
using WString = BasicString<wchar_t>;

template<class C, class TRAITS = std::char_traits<C>>
using BasicStringsteam = std::basic_stringstream<C, TRAITS, Allocator<C>>;
using Stringstream = BasicStringsteam<char>;
using WStringstream = BasicStringsteam<wchar_t>;

template<class T>
using Deque = std::deque<T, Allocator<T>>;

template<class T>
using Queue = std::queue<T, Deque<T>>;

template<class T>
using List = std::list<T, Allocator<T>>;

using JsonDocument = rapidjson::GenericDocument<rapidjson::UTF8<>, JsonAllocator>;
using JsonValue = rapidjson::GenericValue<rapidjson::UTF8<>, JsonAllocator>;
using JsonStringBuffer = rapidjson::GenericStringBuffer<rapidjson::UTF8<>, JsonAllocator>;
using JsonWriter = rapidjson::Writer<JsonStringBuffer>;

}

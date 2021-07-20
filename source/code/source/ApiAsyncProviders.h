#pragma once

#include "AsyncProvider.h"
#include "Entity.h"
#include "AsyncOp.h"

namespace PlayFab
{

// XAsync Provider for PlayFab API calls
template<typename CallT>
class ApiProvider : public Provider
{
public:
    template<size_t n>
    ApiProvider(XAsyncBlock* async, const char(&identityName)[n], CallT call) :
        Provider{ async, identityName },
        m_call{ call }
    {
    }

protected:
    using ResultT = typename Detail::UnwrapAsyncT<typename std::result_of_t<CallT(const TaskQueue&)>>;

    // Always kick of the API call during XAsyncOp::Begin so we don't have to worry about lifetime of request
    // and API objects (even though they are hidden as part of a std::bind)
    HRESULT Begin(TaskQueue&& queue) override
    {
        m_call(queue).Finally([this](Result<ResultT> result)
        {
            if (Succeeded(result))
            {
                TRACE_VERBOSE("ApiProvider[ID=%s] Call suceeded (hr=0x%08x)", identityName, result.hr);
                this->Complete(this->StoreResult(std::move(result)));
            }
            else
            {
                TRACE_ERROR("ApiProvider[ID=%s] Call failed with message \"%s\" (hr=0x%08x)", identityName, result.errorMessage.data(), result.hr);
                this->Fail(result.hr);
            }
        });

        return S_OK;
    }

    HRESULT GetResult(void* buffer, size_t bufferSize) override
    {
        GetResultHelper(buffer, bufferSize);
        return S_OK;
    }

    // How we deliver the XAsync result will depend on ResultT of m_call:
    // 1. If there is no result payload, there is no result to store & GetResult Op should never be called.
    // 2. If the result payload is serializable, store the result, report the needed result buffer size, and serialize it into
    //    the clients buffer during GetResult.
    // 3. If the result payload isn't serializable, store the result, report the needed buffer size as sizeof(PlayFabResultHandle),
    //    and during GetResult, create a PlayFabResult object return a pointer to that.

    template<typename T = ResultT>
    size_t StoreResult(Result<void>&&, typename std::enable_if_t<std::is_void_v<T>>* = 0)
    {
        // No result to store in this case
        return 0;
    }

    template<typename T = ResultT>
    size_t StoreResult(Result<ResultT>&& result, typename std::enable_if_t<!std::is_void_v<T> && std::is_base_of_v<SerializableModel, T>>* = 0)
    {
        this->m_result.emplace(std::move(result));
        return this->m_result->Payload().SerializedSize();
    }

    template<typename T = ResultT>
    size_t StoreResult(Result<ResultT>&& result, typename std::enable_if_t<!std::is_void_v<T> && !std::is_base_of_v<SerializableModel, T>>* = 0)
    {
        this->m_result.emplace(std::move(result));
        return sizeof(PlayFabResultHandle);
    }

    template<typename T = ResultT>
    void GetResultHelper(void*, size_t, typename std::enable_if_t<std::is_void_v<T>>* = 0)
    {
        // Should never be called
        assert(false);
    }

    template<typename T = ResultT>
    void GetResultHelper(void* buffer, size_t bufferSize, typename std::enable_if_t<!std::is_void_v<T> && std::is_base_of_v<SerializableModel, T>>* = 0)
    {
        assert(m_result.has_value());
        m_result->Payload().Serialize(buffer, bufferSize);
    }

    template<typename T = ResultT>
    void GetResultHelper(void* buffer, size_t /*bufferSize*/, typename std::enable_if_t<!std::is_void_v<T> && !std::is_base_of_v<SerializableModel, T>>* = 0)
    {
        assert(m_result.has_value());
        auto resultPtr = static_cast<PlayFabResult**>(buffer);
        *resultPtr = MakeUnique<PlayFabResult>(MakeShared<ResultT>(m_result->ExtractPayload())).release();
    }

    CallT m_call;
    // Storing Result<ResultT> rather than ResultT because Result<void> is valid so we don't need a specialization for that
    StdExtra::optional<Result<ResultT>> m_result;

    using Provider::identityName;
};

template<typename CallT, size_t n>
UniquePtr<ApiProvider<CallT>> MakeProvider(XAsyncBlock* async, const char(&identityName)[n], CallT call)
{
    return MakeUnique<ApiProvider<CallT>>(async, identityName, std::move(call));
}

template<typename CallT>
class EntityApiProvider : public ApiProvider<CallT>
{
public:
    template<size_t n>
    EntityApiProvider(XAsyncBlock* async, const char(&identityName)[n], SharedPtr<Entity> entity, CallT call) :
        ApiProvider<CallT>{ async, identityName, std::move(call) },
        m_entity{ std::move(entity) }
    {
    }

protected:
    using ApiProvider<CallT>::ResultT;

    HRESULT Begin(TaskQueue&& queue) override
    {
        m_entity->MakePlayFabCallWithAuthRetry(queue, std::move(ApiProvider<CallT>::m_call)).Finally([this](Result<ResultT> result)
        {
            if (Succeeded(result))
            {
                TRACE_VERBOSE("EntityApiProvider[ID=%s] Call suceeded (hr=0x%08x)", identityName, result.hr);
                this->Complete(this->ApiProvider<CallT>::StoreResult(std::move(result)));
            }
            else
            {
                TRACE_ERROR("EntityApiProvider[ID=%s] Call failed with message \"%s\" (hr=0x%08x)", identityName, result.errorMessage.data(), result.hr);
                this->Fail(result.hr);
            }
        });
        return S_OK;
    }

private:
    using ApiProvider<CallT>::m_call;
    using ApiProvider<CallT>::identityName;

    SharedPtr<Entity> m_entity;
};

template<typename CallT, size_t n>
UniquePtr<EntityApiProvider<CallT>> MakeEntityProvider(XAsyncBlock* async, const char(&identityName)[n], SharedPtr<Entity> entity, CallT call)
{
    return MakeUnique<EntityApiProvider<CallT>>(async, identityName, std::move(entity), std::move(call));
}

// XAsync Provider for PlayFab login API calls
template<typename CallT>
class AuthCallProvider : public Provider
{
public:
    static_assert(std::is_same_v<SharedPtr<Entity>, typename Detail::UnwrapAsyncT<typename std::result_of_t<CallT(const TaskQueue&)>>>, "CallT must return a SharedPt<Entity>");

    template<size_t n>
    AuthCallProvider(XAsyncBlock* async, const char(&identityName)[n], CallT authCall)
        : Provider{ async, identityName },
        m_call{ authCall }
    {
    }

protected:
    // Always kick of the API call during XAsyncOp::Begin so we don't have to worry about lifetime of request
    // and API objects (even though they are hidden as part of a std::bind)
    HRESULT Begin(TaskQueue&& queue) override
    {
        m_call(queue).Finally([this](Result<SharedPtr<Entity>> result)
        {
            if (Succeeded(result))
            {
                TRACE_VERBOSE("AuthCallProvider[ID=%s] Call suceeded (hr=0x%08x)", identityName, result.hr);
                this->m_entity = result.ExtractPayload();
                this->Complete(sizeof(PlayFabEntity*));
            }
            else
            {
                TRACE_ERROR("AuthCallProvider[ID=%s] Call failed with message \"%s\" (hr=0x%08x)", identityName, result.errorMessage.data(), result.hr);
                this->Fail(result.hr);
            }
        });

        return S_OK;
    }

    HRESULT GetResult(void* buffer, size_t bufferSize) override
    {
        assert(bufferSize == sizeof(PlayFabEntity*));
        auto contextPtr = static_cast<PlayFabEntity**>(buffer);
        *contextPtr = MakeUnique<PlayFabEntity>(m_entity).release();
        return S_OK;
    }

private:
    CallT m_call;
    SharedPtr<Entity> m_entity;
};

template<typename CallT, size_t n>
UniquePtr<AuthCallProvider<CallT>> MakeAuthProvider(XAsyncBlock* async, const char(&identityName)[n], CallT authCall)
{
    return MakeUnique<AuthCallProvider<CallT>>(async, identityName, std::move(authCall));
}

}

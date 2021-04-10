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
    ApiProvider(XAsyncBlock* async, CallT call) :
        Provider{ async },
        m_call{ call }
    {
    }

protected:
    // Always kick of the API call during XAsyncOp::Begin so we don't have to worry about lifetime of request
    // and API objects (even though they are hidden as part of a std::bind)
    HRESULT Begin(TaskQueue&& queue) override
    {
        MakeCall(std::move(queue));
        return S_OK;
    }

    HRESULT GetResult(void* buffer, size_t bufferSize) override
    {
        GetResultHelper(buffer, bufferSize);
        return S_OK;
    }

private:
    // How we deliver the XAsync result will depend on ResultT of m_call:
    // 1. If there is no result payload, there is no result to store & GetResult Op should never be called.
    // 2. If the result payload is serializable, store the result, report the needed result buffer size, and serialize it into
    //    the clients buffer during GetResult.
    // 3. If the result payload isn't serializable, store the result, report the needed buffer size as sizeof(PlayFabResultHandle),
    //    and during GetResult, create a PlayFabResult object return a pointer to that.
    using ResultT = typename Detail::UnwrapAsyncT<typename std::result_of_t<CallT(const TaskQueue&)>>;

    template<typename T = ResultT>
    void MakeCall(TaskQueue&& queue, typename std::enable_if_t<std::is_void_v<T>>* = 0)
    {
        m_call(queue).Finally([this](Result<void> result)
        {
            if (Succeeded(result))
            {
                this->Complete(0);
            }
            else
            {
                this->Fail(result.hr);
            }
        });
    }

    template<typename T = ResultT>
    void MakeCall(TaskQueue&& queue, typename std::enable_if_t<!std::is_void_v<T> && std::is_base_of_v<SerializableModel, T>>* = 0)
    {
        m_call(queue).Finally([this](Result<ResultT> result)
        {
            if (Succeeded(result))
            {
                this->m_result.emplace(std::move(result));
                this->Complete(this->m_result->Payload().SerializedSize());
            }
            else
            {
                this->Fail(result.hr);
            }
        });
    }

    template<typename T = ResultT>
    void MakeCall(TaskQueue&& queue, typename std::enable_if_t<!std::is_void_v<T> && !std::is_base_of_v<SerializableModel, T>>* = 0)
    {
        m_call(queue).Finally([this](Result<ResultT> result)
        {
            if (Succeeded(result))
            {
                this->m_result.emplace(std::move(result));
                this->Complete(sizeof(PlayFabResultHandle));
            }
            else
            {
                this->Fail(result.hr);
            }
        });
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
};

template<typename CallT>
UniquePtr<ApiProvider<CallT>> MakeProvider(XAsyncBlock* async, CallT call)
{
    return MakeUnique<ApiProvider<CallT>>(async, std::move(call));
}

// XAsync Provider for PlayFab login API calls
template<typename CallT>
class AuthCallProvider : public Provider
{
public:
    static_assert(std::is_same_v<SharedPtr<Entity>, typename Detail::UnwrapAsyncT<typename std::result_of_t<CallT(const TaskQueue&)>>>, "CallT must return an AuthContext");

    AuthCallProvider(XAsyncBlock* async, CallT authCall)
        : Provider{ async },
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
                this->m_entity = result.ExtractPayload();
                this->Complete(sizeof(PlayFabEntity*));
            }
            else
            {
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

template<typename CallT>
UniquePtr<AuthCallProvider<CallT>> MakeAuthProvider(XAsyncBlock* async, CallT authCall)
{
    return MakeUnique<AuthCallProvider<CallT>>(async, std::move(authCall));
}

}
#include "stdafx.h"
#include <playfab/PlayFabEventManagerApi.h>
#include "Event.h"
#include "Entity.h"

using namespace PlayFab;
using namespace PlayFab::EventManager;

HRESULT PlayFabEventManagerEventCreate(
    _In_ PlayFabEventManagerEventType eventType,
    _In_ const char* eventName,
    _In_opt_ const char* eventNamespace,
    _In_opt_ const char* entityId,
    _In_opt_ const char* entityType,
    _Out_ PlayFabEventManagerEventHandle* eventHandle
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(eventHandle);

    auto event = MakeShared<Event>(eventType, eventName);
    if (eventNamespace)
    {
        event->SetNamespace(eventNamespace);
    }
    if (entityId)
    {
        event->SetEntity(entityId, entityType);
    }

    *eventHandle = MakeUnique<PlayFabEventManagerEvent>(event).release();
    return S_OK;
}

HRESULT PlayFabEventManagerEventDuplicateHandle(
    _In_ PlayFabEventManagerEventHandle eventHandle,
    _Out_ PlayFabEventManagerEventHandle* duplicatedEventHandle
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(eventHandle);
    RETURN_HR_INVALIDARG_IF_NULL(duplicatedEventHandle);

    *duplicatedEventHandle = MakeUnique<PlayFabEventManagerEvent>(*eventHandle).release();
    return S_OK;
}

void PlayFabEventManagerEventCloseHandle(
    _Out_ PlayFabEventManagerEventHandle eventHandle
) noexcept
{
    UniquePtr<PlayFabEventManagerEvent>{ eventHandle };
}

HRESULT PlayFabEventManagerEventSetNamespace(
    _In_ PlayFabEventManagerEventHandle eventHandle,
    _In_ const char* eventNamespace
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(eventHandle);
    RETURN_HR_INVALIDARG_IF_NULL(eventNamespace);

    eventHandle->event->SetNamespace(eventNamespace);
    return S_OK;
}

HRESULT PlayFabEventManagerEventSetEntity(
    _In_ PlayFabEventManagerEventHandle eventHandle,
    _In_ const char* entityId,
    _In_opt_ const char* entityType
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(eventHandle);
    RETURN_HR_INVALIDARG_IF_NULL(entityId);

    eventHandle->event->SetEntity(entityId, entityType);
    return S_OK;
}

template<typename T>
HRESULT PlayFabEventManagerEventSetProperty(
    _In_ PlayFabEventManagerEventHandle eventHandle,
    _In_ const char* key,
    _In_ T value
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(eventHandle);
    RETURN_HR_INVALIDARG_IF_NULL(key);

    eventHandle->event->SetProperty(key, value);
    return S_OK;
}

HRESULT PlayFabEventManagerEventSetStringProperty(
    _In_ PlayFabEventManagerEventHandle eventHandle,
    _In_ const char* key,
    _In_ const char* value
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(value);
    return PlayFabEventManagerEventSetProperty(eventHandle, key, value);
}

HRESULT PlayFabEventManagerEventSetBooleanProperty(
    _In_ PlayFabEventManagerEventHandle eventHandle,
    _In_ const char* key,
    _In_ bool value
) noexcept
{
    return PlayFabEventManagerEventSetProperty(eventHandle, key, value);
}

HRESULT PlayFabEventManagerEventSetIntProperty(
    _In_ PlayFabEventManagerEventHandle eventHandle,
    _In_ const char* key,
    _In_ int64_t value
) noexcept
{
    return PlayFabEventManagerEventSetProperty(eventHandle, key, value);
}

HRESULT PlayFabEventManagerEventSetUintProperty(
    _In_ PlayFabEventManagerEventHandle eventHandle,
    _In_ const char* key,
    _In_ uint64_t value
) noexcept
{
    return PlayFabEventManagerEventSetProperty(eventHandle, key, value);
}

HRESULT PlayFabEventManagerEventSetDoubleProperty(
    _In_ PlayFabEventManagerEventHandle eventHandle,
    _In_ const char* key,
    _In_ double value
) noexcept
{
    return PlayFabEventManagerEventSetProperty(eventHandle, key, value);
}

HRESULT PlayFabEventManagerWriteEventAsync(
    _In_ PlayFabEntityHandle entityHandle,
    _In_ PlayFabEventManagerEventHandle eventHandle,
    _In_opt_ void* callbackContext,
    _In_opt_ PlayFabEventManagerWriteEventCompletionCallback* callback
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);
    RETURN_HR_INVALIDARG_IF_NULL(eventHandle);

    entityHandle->entity->eventManagerAPI.WriteEvent(eventHandle->event).Finally([ callbackContext, callback ](Result<String> result)
    {
        if (callback)
        {
            const char* assignedId{ nullptr };
            if (Succeeded(result))
            {
                assignedId = result.Payload().data();
            }
            callback(callbackContext, result.hr, assignedId);
        }
    });

    return S_OK;
}

HRESULT PlayFabEventManagerTerminate(
    _In_ PlayFabEntityHandle entityHandle,
    _In_ bool wait,
    _In_opt_ void* callbackContext,
    _In_opt_ PlayFabEventManagerTerminatedCallback* callback
) noexcept
{
    RETURN_HR_INVALIDARG_IF_NULL(entityHandle);

    struct TerminationContext
    {
        void* callbackContext;
        PlayFabEventManagerTerminatedCallback* callback;
        std::mutex mutex;
        std::condition_variable cv;
        bool terminated{ false };
    };

    auto context = MakeShared<TerminationContext>();
    context->callbackContext = callbackContext;
    context->callback = callback;

    HRESULT hr = entityHandle->entity->eventManagerAPI.Terminate([context](void)
    {
        if (context->callback)
        {
            context->callback(context->callbackContext);
        }

        std::lock_guard<std::mutex> lock{ context->mutex };
        context->terminated = true;
    });

    RETURN_IF_FAILED(hr);

    if (wait)
    {
        std::unique_lock<std::mutex> lock{ context->mutex };
        while (!context->terminated)
        {
            context->cv.wait(lock);
        }
    }

    return S_OK;
}

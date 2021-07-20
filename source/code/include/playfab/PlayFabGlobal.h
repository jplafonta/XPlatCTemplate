// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

#include <httpClient/pal.h>
#include <httpClient/async.h>

extern "C"
{

/// <summary>
/// A callback invoked every time a new memory buffer must be dynamically allocated by the library.
/// This callback is optionally installed by calling PlayFabMemSetFunctions()
/// 
/// The callback must allocate and return a pointer to a contiguous block of memory of the 
/// specified size that will remain valid until the app's corresponding PlayFabMemFreeFunction 
/// callback is invoked to release it.
/// 
/// Every non-null pointer returned by this method will be subsequently passed to the corresponding
/// PlayFabMemFreeFunction callback once the memory is no longer needed.
/// </summary>
/// <returns>A pointer to an allocated block of memory of the specified size, or a null 
/// pointer if allocation failed.</returns>
/// <param name="size">The size of the allocation to be made. This value will never be zero.</param>
/// <param name="memoryType">An opaque identifier representing the internal category of 
/// memory being allocated.</param>
typedef _Ret_maybenull_ _Post_writable_byte_size_(size) void* STDAPIVCALLTYPE PlayFabMemAllocFunction(
    _In_ size_t size,
    _In_ uint32_t memoryType
);

/// <summary>
/// A callback invoked every time a previously allocated memory buffer is no longer needed by 
/// the library and can be freed. This callback is optionally installed by calling PlayFabMemSetFunctions()
///
/// The callback is invoked whenever the library has finished using a memory buffer previously 
/// returned by the app's corresponding PlayFabMemAllocFunction such that the application can free the
/// memory buffer.
/// </summary>
/// <param name="pointer">The pointer to the memory buffer previously allocated. This value will
/// never be a null pointer.</param>
/// <param name="memoryType">An opaque identifier representing the internal category of 
/// memory being allocated.</param>
typedef void STDAPIVCALLTYPE PlayFabMemFreeFunction(
    _In_ _Post_invalid_ void* pointer,
    _In_ uint32_t memoryType
);

/// <summary>
/// Optionally sets the memory hook functions to allow callers to control route memory 
/// allocations to their own memory manager. This must be called before PlayFabInitialize() 
/// and can not be called again once memory hooks have been set.
///
/// This method allows the application to install custom memory allocation routines in order 
/// to service all requests for new memory buffers instead of using default allocation routines.
/// </summary>
/// <param name="memAllocFunc">A pointer to the custom allocation callback to use.</param>
/// <param name="memFreeFunc">A pointer to the custom freeing callback to use.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI PlayFabMemSetFunctions(
    _In_opt_ PlayFabMemAllocFunction* memAllocFunc,
    _In_opt_ PlayFabMemFreeFunction* memFreeFunc
) noexcept;

/// <summary>
/// Gets the memory hook functions to allow callers to control route memory allocations to their 
/// own memory manager. This method allows the application get the default memory allocation routines.
/// This can be used along with PlayFabMemSetFunctions() to monitor all memory allocations.
/// </summary>
/// <param name="memAllocFunc">Set to the current allocation callback. Returns the default routine 
/// if not previously set</param>
/// <param name="memFreeFunc">Set to the to the current memory free callback. Returns the default 
/// routine if not previously set</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI PlayFabMemGetFunctions(
    _Out_ PlayFabMemAllocFunction** memAllocFunc,
    _Out_ PlayFabMemFreeFunction** memFreeFunc
) noexcept;

/// <summary>
/// Handle to global state created by PlayFabInitialize. A PlayFabStateHandle will be needed to call login methods.
/// Cleaned up with PlayFabCleanupAsync.
/// </summary>
typedef struct PlayFabGlobalState* PlayFabStateHandle;

/// <summary>
/// Create PlayFab global state.
/// </summary>
/// <param name="titleId">TitleId for the title. Found in the Game Manager for your title on the PlayFab Website.</param>
/// <param name="backgroundQueue">An XTaskQueue that should be used for background work. If no queue is a default (threadpool) queue will be used.</param>
/// <param name="stateHandle">Pointer to PlayFabStateHandle to write.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabInitialize(
    _In_z_ const char* titleId,
    _In_opt_ XTaskQueueHandle backgroundQueue,
    _Outptr_ PlayFabStateHandle* stateHandle
) noexcept;

#if defined(ENABLE_PLAYFABSERVER_API) || defined(ENABLE_PLAYFABADMIN_API)
/// <summary>
/// Create PlayFab global state. Should be only used when implementing admin or server code.
/// </summary>
/// <param name="titleId">TitleId for the title. Found in the Game Manager for your title on the PlayFab Website.</param>
/// <param name="secretKey">Key to be used for Authentication for Server and Admin APIs.</param>
/// <param name="backgroundQueue">An XTaskQueue that should be used for background work. If no queue is a default (threadpool) queue will be used.</param>
/// <param name="stateHandle">Pointer to PlayFabStateHandle to write.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabAdminInitialize(
    _In_z_ const char* titleId,
    _In_z_ const char* secretKey,
    _In_opt_ XTaskQueueHandle backgroundQueue,
    _Outptr_ PlayFabStateHandle* stateHandle
) noexcept;
#endif


/// <summary>
/// Cleanup PlayFab global state.
/// </summary>
/// <param name="stateHandle">Handle to the PlayFab state to cleanup.</param>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>Asynchronous result returned via XAsyncGetStatus.</remarks>
HRESULT PlayFabCleanupAsync(
    _In_ PlayFabStateHandle stateHandle,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Opaque handle to PlayFab API results. Lifetime of result objects may be tied to a PlayFabResultHandle.
/// </summary>
typedef struct PlayFabResult* PlayFabResultHandle;

/// <summary>
/// Duplicate a result handle.
/// </summary>
/// <param name="resultHandle">PlayFabResultHandle to duplicate.</param>
/// <param name="duplicatedHandle">Pointer to duplicated handle.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabResultDuplicateHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabResultHandle* duplicatedHandle
) noexcept;

/// <summary>
/// Close a result handle. If it is the last handle, the underlying result object will be destroyed.
/// </summary>
/// <param name="resultHandle">PlayFabResultHandle to close.</param>
/// <returns>Result code for this API operation.</returns>
void PlayFabResultCloseHandle(
    _In_ PlayFabResultHandle resultHandle
) noexcept;

/// <summary>
/// A token returned when registering a callback to identify the registration. This token is later used 
/// to unregister the callback.
/// </summary>
typedef uint64_t PlayFabRegistrationToken;

}

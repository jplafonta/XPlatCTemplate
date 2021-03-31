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
/// and can not be called again until after calling PlayFabCleanupAsync()
///
/// This method allows the application to install custom memory allocation routines in order 
/// to service all requests for new memory buffers instead of using default allocation routines.
///
/// The <paramref name="memAllocFunc" /> and <paramref name="memFreeFunc" /> parameters can be null
/// pointers to restore the default routines. Both callback pointers must be null or both must 
/// be non-null. Mixing custom and default routines is not permitted.
/// </summary>
/// <param name="memAllocFunc">A pointer to the custom allocation callback to use, or a null 
/// pointer to restore the default.</param>
/// <param name="memFreeFunc">A pointer to the custom freeing callback to use, or a null 
/// pointer to restore the default.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMemSetFunctions(
    _In_opt_ PlayFabMemAllocFunction* memAllocFunc,
    _In_opt_ PlayFabMemFreeFunction* memFreeFunc
) noexcept;

/// <summary>
/// Gets the memory hook functions to allow callers to control route memory allocations to their 
/// own memory manager.  This method allows the application get the default memory allocation routines.
/// This can be used along with XblMemSetFunctions() to monitor all memory allocations.
/// </summary>
/// <param name="memAllocFunc">Set to the current allocation callback.  Returns the default routine 
/// if not previously set</param>
/// <param name="memFreeFunc">Set to the to the current memory free callback.  Returns the default 
/// routine if not previously set</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMemGetFunctions(
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
/// <param name="stateHandle">Pointer to PlayFabStateHandle.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabInitialize(
    _Outptr_ PlayFabStateHandle* stateHandle
) noexcept;

/// <summary>
/// Cleanup PlayFab global state.
/// </summary>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <param name="stateHandle">Handle to the PlayFab state to cleanup.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>Asynchronous result returned via XAsyncGetStatus.</remarks>
HRESULT PlayFabCleanupAsync(
    _In_ XAsyncBlock* async,
    _In_ PlayFabStateHandle stateHandle
) noexcept;

}

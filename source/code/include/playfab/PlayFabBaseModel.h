// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

#include <httpClient/pal.h>

extern "C"
{

/// Property annotations for PlayFab Models
#define PF_MAP
#define PF_ARRAY
#define PF_OPTIONAL
#define PF_COLLECTION_COUNT

/// <summary>
/// String representation of a Json Object
/// </summary>
typedef struct PlayFabJsonObject
{
    const char* stringValue;
} PlayFabJsonObject;

/// <summary>
/// Dictionary entry for an associative array with string values.
/// </summary>
typedef struct PlayFabStringDictionaryEntry
{
    const char* key;
    const char* value;
} PlayFabStringDictionaryEntry;

/// <summary>
/// Dictionary entry for an associative array with int32_t values.
/// </summary>
typedef struct PlayFabInt32DictionaryEntry
{
    const char* key;
    int32_t value;
} PlayFabInt32DictionaryEntry;

/// <summary>
/// Dictionary entry for an associative array with uint32_t values.
/// </summary>
typedef struct PlayFabUint32DictionaryEntry
{
    const char* key;
    uint32_t value;
} PlayFabUint32DictionaryEntry;

/// <summary>
/// Dictionary entry for an associative array with time_t values.
/// </summary>
typedef struct PlayFabDateTimeDictionaryEntry
{
    const char* key;
    time_t value;
} PlayFabDateTimeDictionaryEntry;

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

}

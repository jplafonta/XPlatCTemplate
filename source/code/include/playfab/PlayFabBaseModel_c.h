#pragma once

#include <httpClient/pal.h>

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

typedef struct PlayFabStringDictionaryEntry
{
    const char* key;
    const char* value;
} PlayFabStringDictionaryEntry;

typedef struct PlayFabInt32DictionaryEntry
{
    const char* key;
    int32_t value;
} PlayFabInt32DictionaryEntry;

typedef struct PlayFabUint32DictionaryEntry
{
    const char* key;
    uint32_t value;
} PlayFabUint32DictionaryEntry;

typedef struct PlayFabDateTimeDictionaryEntry
{
    const char* key;
    time_t value;
} PlayFabDateTimeDictionaryEntry;


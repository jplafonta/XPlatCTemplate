#pragma once

#include <httpClient/pal.h>

typedef const char* PlayFabJsonString;

struct PlayFabStringDictionaryEntry
{
    const char* key;
    const char* value;
};

struct PlayFabInt32DictionaryEntry
{
    const char* key;
    int32_t value;
};

struct PlayFabUint32DictionaryEntry
{
    const char* key;
    uint32_t value;
};

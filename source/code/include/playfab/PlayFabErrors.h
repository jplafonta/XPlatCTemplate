// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

#include <httpClient/pal.h>

extern "C"
{

// PlayFab HRESULT codes

// FACILITY_XBOX + 0x5300 + offset
#define E_PLAYFAB_EVENTMANAGERBUFFEROVERFLOW            MAKE_E_HC(0x5300L) // 0x89235300
#define E_PLAYFAB_EVENTMANAGERINVALIDOPERATION          MAKE_E_HC(0x5301L) // 0x89235301

// E_PLAYFAB_INTERNAL_* values should never be returned to callers
#define E_PLAYFAB_INTERNAL_EVENTMANAGERBUFFEREMPTY      MAKE_E_HC(0x5350L) // 0x89235350

}

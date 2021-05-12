// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

#include <httpClient/pal.h>

extern "C"
{

// PlayFab HRESULT error codes (FACILITY_XBOX + 0x5300 + offset)

// Errors returned from any PlayFab service wrapper API.
// It is safe to retry calls that fail with the following errors (with an exponential delay back-off).
#define E_PLAYFAB_RATELIMITEXCEEDED                     MAKE_E_HC(0x5300L) // 0x89235300
#define E_PLAYFAB_OVERGAMEMANAGERLIMIT                  MAKE_E_HC(0x5301L) // 0x89235301
#define E_PLAYFAB_SERVICEUNAVAILABLE                    MAKE_E_HC(0x5302L) // 0x89235302

// If the following errors are returned, the call shouldn't be retried. Without changes, the call will never succeed.
#define E_PLAYFAB_ACCOUNTBANNED                         MAKE_E_HC(0x5310L) // 0x89235310
#define E_PLAYFAB_ACCOUNTDELETED                        MAKE_E_HC(0x5311L) // 0x89235311
#define E_PLAYFAB_ACCOUNTNOTFOUND                       MAKE_E_HC(0x5312L) // 0x89235312
#define E_PLAYFAB_APIREQUESTSDISABLEDFORTITLE           MAKE_E_HC(0x5313L) // 0x89235313
#define E_PLAYFAB_INVALIDREQUEST                        MAKE_E_HC(0x5314L) // 0x89235314
#define E_PLAYFAB_NOTAUTHENTICATED                      MAKE_E_HC(0x5315L) // 0x89235315
#define E_PLAYFAB_NOTAUTHORIZED                         MAKE_E_HC(0x5316L) // 0x89235316
#define E_PLAYFAB_PROFILEDOESNOTEXIST                   MAKE_E_HC(0x5317L) // 0x89235317
#define E_PLAYFAB_TITLEDELETED                          MAKE_E_HC(0x5318L) // 0x89235318
#define E_PLAYFAB_UNKNOWNERROR                          MAKE_E_HC(0x5319L) // 0x89235319
#define E_PLAYFAB_SERVICEERROR                          MAKE_E_HC(0x531AL) // 0x8923531A

// Errors returned from PlayFabEventMangager APIs.
#define E_PLAYFAB_EVENTMANAGERBUFFEROVERFLOW            MAKE_E_HC(0x5340L) // 0x89235340
#define E_PLAYFAB_EVENTMANAGERINVALIDOPERATION          MAKE_E_HC(0x5341L) // 0x89235341

// E_PLAYFAB_INTERNAL_* values should never be returned to callers
#define E_PLAYFAB_INTERNAL_EVENTMANAGERBUFFEREMPTY      MAKE_E_HC(0x5350L) // 0x89235350
#define E_PLAYFAB_INTERNAL_QOSTIMEOUTEXCEEDED           MAKE_E_HC(0x5351L) // 0x89235351

}

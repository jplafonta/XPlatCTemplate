// Copyright (C) Microsoft Corporation. All rights reserved.
// stdafx.h : The file for declaring precompiled headers.

#ifndef STDAFX_H
#define STDAFX_H

#pragma once

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <array>
#include <assert.h>

#include <httpClient/pal.h>
#include <httpClient/async.h>
#include "JsonHeaders.h"
#include "StdOptional.h"
#include "ResultMacros.h"
#include "InternalMemory.h"
#include "Result.h"
#include "Error.h"
#include "PlayFabPlatformMacros.h"
#include "PlatformUtils.h"
#include "PlayFabApiSettings.h"
#include "PlayFabAuthenticationContext.h"

#define UNREFERENCED_PARAMETER(P) (P)

#ifdef PLAYFAB_PLATFORM_LINUX
#include <stdio.h>
#endif // PLAYFAB_PLATFORM_LINUX

#ifdef PLAYFAB_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#endif // PLAYFAB_PLATFORM_WINDOWS

#endif // STDAFX_H

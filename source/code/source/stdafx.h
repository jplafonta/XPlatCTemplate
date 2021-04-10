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

// libHttpClient headers
#include <httpClient/pal.h>
#include <httpClient/async.h>

// rapidjson headers
#define RAPIDJSON_NAMESPACE PlayFab::rapidjson
#define RAPIDJSON_NAMESPACE_BEGIN namespace PlayFab { namespace rapidjson {
#define RAPIDJSON_NAMESPACE_END } }
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>

#include "StdOptional.h"
#include "ResultMacros.h"
#include "InternalMemory.h"
#include "Result.h"
#include "Error.h"
#include "PlayFabPlatformMacros.h"
#include "PlatformUtils.h"

#define UNREFERENCED_PARAMETER(P) (P)

#ifdef PLAYFAB_PLATFORM_LINUX
#include <stdio.h>
#endif // PLAYFAB_PLATFORM_LINUX

#ifdef PLAYFAB_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#endif // PLAYFAB_PLATFORM_WINDOWS

#endif // STDAFX_H

// Copyright (C) Microsoft Corporation. All rights reserved.
//
// This header file is used to include platform-specific headers of JsonCpp library.

#pragma once

#define RAPIDJSON_NAMESPACE PlayFab::rapidjson
#define RAPIDJSON_NAMESPACE_BEGIN namespace PlayFab { namespace rapidjson {
#define RAPIDJSON_NAMESPACE_END } }

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>

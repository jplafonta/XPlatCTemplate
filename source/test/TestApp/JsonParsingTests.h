// Copyright (C) Microsoft Corporation. All rights reserved.

#pragma once

#include "TestCase.h"
#include <JsonUtils.h>

namespace PlayFabUnit
{
    class JsonParsingTests : public TestCase
    {
    private:
        void BasicJsonParsing(TestContext& testContext);

    protected:
        void AddTests() override;

    public:
        // Synchronous test
        void Tick(TestContext&) override {}
    };
}

// Copyright (C) Microsoft Corporation. All rights reserved.

#pragma once

#include "TestCase.h"
#include <JsonUtils.h>

namespace PlayFabUnit
{
    class ModelTests : public TestCase
    {
    private:
        void BasicModelTest(TestContext& testContext);
        void TestMove(TestContext& testContext);
        void TestTimeDictionarySpecialization(TestContext& testContext);

    protected:
        void AddTests() override;

    public:
        // Synchronous test
        void Tick(TestContext&) override {}
    };
}

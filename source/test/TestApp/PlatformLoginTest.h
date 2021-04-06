// Copyright (C) Microsoft Corporation. All rights reserved.

#pragma once

#if !defined(DISABLE_PLAYFABCLIENT_API)

#include "TestCase.h"
#include <playfab/PlayFabGlobal.h>

namespace PlayFabUnit
{
    class PlatformLoginTest : public PlayFabApiTestCase
    {
    private:
        void TestPlatformSpecificLogin(TestContext& testContext);

    protected:
        void AddTests() override;

    public:
        PlayFabStateHandle stateHandle{ nullptr };

        void ClassSetUp() override;
        void SetUp(TestContext& testContext) override;
        void Tick(TestContext& testContext) override;
        void TearDown(TestContext& testContext) override;
        void ClassTearDown() override;
    };
}

#endif

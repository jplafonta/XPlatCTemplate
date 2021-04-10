// Copyright (C) Microsoft Corporation. All rights reserved.

// This file is meant to be compiled into projects that do not have a useful/valid platform specific login.
// When present, add instead the platform specific file, and do NOT add this one

#include "TestAppPch.h"

#if !defined(DISABLE_PLAYFABCLIENT_API)

#include "TestContext.h"
#include "PlatformLoginTest.h"
#include <playfab/PlayFabClientAuthApi.h>

namespace PlayFabUnit
{
    // CLIENT API
    // Attempt a successful login
    void PlatformLoginTest::TestPlatformSpecificLogin(TestContext& testContext)
    {
        PlayFabClientLoginWithCustomIDRequest request{};
        //request.customId = PlayFabSettings::buildIdentifier.data();
        request.customId = "CustomId";
        bool createAccount = true;
        request.createAccount = &createAccount;
        request.titleId = testTitleData.titleId.data();

        auto async = std::make_unique<XAsyncBlock>();
        async->context = &testContext;
        async->callback = [](XAsyncBlock* async)
        {
            std::unique_ptr<XAsyncBlock> reclaim{ async };

            auto testContext = static_cast<TestContext*>(async->context);
            PlayFabEntityHandle entityHandle{};
            HRESULT hr = PlayFabGetAuthResult(async, &entityHandle);

            if (SUCCEEDED(hr))
            {
                testContext->Pass();
            }
            else
            {
                std::stringstream ss;
                ss << "LoginWithCustomId Failed with hr=" << std::hex << hr;
                testContext->Fail(ss.str());
            }

            PlayFabEntityCloseHandle(entityHandle);
        };

        HRESULT hr = PlayFabClientLoginWithCustomIDAsync(stateHandle, &request, async.get());
        if (FAILED(hr))
        {
            std::stringstream ss;
            ss << "LoginWithCustomId Failed with hr=" << std::hex << hr;
            testContext.Fail(ss.str());
        }
        else
        {
            async.release();
        }
    }

    void PlatformLoginTest::AddTests()
    {
        AddTest("TestPlatformSpecificLogin", &PlatformLoginTest::TestPlatformSpecificLogin);
    }

    void PlatformLoginTest::ClassSetUp()
    {
        HRESULT hr = PlayFabInitialize(testTitleData.titleId.data(), nullptr, &stateHandle);
        assert(SUCCEEDED(hr));
        UNREFERENCED_PARAMETER(hr);
    }

    void PlatformLoginTest::SetUp(TestContext&)
    {
    }

    void PlatformLoginTest::Tick(TestContext&)
    {
    }

    void PlatformLoginTest::TearDown(TestContext&)
    {
    }

    void PlatformLoginTest::ClassTearDown()
    {
        XAsyncBlock async{};
        HRESULT hr = PlayFabCleanupAsync(stateHandle, &async);
        assert(SUCCEEDED(hr));

        hr = XAsyncGetStatus(&async, true);
        assert(SUCCEEDED(hr));

        UNREFERENCED_PARAMETER(hr);
    }
}

#endif

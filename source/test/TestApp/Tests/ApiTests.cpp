#include "TestAppPch.h"
#include "TestContext.h"
#include "ApiTests.h"
#include "XAsyncHelper.h"
#include "playfab/PFCore.h"

namespace PlayFabUnit
{


void ApiTests::TestApiStaticSizeResult(TestContext& testContext)
{
    struct GetTimeResult : public XAsyncResult
    {
        PFTitleDataManagementGetTimeResult result{};

        HRESULT Get(XAsyncBlock* async) override
        {
            return PFTitleDataManagementClientGetTimeGetResult(async, &result);
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetTimeResult>>(testContext);

    HRESULT hr = PFTitleDataManagementClientGetTimeAsync(entityHandle, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabClientGetTimeAsync", hr);
        return;
    }
    async.release();
}

void ApiTests::TestApiSerializableResult(TestContext& testContext)
{
    static std::string groupId;

    struct CreateSharedGroupResult : public XAsyncResult
    {
        PFGroupsCreateSharedGroupResult* result;

        HRESULT Get(XAsyncBlock* async) override
        {
            size_t requiredBufferSize;
            RETURN_IF_FAILED(PFGroupsClientCreateSharedGroupGetResultSize(async, &requiredBufferSize));

            resultBuffer.resize(requiredBufferSize);
            return PFGroupsClientCreateSharedGroupGetResult(async, resultBuffer.size(), resultBuffer.data(), &result, nullptr);
        }

        HRESULT Validate() override
        {
            if (memcmp(result->sharedGroupId, groupId.data(), groupId.length()))
            {
                return E_FAIL;
            }
            return S_OK;
        }
    };

    auto async = std::make_unique<XAsyncHelper<CreateSharedGroupResult>>(testContext);

    std::stringstream uniqueGroupId;
    uniqueGroupId << "GroupId_" << time(nullptr);
    groupId = uniqueGroupId.str();

    PFGroupsCreateSharedGroupRequest request{ groupId.data() };
    HRESULT hr = PFGroupsClientCreateSharedGroupAsync(entityHandle, &request, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabClientCreateSharedGroupAsync", hr);
        return;
    }

    async.release();
}

void ApiTests::TestApiResultHandle(TestContext& testContext)
{
    struct GetPlayerProfileResult : public XAsyncResult
    {
        PFAccountManagementGetPlayerProfileResult* result{ nullptr };

        HRESULT Get(XAsyncBlock* async) override
        {
            return PFAccountManagementClientGetPlayerProfileGetResult(async, &resultHandle, &result);
        }

        HRESULT Validate() override
        {
            // Any fields we can easily validate here?
            return S_OK;
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetPlayerProfileResult>>(testContext);

    PFAccountManagementGetPlayerProfileRequest request{};
    HRESULT hr = PFAccountManagementClientGetPlayerProfileAsync(entityHandle, &request, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabClientGetPlayerProfileAsync", hr);
        return;
    }
    async.release();
}

void ApiTests::TestApiSessionTicket(TestContext& /*testContext*/)
{
    // Above API calls are using SessionTicket for auth, so just skipping this
}

void ApiTests::TestApiEntityToken(TestContext& testContext)
{
    struct GetProfilesResult : public XAsyncResult
    {
        PFAccountManagementGetEntityProfileResponse* result{ nullptr };

        HRESULT Get(XAsyncBlock* async) override
        {
            return PFAccountManagementGetProfileGetResult(async, &resultHandle, &result);
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetProfilesResult>>(testContext);

    PFAccountManagementGetEntityProfileRequest request{};
    HRESULT hr = PFAccountManagementGetProfileAsync(entityHandle, &request, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabProfilesGetProfileAsync", hr);
        return;
    }
    async.release();
}

void ApiTests::TestApiSecretKey(TestContext& testContext)
{
    struct GetTitleDataResult : public XAsyncResult
    {
        PFTitleDataManagementGetTitleDataResult* result;

        HRESULT Get(XAsyncBlock* async) override
        {
            return PFTitleDataManagementAdminGetTitleDataGetResult(async, &resultHandle, &result);
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetTitleDataResult>>(testContext);

    PFTitleDataManagementGetTitleDataRequest request{};
    HRESULT hr = PFTitleDataManagementAdminGetTitleDataAsync(stateHandle, &request, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabAdminGetTitleDataAsync", hr);
        return;
    }
    async.release();
}

void ApiTests::TestApiNoAuth(TestContext& testContext)
{
    struct GetTitlePublicKeyResult : public XAsyncResult
    {
        PFAuthenticationGetTitlePublicKeyResult* result;

        HRESULT Get(XAsyncBlock* async) override
        {
            size_t requiredBufferSize;
            RETURN_IF_FAILED(PFAuthenticationClientGetTitlePublicKeyGetResultSize(async, &requiredBufferSize));

            resultBuffer.resize(requiredBufferSize);
            return PFAuthenticationClientGetTitlePublicKeyGetResult(async, resultBuffer.size(), resultBuffer.data(), &result, nullptr);
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetTitlePublicKeyResult>>(testContext);

    PFAuthenticationGetTitlePublicKeyRequest request{};
    request.titleId = testTitleData.titleId.data();
    request.titleSharedSecret = testTitleData.developerSecretKey.data();
    HRESULT hr = PFAuthenticationClientGetTitlePublicKeyAsync(stateHandle, &request, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabClientGetTitlePublicKeyAsync", hr);
        return;
    }
    async.release();
}

void ApiTests::TestGetEntityTokenWithAuthContext(TestContext& testContext)
{
    auto async = std::make_unique<XAsyncHelper<XAsyncResult>>(testContext);

    PFAuthenticationGetEntityTokenRequest request{};
    HRESULT hr = PFEntityGetEntityTokenAsync(entityHandle, &request, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabEntityGetEntityTokenAsync", hr);
        return;
    }
    async.release();
}

void ApiTests::TestGetEntityTokenWithSecretKey(TestContext& testContext)
{
    struct GetEntityTokenResult : public XAsyncResult
    {
        PFEntityHandle newEntityHandle{};

        HRESULT Get(XAsyncBlock* async) override
        {
            return PFAuthenticationGetEntityTokenGetResult(async, &newEntityHandle);
        }

        ~GetEntityTokenResult()
        {
            PFEntityCloseHandle(newEntityHandle);
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetEntityTokenResult>>(testContext);

    PFAuthenticationGetEntityTokenRequest request{};
    HRESULT hr = PFAuthenticationGetEntityTokenAsync(stateHandle, &request, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabAuthenticationGetEntityTokenAsync", hr);
        return;
    }
    async.release();
}

void ApiTests::TestGetQoSMeasurements(TestContext& testContext)
{
    struct QoSMeasurements : public XAsyncResult
    {
        PFQoSMeasurements* qosMeasurements;

        HRESULT Get(XAsyncBlock* async) override
        {
            return PFQoSGetMeasurementsGetResult(async, &resultHandle, &qosMeasurements);
        }
    };

    auto async = std::make_unique<XAsyncHelper<QoSMeasurements>>(testContext);

    HRESULT hr = PFQoSGetMeasurmentsAsync(entityHandle, 30, 250, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabAuthenticationGetEntityTokenAsync", hr);
        return;
    }
    async.release();
}

void ApiTests::AddTests()
{
    AddTest("TestApiStaticSizeResult", &ApiTests::TestApiStaticSizeResult);
    AddTest("TestApiSerializableResult", &ApiTests::TestApiSerializableResult);
    AddTest("TestApiResultHandle", &ApiTests::TestApiResultHandle);
    //AddTest("TestApiSessionTicket", &ApiTests::TestApiSessionTicket);
    AddTest("TestApiEntityToken", &ApiTests::TestApiEntityToken);
    AddTest("TestApiSecretKey", &ApiTests::TestApiSecretKey);
    //AddTest("TestApiNoAuth", &ApiTests::TestApiNoAuth);
    AddTest("TestGetEntityTokenWithAuthContext", &ApiTests::TestGetEntityTokenWithAuthContext);
    AddTest("TestGetEntityTokenWithSecretKey", &ApiTests::TestGetEntityTokenWithSecretKey);
    AddTest("TestGetQoSMeasurements", &ApiTests::TestGetQoSMeasurements);
}

void ApiTests::ClassSetUp()
{
    HRESULT hr = PFAdminInitialize(testTitleData.titleId.data(), testTitleData.developerSecretKey.data(), nullptr, &stateHandle);
    if (SUCCEEDED(hr))
    {
        PFAuthenticationLoginWithCustomIDRequest request{};
        request.customId = "CustomId";
        bool createAccount = true;
        request.createAccount = &createAccount;
        request.titleId = testTitleData.titleId.data();

        XAsyncBlock async{};
        hr = PFAuthenticationClientLoginWithCustomIDAsync(stateHandle, &request, &async);
        if (SUCCEEDED(hr))
        {
            // Synchronously what for login to complete
            hr = XAsyncGetStatus(&async, true);
            if (SUCCEEDED(hr))
            {
                PFAuthenticationClientLoginGetResult(&async, &entityHandle);
            }
        }
    }
}

void ApiTests::ClassTearDown()
{
    PFEntityCloseHandle(entityHandle);

    XAsyncBlock async{};
    HRESULT hr = PFCleanupAsync(stateHandle, &async);
    assert(SUCCEEDED(hr));

    hr = XAsyncGetStatus(&async, true);
    assert(SUCCEEDED(hr));

    UNREFERENCED_PARAMETER(hr);
}

void ApiTests::SetUp(TestContext& testContext)
{
    if (!entityHandle)
    {
        testContext.Skip("Skipping test because login failed");
    }
}

}
#include "TestAppPch.h"
#include "TestContext.h"
#include "ApiTests.h"
#include "XAsyncHelper.h"
#include <playfab/PlayFabClientAuthApi.h>
#include <playfab/PlayFabClientApi.h>
#include <playfab/PlayFabProfilesApi.h>
#include <playfab/PlayFabAdminApi.h>
#include <playfab/PlayFabAuthenticationAuthApi.h>

namespace PlayFabUnit
{


void ApiTests::TestApiStaticSizeResult(TestContext& testContext)
{
    struct GetTimeResult : public XAsyncResult
    {
        PlayFabClientGetTimeResult result{};

        HRESULT Get(XAsyncBlock* async) override
        {
            return PlayFabClientGetTimeGetResult(async, &result);
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetTimeResult>>(testContext);

    HRESULT hr = PlayFabClientGetTimeAsync(entityHandle, &async->asyncBlock);
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
        PlayFabClientCreateSharedGroupResult* result;

        HRESULT Get(XAsyncBlock* async) override
        {
            size_t requiredBufferSize;
            RETURN_IF_FAILED(PlayFabClientCreateSharedGroupGetResultSize(async, &requiredBufferSize));

            resultBuffer.resize(requiredBufferSize);
            return PlayFabClientCreateSharedGroupGetResult(async, resultBuffer.size(), resultBuffer.data(), &result, nullptr);
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

    PlayFabClientCreateSharedGroupRequest request{ groupId.data() };
    HRESULT hr = PlayFabClientCreateSharedGroupAsync(entityHandle, &request, &async->asyncBlock);
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
        PlayFabClientGetPlayerProfileResult* result{ nullptr };

        HRESULT Get(XAsyncBlock* async) override
        {
            return PlayFabClientGetPlayerProfileGetResult(async, &resultHandle, &result);
        }

        HRESULT Validate() override
        {
            // Any fields we can easily validate here?
            return S_OK;
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetPlayerProfileResult>>(testContext);

    PlayFabClientGetPlayerProfileRequest request{};
    HRESULT hr = PlayFabClientGetPlayerProfileAsync(entityHandle, &request, &async->asyncBlock);
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
        PlayFabProfilesGetEntityProfileResponse* result{ nullptr };

        HRESULT Get(XAsyncBlock* async) override
        {
            return PlayFabProfilesGetProfileGetResult(async, &resultHandle, &result);
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetProfilesResult>>(testContext);

    PlayFabProfilesGetEntityProfileRequest request{};
    HRESULT hr = PlayFabProfilesGetProfileAsync(entityHandle, &request, &async->asyncBlock);
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
        PlayFabAdminGetTitleDataResult* result;

        HRESULT Get(XAsyncBlock* async) override
        {
            return PlayFabAdminGetTitleDataGetResult(async, &resultHandle, &result);
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetTitleDataResult>>(testContext);

    PlayFabAdminGetTitleDataRequest request{};
    HRESULT hr = PlayFabAdminGetTitleDataAsync(stateHandle, &request, &async->asyncBlock);
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
        PlayFabClientGetTitlePublicKeyResult* result;

        HRESULT Get(XAsyncBlock* async) override
        {
            size_t requiredBufferSize;
            RETURN_IF_FAILED(PlayFabClientGetTitlePublicKeyGetResultSize(async, &requiredBufferSize));

            resultBuffer.resize(requiredBufferSize);
            return PlayFabClientGetTitlePublicKeyGetResult(async, resultBuffer.size(), resultBuffer.data(), &result, nullptr);
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetTitlePublicKeyResult>>(testContext);

    PlayFabClientGetTitlePublicKeyRequest request{};
    request.titleId = testTitleData.titleId.data();
    request.titleSharedSecret = testTitleData.developerSecretKey.data();
    HRESULT hr = PlayFabClientGetTitlePublicKeyAsync(stateHandle, &request, &async->asyncBlock);
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

    PlayFabAuthenticationGetEntityTokenRequest request{};
    HRESULT hr = PlayFabEntityGetEntityTokenAsync(entityHandle, &request, &async->asyncBlock);
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
        PlayFabEntityHandle newEntityHandle{};

        HRESULT Get(XAsyncBlock* async) override
        {
            return PlayFabGetAuthResult(async, &newEntityHandle);
        }

        ~GetEntityTokenResult()
        {
            PlayFabEntityCloseHandle(newEntityHandle);
        }
    };

    auto async = std::make_unique<XAsyncHelper<GetEntityTokenResult>>(testContext);

    PlayFabAuthenticationGetEntityTokenRequest request{};
    HRESULT hr = PlayFabAuthenticationGetEntityTokenAsync(stateHandle, &request, &async->asyncBlock);
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
        PlayFabQoSMeasurements* qosMeasurements;

        HRESULT Get(XAsyncBlock* async) override
        {
            return PlayFabQoSGetMeasurementsGetResult(async, &resultHandle, &qosMeasurements);
        }
    };

    auto async = std::make_unique<XAsyncHelper<QoSMeasurements>>(testContext);

    HRESULT hr = PlayFabQoSGetMeasurmentsAsync(entityHandle, 30, 250, &async->asyncBlock);
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
    HRESULT hr = PlayFabAdminInitialize(testTitleData.titleId.data(), testTitleData.developerSecretKey.data(), nullptr, &stateHandle);
    if (SUCCEEDED(hr))
    {
        PlayFabClientLoginWithCustomIDRequest request{};
        request.customId = "CustomId";
        bool createAccount = true;
        request.createAccount = &createAccount;
        request.titleId = testTitleData.titleId.data();

        XAsyncBlock async{};
        hr = PlayFabClientLoginWithCustomIDAsync(stateHandle, &request, &async);
        if (SUCCEEDED(hr))
        {
            // Synchronously what for login to complete
            hr = XAsyncGetStatus(&async, true);
            if (SUCCEEDED(hr))
            {
                PlayFabGetAuthResult(&async, &entityHandle);
            }
        }
    }
}

void ApiTests::ClassTearDown()
{
    PlayFabEntityCloseHandle(entityHandle);

    XAsyncBlock async{};
    HRESULT hr = PlayFabCleanupAsync(stateHandle, &async);
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
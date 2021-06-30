#include "TestAppPch.h"
#include "TestContext.h"
#include "EntityTests.h"
#include "XAsyncHelper.h"
#include <playfab/PlayFabClientAuthApi.h>

namespace PlayFabUnit
{

struct AuthResult : public XAsyncResult
{
    ~AuthResult()
    {
        PlayFabEntityCloseHandle(entityHandle);
    }

    HRESULT Get(XAsyncBlock* async) override
    {
        RETURN_IF_FAILED(PlayFabGetAuthResult(async, &entityHandle));

        const char* playFabId;
        RETURN_IF_FAILED(PlayFabEntityGetPlayFabId(entityHandle, &playFabId));

        const char* entityId;
        RETURN_IF_FAILED(PlayFabEntityGetEntityId(entityHandle, &entityId));

        const char* entityType;
        RETURN_IF_FAILED(PlayFabEntityGetEntityType(entityHandle, &entityType));

        const PlayFabEntityToken* entityToken;
        RETURN_IF_FAILED(PlayFabEntityGetCachedEntityToken(entityHandle, &entityToken));

        PlayFabGetPlayerCombinedInfoResultPayload const* playerCombinedInfo;
        RETURN_IF_FAILED(PlayFabEntityGetPlayerCombinedInfo(entityHandle, &playerCombinedInfo));

        time_t const* lastLoginTime;
        RETURN_IF_FAILED(PlayFabEntityGetLastLoginTime(entityHandle, &lastLoginTime));

        PlayFabUserSettings const* userSettings;
        RETURN_IF_FAILED(PlayFabEntityGetUserSettings(entityHandle, &userSettings));

        PlayFabTreatmentAssignment const* treatmentAssignment;
        RETURN_IF_FAILED(PlayFabEntityGetTreatmentAssignment(entityHandle, &treatmentAssignment));

        return S_OK;
    }

    PlayFabEntityHandle entityHandle{ nullptr };
};

void EntityTests::TestClientLogin(TestContext& testContext)
{
    auto async = std::make_unique<XAsyncHelper<AuthResult>>(testContext);

    PlayFabClientLoginWithCustomIDRequest request{};
    request.customId = "CustomId";
    bool createAccount = true;
    request.createAccount = &createAccount;
    request.titleId = testTitleData.titleId.data();
    PlayFabClientGetPlayerCombinedInfoRequestParams combinedInfoRequestParams{};
    combinedInfoRequestParams.getPlayerProfile = true;
    combinedInfoRequestParams.getTitleData = true;
    combinedInfoRequestParams.getUserAccountInfo = true;
    combinedInfoRequestParams.getUserData = true;
    request.infoRequestParameters = &combinedInfoRequestParams;

    HRESULT hr = PlayFabClientLoginWithCustomIDAsync(stateHandle, &request, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabClientLoginWithCustomIDAsync", hr);
    }
    async.release();
}

void EntityTests::TestManualTokenRefresh(TestContext& testContext)
{
    PlayFabEntityHandle entityHandle{ nullptr };

    {
        XAsyncBlock async{};

        PlayFabClientLoginWithCustomIDRequest request{};
        request.customId = "CustomId";
        bool createAccount = true;
        request.createAccount = &createAccount;
        request.titleId = testTitleData.titleId.data();

        HRESULT hr = PlayFabClientLoginWithCustomIDAsync(stateHandle, &request, &async);
        if (FAILED(hr))
        {
            testContext.Fail("PlayFabClientLoginWithCustomIDAsync", hr);
            return;
        }

        XAsyncGetStatus(&async, true);
        hr = PlayFabGetAuthResult(&async, &entityHandle);

        if (FAILED(hr) || !entityHandle)
        {
            testContext.Fail("PlayFabGetAuthResult", hr);
            return;
        }

        const PlayFabEntityToken* entityToken;
        PlayFabEntityGetCachedEntityToken(entityHandle, &entityToken);
    }

    {
        XAsyncBlock async{};

        PlayFabAuthenticationGetEntityTokenRequest request{};
        HRESULT hr = PlayFabEntityGetEntityTokenAsync(entityHandle, &request, &async);
        if (FAILED(hr))
        {
            testContext.Fail("PlayFabEntityGetEntityTokenAsync", hr);
            return;
        }

        hr = XAsyncGetStatus(&async, true);
        if (FAILED(hr))
        {
            testContext.Fail("PlayFabEntityGetEntityTokenAsync", hr);
            return;
        }

        const PlayFabEntityToken* entityToken;
        PlayFabEntityGetCachedEntityToken(entityHandle, &entityToken);
    }

    testContext.Pass();
}

void EntityTests::AddTests()
{
    AddTest("TestClientLogin", &EntityTests::TestClientLogin);
    AddTest("TestManualTokenRefresh", &EntityTests::TestManualTokenRefresh);
}

void EntityTests::ClassSetUp()
{
    HRESULT hr = PlayFabInitialize(testTitleData.titleId.data(), nullptr, &stateHandle);
    UNREFERENCED_PARAMETER(hr);
}

void EntityTests::ClassTearDown()
{
    XAsyncBlock async{};
    HRESULT hr = PlayFabCleanupAsync(stateHandle, &async);
    assert(SUCCEEDED(hr));

    hr = XAsyncGetStatus(&async, true);
    assert(SUCCEEDED(hr));

    UNREFERENCED_PARAMETER(hr);
}

void EntityTests::SetUp(TestContext& testContext)
{
    if (!stateHandle)
    {
        testContext.Skip("Skipping test because stateHandle is null");
    }
}

}
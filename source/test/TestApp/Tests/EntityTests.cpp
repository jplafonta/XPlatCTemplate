#include "TestAppPch.h"
#include "TestContext.h"
#include "EntityTests.h"
#include "XAsyncHelper.h"
#include <playfab/PFAuthentication.h>

namespace PlayFabUnit
{

struct AuthResult : public XAsyncResult
{
    ~AuthResult()
    {
        PFEntityCloseHandle(entityHandle);
    }

    HRESULT Get(XAsyncBlock* async) override
    {
        RETURN_IF_FAILED(PFGetAuthResult(async, &entityHandle));

        const char* playFabId;
        RETURN_IF_FAILED(PFEntityGetPlayFabId(entityHandle, &playFabId));

        const char* entityId;
        RETURN_IF_FAILED(PFEntityGetEntityId(entityHandle, &entityId));

        const char* entityType;
        RETURN_IF_FAILED(PFEntityGetEntityType(entityHandle, &entityType));

        const PFEntityToken* entityToken;
        RETURN_IF_FAILED(PFEntityGetCachedEntityToken(entityHandle, &entityToken));

        PFGetPlayerCombinedInfoResultPayload const* playerCombinedInfo;
        RETURN_IF_FAILED(PFEntityGetPlayerCombinedInfo(entityHandle, &playerCombinedInfo));

        time_t const* lastLoginTime;
        RETURN_IF_FAILED(PFEntityGetLastLoginTime(entityHandle, &lastLoginTime));

        PFAuthenticationUserSettings const* userSettings;
        RETURN_IF_FAILED(PFEntityGetUserSettings(entityHandle, &userSettings));

        PFTreatmentAssignment const* treatmentAssignment;
        RETURN_IF_FAILED(PFEntityGetTreatmentAssignment(entityHandle, &treatmentAssignment));

        return S_OK;
    }

    PFEntityHandle entityHandle{ nullptr };
};

void EntityTests::TestClientLogin(TestContext& testContext)
{
    auto async = std::make_unique<XAsyncHelper<AuthResult>>(testContext);

    PFAuthenticationLoginWithCustomIDRequest request{};
    request.customId = "CustomId";
    bool createAccount = true;
    request.createAccount = &createAccount;
    request.titleId = testTitleData.titleId.data();
    PFGetPlayerCombinedInfoRequestParams combinedInfoRequestParams{};
    combinedInfoRequestParams.getPlayerProfile = true;
    combinedInfoRequestParams.getTitleData = true;
    combinedInfoRequestParams.getUserAccountInfo = true;
    combinedInfoRequestParams.getUserData = true;
    request.infoRequestParameters = &combinedInfoRequestParams;

    HRESULT hr = PFAuthenticationClientLoginWithCustomIDAsync(stateHandle, &request, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabClientLoginWithCustomIDAsync", hr);
    }
    async.release();
}

void EntityTests::TestManualTokenRefresh(TestContext& testContext)
{
    AuthResult authResult;

    {
        XAsyncBlock async{};

        PFAuthenticationLoginWithCustomIDRequest request{};
        request.customId = "CustomId";
        bool createAccount = true;
        request.createAccount = &createAccount;
        request.titleId = testTitleData.titleId.data();

        HRESULT hr = PFAuthenticationClientLoginWithCustomIDAsync(stateHandle, &request, &async);
        if (FAILED(hr))
        {
            testContext.Fail("PlayFabClientLoginWithCustomIDAsync", hr);
            return;
        }

        XAsyncGetStatus(&async, true);
        hr = PFGetAuthResult(&async, &authResult.entityHandle);

        if (FAILED(hr) || !&authResult.entityHandle)
        {
            testContext.Fail("PlayFabGetAuthResult", hr);
            return;
        }

        const PFEntityToken* entityToken;
        PFEntityGetCachedEntityToken(authResult.entityHandle, &entityToken);
    }

    {
        XAsyncBlock async{};

        PFAuthenticationGetEntityTokenRequest request{};
        HRESULT hr = PFEntityGetEntityTokenAsync(authResult.entityHandle, &request, &async);
        if (FAILED(hr))
        {
            testContext.Fail("PFEntityGetEntityTokenAsync", hr);
            return;
        }

        hr = XAsyncGetStatus(&async, true);
        if (FAILED(hr))
        {
            testContext.Fail("PFEntityGetEntityTokenAsync", hr);
            return;
        }

        const PFEntityToken* entityToken;
        PFEntityGetCachedEntityToken(authResult.entityHandle, &entityToken);
    }

    testContext.Pass();
}

#if HC_PLATFORM == HC_PLATFORM_GDK

// RAII wrapper of XUserHandle
struct XUserHolder
{
    XUserHolder() = default;
    XUserHolder(const XUserHolder&) = delete;
    ~XUserHolder()
    {
        if (this->handle)
        {
            XUserCloseHandle(this->handle);
        }
    }

    XUserHandle handle{ nullptr };
};

HRESULT AddXUser(XUserHolder& user)
{
    // Synchronously add Platform user
    XAsyncBlock async{};
    RETURN_IF_FAILED(XUserAddAsync(XUserAddOptions::AddDefaultUserAllowingUI, &async));
    RETURN_IF_FAILED(XAsyncGetStatus(&async, true));
    RETURN_IF_FAILED(XUserAddResult(&async, &user.handle));

    // Sanity check
    XUserLocalId localId{};
    RETURN_IF_FAILED(XUserGetLocalId(user.handle, &localId));

    return S_OK;
}

void EntityTests::TestLoginWithXUser(TestContext& testContext)
{
    XUserHolder xUser;
    AuthResult playFabAuthResult;

    HRESULT hr = AddXUser(xUser);
    if (FAILED(hr))
    {
        testContext.Fail();
    }
    else
    {
        auto async = std::make_unique<XAsyncHelper<AuthResult>>(testContext);

        PFAuthenticationLoginWithXUserRequest request{};
        bool createAccount = true;
        request.createAccount = &createAccount;
        request.titleId = testTitleData.titleId.data();
        request.userHandle = xUser.handle;

        hr = PFAuthenticationLoginWithXUserAsync(stateHandle, &request, &async->asyncBlock);
        if (FAILED(hr))
        {
            testContext.Fail("PFClientLoginWithXUserAsync", hr);
        }
        async.release();
    }
}
#endif

void EntityTests::AddTests()
{
    AddTest("TestClientLogin", &EntityTests::TestClientLogin);
    AddTest("TestManualTokenRefresh", &EntityTests::TestManualTokenRefresh);
#if HC_PLATFORM == HC_PLATFORM_GDK
    AddTest("TestLoginWithXUser", &EntityTests::TestLoginWithXUser);
#endif
}

void EntityTests::ClassSetUp()
{
    HRESULT hr = PFInitialize(testTitleData.titleId.data(), nullptr, &stateHandle);
    UNREFERENCED_PARAMETER(hr);
}

void EntityTests::ClassTearDown()
{
    XAsyncBlock async{};
    HRESULT hr = PFCleanupAsync(stateHandle, &async);
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
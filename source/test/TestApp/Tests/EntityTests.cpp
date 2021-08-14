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
        PFTitlePlayerCloseHandle(titlePlayerHandle);
        PFEntityCloseHandle(entityHandle);
    }

    HRESULT Get(XAsyncBlock* async) override
    {
        RETURN_IF_FAILED(PFAuthenticationClientLoginGetResult(async, &titlePlayerHandle));
        RETURN_IF_FAILED(PFTitlePlayerGetEntityHandle(titlePlayerHandle, &entityHandle));

        return S_OK;
    }

    HRESULT Validate() override
    {
        const char* playFabId;
        RETURN_IF_FAILED(PFTitlePlayerGetPlayFabId(titlePlayerHandle, &playFabId));

        const char* entityIdFromTitlePlayer;
        RETURN_IF_FAILED(PFTitlePlayerGetEntityId(titlePlayerHandle, &entityIdFromTitlePlayer));

        const char* entityIdFromEntity;
        RETURN_IF_FAILED(PFEntityGetEntityId(entityHandle, &entityIdFromEntity));

        if (std::strcmp(entityIdFromEntity, entityIdFromTitlePlayer))
        {
            return E_FAIL;
        }

        const char* entityType;
        RETURN_IF_FAILED(PFEntityGetEntityType(entityHandle, &entityType));

        const PFEntityToken* entityTokenFromTitlePlayer;
        RETURN_IF_FAILED(PFTitlePlayerGetCachedEntityToken(titlePlayerHandle, &entityTokenFromTitlePlayer));

        const PFEntityToken* entityTokenFromEntity;
        RETURN_IF_FAILED(PFEntityGetCachedEntityToken(entityHandle, &entityTokenFromEntity));

        if (std::strcmp(entityTokenFromEntity->token, entityTokenFromTitlePlayer->token))
        {
            return E_FAIL;
        }

        size_t sessionTicketSize;
        RETURN_IF_FAILED(PFTitlePlayerGetCachedSessionTicketSize(titlePlayerHandle, &sessionTicketSize));

        std::vector<char> sessionTicket(sessionTicketSize);
        RETURN_IF_FAILED(PFTitlePlayerGetCachedSessionTicket(titlePlayerHandle, sessionTicketSize, sessionTicket.data(), nullptr));

        PFGetPlayerCombinedInfoResultPayload const* playerCombinedInfo;
        RETURN_IF_FAILED(PFTitlePlayerGetPlayerCombinedInfo(titlePlayerHandle, &playerCombinedInfo));

        time_t const* lastLoginTime;
        RETURN_IF_FAILED(PFTitlePlayerGetLastLoginTime(titlePlayerHandle, &lastLoginTime));

        PFAuthenticationUserSettings const* userSettings;
        RETURN_IF_FAILED(PFTitlePlayerGetUserSettings(titlePlayerHandle, &userSettings));

        PFTreatmentAssignment const* treatmentAssignment;
        RETURN_IF_FAILED(PFTitlePlayerGetTreatmentAssignment(titlePlayerHandle, &treatmentAssignment));

        return S_OK;
    }

    PFTitlePlayerHandle titlePlayerHandle{ nullptr };
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
    struct State
    {
        AuthResult authResult;
        TestContext& testContext;
    };
    std::unique_ptr<State> testState{ new State{ AuthResult{}, testContext } };

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
        hr = PFAuthenticationClientLoginGetResult(&async, &testState->authResult.titlePlayerHandle);

        if (FAILED(hr) || !testState->authResult.titlePlayerHandle)
        {
            testContext.Fail("PlayFabGetAuthResult", hr);
            return;
        }

        PFTitlePlayerGetEntityHandle(testState->authResult.titlePlayerHandle, &testState->authResult.entityHandle);

        const PFEntityToken* entityToken;
        PFEntityGetCachedEntityToken(testState->authResult.entityHandle, &entityToken);
    }

    PFRegistrationToken token{};
    PFEntityRegisterTokenRefreshedCallback(testState->authResult.entityHandle, nullptr, 
        [](_In_ const PFEntityToken* newToken, _In_opt_ void* context)
        {
            std::unique_ptr<State> testState{ static_cast<State*>(context) };

            const PFEntityToken* entityToken;
            PFEntityGetCachedEntityToken(testState->authResult.entityHandle, &entityToken);

            if (std::strcmp(newToken->token, entityToken->token))
            {
                testState->testContext.Fail();
            }
            else
            {
                testState->testContext.Pass();
            }

        }, testState.get(), &token);

    {
        XAsyncBlock async{};

        PFAuthenticationGetEntityTokenRequest request{};
        HRESULT hr = PFEntityGetEntityTokenAsync(testState->authResult.entityHandle, &request, &async);
        if (FAILED(hr))
        {
            testContext.Fail("PFEntityGetEntityTokenAsync", hr);
            return;
        }

        hr = XAsyncGetStatus(&async, true);
        if (SUCCEEDED(hr))
        {
            PFEntityHandle newHandle{};
            PFEntityGetEntityTokenGetResult(&async, &newHandle);
            PFEntityCloseHandle(newHandle);
        }

    }

    testState.release();
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
    HRESULT hr = PFUninitializeAsync(stateHandle, &async);
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
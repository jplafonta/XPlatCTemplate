#include "TestAppPch.h"
#include "TestContext.h"
#include "ApiTests.h"
#include <playfab/PlayFabClientAuthApi.h>
#include <playfab/PlayFabClientApi.h>
#include <playfab/PlayFabProfilesApi.h>
#include <playfab/PlayFabAdminApi.h>
#include <playfab/PlayFabAuthenticationAuthApi.h>

namespace PlayFabUnit
{

// Helper class to hold, retrieve, and validate the result of a completed XAsync operation. Intentionally
// storing the and validating the result in a different context so that we can make sure the result
// lifetime is maintained correctly
struct XAsyncResult
{
    std::vector<char> resultBuffer{};
    PlayFabResultHandle resultHandle{ nullptr };

    virtual HRESULT Get(XAsyncBlock* async)
    {
        return XAsyncGetStatus(async, false);
    };
    virtual HRESULT Validate()
    {
        return S_OK;
    }
    virtual ~XAsyncResult()
    {
        PlayFabResultCloseHandle(resultHandle);
    }
};

// Helper class for calling XAsync APIs and validating the result
template<typename ResultT = XAsyncResult>
struct XAsyncHelper
{
public:
    static_assert(std::is_base_of_v<XAsyncResult, ResultT>, "ResultT needs to be an XAsyncResult");

    XAsyncHelper(TestContext& tc) :
        asyncBlock{},
        testContext{ tc },
        asyncResult{ std::make_shared<ResultT>() }
    {
        asyncBlock.context = this;
        asyncBlock.callback = &XAsyncCallback;
    }

    ~XAsyncHelper()
    {
        asyncResult.reset();
    }

    XAsyncBlock asyncBlock;

private:
    static void CALLBACK XAsyncCallback(XAsyncBlock* async)
    {
        std::unique_ptr<XAsyncHelper> context{ static_cast<XAsyncHelper*>(async->context) };
        HRESULT hr = context->asyncResult->Get(async);
        if (FAILED(hr))
        {
            context->testContext.Fail("XAsyncResult::Get", hr);
        }
        else
        {
            // validate asynchronously to ensure result lifetime is maintained
            std::thread validateResultThread([context{ std::move(context) }]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });

                HRESULT hr = context->asyncResult->Validate();
                if (FAILED(hr))
                {
                    context->testContext.Fail("XAsyncResult::Validate", hr);
                }
                else
                {
                    context->testContext.Pass();
                }
            });

            validateResultThread.detach();
        }
    }

    TestContext& testContext;
    std::shared_ptr<ResultT> asyncResult;
};

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
    static char newsTitle[] = "News Title";
    static char newsBody[] = "News!";

    struct AddNewsResult : public XAsyncResult
    {
        PlayFabAdminAddNewsResult* result;

        HRESULT Get(XAsyncBlock* async) override
        {
            size_t requiredBufferSize;
            RETURN_IF_FAILED(PlayFabAdminAddNewsGetResultSize(async, &requiredBufferSize));

            resultBuffer.resize(requiredBufferSize);
            return PlayFabAdminAddNewsGetResult(async, resultBuffer.size(), resultBuffer.data(), &result, nullptr);
        }
    };

    auto async = std::make_unique<XAsyncHelper<AddNewsResult>>(testContext);

    PlayFabAdminAddNewsRequest request{};
    request.title = newsTitle;
    request.body = newsBody;

    HRESULT hr = PlayFabAdminAddNewsAsync(stateHandle, &request, &async->asyncBlock);
    if (FAILED(hr))
    {
        testContext.Fail("PlayFabAdminAddNewsAsync", hr);
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
}

void ApiTests::ClassSetUp()
{
    HRESULT hr = PlayFabInitialize(testTitleData.titleId.data(), testTitleData.developerSecretKey.data(), &stateHandle);
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
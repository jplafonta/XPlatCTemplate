#include "TestAppPch.h"
#include "TestContext.h"
#include "EventManagerTests.h"
#include <playfab/PlayFabClientAuthApi.h>

namespace PlayFabUnit
{

class Event
{
public:
    Event(PlayFabEventManagerEventType type, const char* name)
    {
        HRESULT hr = PlayFabEventManagerEventCreate(type, name, nullptr, nullptr, nullptr, &handle);
        assert(SUCCEEDED(hr));
        UNREFERENCED_PARAMETER(hr);
    }

    Event(const Event& other)
    {
        HRESULT hr = PlayFabEventManagerEventDuplicateHandle(other.handle, &handle);
        assert(SUCCEEDED(hr));
        UNREFERENCED_PARAMETER(hr);
    }

    ~Event()
    {
        PlayFabEventManagerEventCloseHandle(handle);
    }

    PlayFabEventManagerEventHandle handle;
};

struct WriteEventContext
{
    TestContext& testContext;
    Event event;
};

void CALLBACK WriteEventComplete(void* context, HRESULT result, const char* /*assignedEventId*/)
{
    auto writeEventContext = std::unique_ptr<WriteEventContext>{ static_cast<WriteEventContext*>(context) };

    std::stringstream message;
    if (FAILED(result))
    {
        message << "PlayFabEventManagerWriteEventAsync async result failed hr=" << std::hex << result;
        writeEventContext->testContext.Fail(message.str());
    }
}

void EventManagerTests::TestWriteEvent(TestContext& testContext)
{
    Event event{ PlayFabEventManagerEventType::Default, "TestEvent" };
    PlayFabEventManagerEventSetStringProperty(event.handle, "Key", "Value");

    auto writeContext = std::unique_ptr<WriteEventContext>(new WriteEventContext{ testContext, event });

    HRESULT hr = PlayFabEventManagerWriteEventAsync(entityHandle, event.handle, writeContext.get(), &WriteEventComplete);
    if (SUCCEEDED(hr))
    {
        writeContext.release();
    }
    else
    {
        testContext.Fail("PlayFabEventManagerWriteEventAsync", hr);
    }
}

void EventManagerTests::AddTests()
{
    AddTest("TestWriteEvent", &EventManagerTests::TestWriteEvent);
}

void EventManagerTests::ClassSetUp()
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

void EventManagerTests::ClassTearDown()
{
    PlayFabEntityCloseHandle(entityHandle);

    XAsyncBlock async{};
    HRESULT hr = PlayFabCleanupAsync(stateHandle, &async);
    assert(SUCCEEDED(hr));

    hr = XAsyncGetStatus(&async, true);
    assert(SUCCEEDED(hr));

    UNREFERENCED_PARAMETER(hr);
}

void EventManagerTests::SetUp(TestContext& testContext)
{
    if (!entityHandle)
    {
        testContext.Skip("Skipping test because login failed");
    }
}


}
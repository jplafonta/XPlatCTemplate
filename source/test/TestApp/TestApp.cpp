// Copyright (C) Microsoft Corporation. All rights reserved.

#include "TestAppPch.h"

#include <cstdarg>
#include <functional>
#include <thread>

#include "TestApp.h"
#include "TestRunner.h"
#include "TestReport.h"
#include "TestUtils.h"

#if !defined(DISABLE_PLAYFABCLIENT_API)
#include <playfab/PlayFabSettings.h>
#include "PlatformLoginTest.h"
#endif

#include "JsonParsingTests.h"
#include "ModelTests.h"

using namespace PlayFab;

namespace PlayFabUnit
{
#if !defined(DISABLE_PLAYFABCLIENT_API)
    // Time out if waiting for the final cloudscript submission longer than this
    constexpr int CLOUDSCRIPT_TIMEOUT_MS = 30000;
#endif // !defined(DISABLE_PLAYFABCLIENT_API)

    void TestApp::Log(const char* format, ...)
    {
        static char message[4096];

        va_list args;
        va_start(args, format);
#if defined(PLAYFAB_PLATFORM_PLAYSTATION)
        vsnprintf_s(message, sizeof(message), format, args);
#elif defined(PLAYFAB_PLATFORM_IOS) || defined(PLAYFAB_PLATFORM_ANDROID) || defined(PLAYFAB_PLATFORM_LINUX) || defined(PLAYFAB_PLATFORM_SWITCH)
        vsnprintf(message, sizeof(message), format, args);
#else
        _vsnprintf_s(message, sizeof(message), format, args);
#endif // defined(PLAYFAB_PLATFORM_PLAYSTATION)
        va_end(args);

        // Output the message in a platform-dependent way.
        LogPut(message);
    }

    int TestApp::Main()
    {
        HRESULT hr = HCInitialize(nullptr);
        assert(SUCCEEDED(hr));
        HCSettingsSetTraceLevel(HCTraceLevel::Verbose);
        HCTraceSetTraceToDebugger(true);

        // Load the TestTitleData
        TestTitleData testTitleData;
        bool loadSuccessful = LoadTitleData(testTitleData);

        // If the title data fails to load, and you want to use custom hard-coded data, you can
        // comment out the return statement and fill out the TestTitleData fields manually.
        if (!loadSuccessful)
        {
            //printf("Failed to load testTitleData\n");
            //return 1;

            // TODO: POPULATE THIS SECTION WITH REAL INFORMATION (or set up a testTitleData file, and set your PF_TEST_TITLE_DATA_JSON to the path for that file)
            testTitleData.titleId = "E18D7"; // The titleId for your title, found in the "Settings" section of PlayFab Game Manager
            //testTitleData.userEmail = ""; // This is the email for a valid user (test tries to log into it with an invalid password, and verifies error result)
        }

        // Initialize the test runner/test data.
        TestRunner testRunner;

#if !defined(DISABLE_PLAYFABCLIENT_API)
        // Set this up for use when the tests finish
        this->clientApi = std::make_shared<PlayFab::PlayFabClientInstanceAPI>(PlayFab::PlayFabSettings::staticPlayer);

        PlatformLoginTest loginTest;
        loginTest.SetTitleInfo(testTitleData);
        testRunner.Add(loginTest);
#endif // !defined(DISABLE_PLAYFABCLIENT_API)

        JsonParsingTests jsonTests;
        testRunner.Add(jsonTests);

        ModelTests modelTests;
        testRunner.Add(modelTests);

        // Run the tests (blocks until all tests have finished).
        testRunner.Run();

        // Publish the test summary to STDOUT.
        Log("%s\n", testRunner.suiteTestSummary.c_str());

#if !defined(DISABLE_PLAYFABCLIENT_API)
        // Publish the test report via cloud script (and wait for it to finish).

        PlayFab::ClientModels::LoginWithCustomIDRequest request;
        request.customId = PlayFab::PlayFabSettings::buildIdentifier.data();
        bool createAccount = true;
        request.createAccount = &createAccount;
        request.titleId = testTitleData.titleId.data();
        this->clientApi->LoginWithCustomID(request,
            PlayFab::TaskQueue(),
            std::bind(&TestApp::OnPostReportLogin, this, std::placeholders::_1, testRunner.suiteTestReport),
            std::bind(&TestApp::OnPostReportError, this, std::placeholders::_1)
        );

        // Wait for CloudResponse
        {
            std::unique_lock<std::mutex> lk(cloudResponseMutex);
            cloudResponseConditionVar.wait_until(lk, std::chrono::system_clock::now() + std::chrono::milliseconds(CLOUDSCRIPT_TIMEOUT_MS), [this] {return !this->cloudResponse.empty(); });
            lk.unlock();
        }

        // Publish the cloud script response to STDOUT.
        Log("Cloud Response: %s\n", cloudResponse.c_str());
#endif //!defined(DISABLE_PLAYFABCLIENT_API)

        // Return 0 (success) if all tests passed. Otherwise, return 1 (error).
        return testRunner.AllTestsPassed()
#if !defined(DISABLE_PLAYFABCLIENT_API)
            && !cloudResponse.empty() ? 0 : 1
#endif // !defined(DISABLE_PLAYFABCLIENT_API)
            ;
    }

    bool TestApp::LoadTitleData(TestTitleData& titleData)
    {
        // Load JSON string in a platform-dependent way.
        std::shared_ptr<char*> titleJsonPtr;
        size_t size;
        const bool loadedSuccessfully = LoadTitleDataJson(titleJsonPtr, size);

        if (!loadedSuccessfully)
        {
            return false;
        }

        // Parse JSON string into output TestTitleData.
        JsonDocument titleDataJson;
        titleDataJson.Parse(*titleJsonPtr);

        if (!titleDataJson.HasParseError())
        {
            titleData.titleId = titleDataJson["titleId"].GetString();
            titleData.userEmail = titleDataJson["userEmail"].GetString();
            titleData.developerSecretKey = titleDataJson["developerSecretKey"].GetString();
        }

        return !titleDataJson.HasParseError();
    }

#if !defined(DISABLE_PLAYFABCLIENT_API)
    void TestApp::OnPostReportLogin(const PlayFab::ClientModels::LoginResult& result, TestReport& testReport)
    {
        // Prepare a JSON value as a param for the remote cloud script.
        JsonValue cloudReportJson{ rapidjson::kObjectType };
        cloudReportJson.AddMember("customId", JsonValue{ result.playFabId, s_jsonAllocator }, s_jsonAllocator);

        // The expected format is a list of TestSuiteReports, but this framework only submits one
        JsonValue arrayInit{ rapidjson::kArrayType };

        // Encode the test report as JSON.
        arrayInit.PushBack(testReport.internalReport.ToJson(), s_jsonAllocator);

        cloudReportJson.AddMember("testReport", arrayInit, s_jsonAllocator);

        JsonStringBuffer stringBuffer;
        JsonWriter writer{ stringBuffer };
        cloudReportJson.Accept(writer);

        // Save the test results via cloud script.
        PlayFab::ClientModels::ExecuteCloudScriptRequest request{};
        request.functionName = "SaveTestData";
        request.functionParameter.stringValue = stringBuffer.GetString();
        bool generatePlayStreamEvent = true;
        request.generatePlayStreamEvent = &generatePlayStreamEvent;
        this->clientApi->ExecuteCloudScript(request,
            PlayFab::TaskQueue(),
            std::bind(&TestApp::OnPostReportComplete, this, std::placeholders::_1),
            std::bind(&TestApp::OnPostReportError, this, std::placeholders::_1)
        );
    }

    void TestApp::OnPostReportComplete(const PlayFab::ClientModels::ExecuteCloudScriptResult& result)
    {
        std::stringstream ss;
        if (result.error)
        {
            ss << "Test report submitted via cloud script: " << PlayFab::PlayFabSettings::buildIdentifier.data() << ", " << PlayFab::PlayFabSettings::staticPlayer->playFabId.data();
            cloudResponse = ss.str();
        }
        else
        {
            ss << "Error executing test report cloud script:\n" << result.error->error << ": " << result.error->message;
            cloudResponse += ss.str();
        }
        cloudResponseConditionVar.notify_one();
    }

    void TestApp::OnPostReportError(const PlayFab::PlayFabError& error)
    {
        std::stringstream ss;
        ss << "Failed to report results via cloud script: " << error.GenerateErrorReport().data();
        cloudResponse = ss.str();
        cloudResponseConditionVar.notify_one();
    }
#endif // !defined(DISABLE_PLAYFABCLIENT_API)
}

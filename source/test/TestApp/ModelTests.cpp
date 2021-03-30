#include "TestAppPch.h"
#include "ModelTests.h"
#include <Client/ClientDataModels.h>
#include <playfab/PlayFabPlatformUtils.h>

using namespace PlayFab;
using namespace PlayFab::ClientModels;

constexpr char profileResultJson[] = R"(
{
   "PlayerProfile":{
      "PublisherId":"624BD2B0B8811A52",
      "TitleId":"E18D7",
      "PlayerId":"6842934CB46C0834"
   }
}
)";

namespace PlayFabUnit
{
    void ModelTests::BasicModelTest(TestContext& testContext)
    {
        JsonDocument inputJson;
        inputJson.Parse(profileResultJson);

        GetPlayerProfileResult profileResult;
        profileResult.FromJson(inputJson);

        auto outputJson = JsonUtils::ToJson<PlayFabClientGetPlayerProfileResult>(profileResult);

        if (inputJson != outputJson)
        {
            testContext.Fail("inputJson did not match outputJson");
        }

        testContext.Pass();
    }

    void ModelTests::TestMove(TestContext& testContext)
    {
        constexpr char jsonArray[] = "[1, 2, 3, 4, 5]";

        JsonDocument inputJson;
        inputJson.Parse(jsonArray);

        PointerArray<int, int> pointerArray;
        pointerArray.FromJson(inputJson);

        auto movedArray{ std::move(pointerArray) };
        auto outputJson = movedArray.ToJson();

        if (inputJson != outputJson)
        {
            testContext.Fail("inputJson did not match outputJson");
        }

        testContext.Pass();
    }

    void ModelTests::TestTimeDictionarySpecialization(TestContext& testContext)
    {
        const std::array<time_t, 3> times{ time(0), time(0), time(0) };

        JsonDocument inputJson{ rapidjson::kObjectType };
        auto& a{ inputJson.GetAllocator() };
        for (auto i = 0; i < times.size(); ++i)
        {
            std::stringstream ss;
            ss << "DateTime" << i << "_" << times[i];
            inputJson.AddMember(JsonValue{ ss.str().data(), a }, JsonValue{ PlayFab::TimeTToIso8601String(times[i]).data(), a }, a);
        }

        AssociativeArray<PlayFabDateTimeDictionaryEntry, void> dictionary;
        dictionary.FromJson(inputJson);

        if (times.size() != inputJson.MemberCount() || times.size() != dictionary.Size())
        {
            testContext.Fail("dictionary size didnt match json object size");
        }
        for (auto i = 0; i < times.size(); ++i)
        {
            if (times[i] != dictionary.Data()[i].value)
            {
                testContext.Fail("time mismatch");
            }
        }

        auto outputJson = dictionary.ToJson();
        if (inputJson != outputJson)
        {
            testContext.Fail("inputJson did not match outputJson");
        }

        testContext.Pass();
    }

    void ModelTests::AddTests()
    {
        AddTest("BasicModelTest", &ModelTests::BasicModelTest);
        AddTest("TestMove", &ModelTests::TestMove);
        AddTest("TestTimeDictionarySpecialization", &ModelTests::TestTimeDictionarySpecialization);
    }
}
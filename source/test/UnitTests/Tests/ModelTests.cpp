#include "pch.h"
#include "Shared/SharedDataModels.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PlayFab
{

namespace UnitTests
{

constexpr char profileResultJson[] = R"(
{
   "PlayerProfile":{
      "PublisherId":"624BD2B0B8811A52",
      "TitleId":"E18D7",
      "PlayerId":"6842934CB46C0834"
   }
}
)";

TEST_CLASS(ModelTests)
{
public:
    TEST_METHOD(MoveModel)
    {
        constexpr char jsonArray[] = "[1, 2, 3, 4, 5]";

        JsonDocument inputJson;
        inputJson.Parse(jsonArray);

        PointerArrayModel<int, int> pointerArray;
        pointerArray.FromJson(inputJson);

        auto movedArray{ std::move(pointerArray) };
        auto outputJson = movedArray.ToJson();

        Assert::IsTrue(inputJson == outputJson);
    }

    TEST_METHOD(TimeDictionarySpecialization)
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

        AssociativeArrayModel<PFDateTimeDictionaryEntry, void> dictionary;
        dictionary.FromJson(inputJson);

        Assert::IsTrue(times.size() == inputJson.MemberCount());
        Assert::IsTrue(times.size() == dictionary.Size());
        for (auto i = 0; i < times.size(); ++i)
        {
            Assert::IsTrue(times[i] == dictionary.Data()[i].value);
        }

        auto outputJson = dictionary.ToJson();
        Assert::IsTrue(inputJson == outputJson);
    }
};

} // UnitTests
} // PlayFab
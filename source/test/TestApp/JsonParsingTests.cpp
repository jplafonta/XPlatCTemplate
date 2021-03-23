#include "TestAppPch.h"
#include "JsonParsingTests.h"
#include <playfab/PlayFabClientDataModels.h>

using namespace PlayFab;

enum class PlayFabEnum 
{
    Value1,
    Value2
};

template<> struct EnumRange<PlayFabEnum> { static constexpr PlayFabEnum maxValue = PlayFabEnum::Value2; };

constexpr char jsonString[] = R"(
{
    "EnumValue":"Value2",
    "ArrayValue":[0,1,2,3,4,5],
    "SubObjectValue": 
    {
        "CountryCode":"IN"
    }
}
)";

namespace PlayFabUnit
{
    void JsonParsingTests::BasicJsonParsing(TestContext& testContext)
    {
        struct SubObjectModel : public PlayFabBaseModel
        {
            PlayFabCountryCode CountryCode;

            void FromJson(const JsonValue& input)
            {
                JsonUtils::ObjectGetMember(input, "CountryCode", CountryCode);
            }

            JsonValue ToJson() const 
            {
                JsonValue output{ rapidjson::kObjectType };
                JsonUtils::ObjectAddMember(output, "CountryCode", CountryCode);
                return output;
            }
        };

        struct ObjectModel : public PlayFabBaseModel
        {
            PlayFabEnum EnumValue;
            List<int> ArrayValue;
            SubObjectModel SubObjectValue;

            void FromJson(const JsonValue& input)
            {
                JsonUtils::ObjectGetMember(input, "EnumValue", EnumValue);
                JsonUtils::ObjectGetMember(input, "ArrayValue", ArrayValue);
                JsonUtils::ObjectGetMember(input, "SubObjectValue", SubObjectValue);
            }

            JsonValue ToJson() const
            {
                JsonValue output{ rapidjson::kObjectType };
                JsonUtils::ObjectAddMember(output, "EnumValue", EnumValue);
                JsonUtils::ObjectAddMember(output, "ArrayValue", ArrayValue);
                JsonUtils::ObjectAddMember(output, "SubObjectValue", SubObjectValue);
                return output;
            }
        };

        JsonDocument inputJson;
        inputJson.Parse(jsonString);

        ObjectModel model;
        model.FromJson(inputJson);

        JsonValue outputJson{ model.ToJson() };
        if (inputJson == outputJson)
        {
            testContext.Pass();
        }
        else
        {
            testContext.Fail("inputJson did not match outputJson");
        }
    }

    void JsonParsingTests::AddTests()
    {
        AddTest("BasicJsonParsing", &JsonParsingTests::BasicJsonParsing);
    }
}
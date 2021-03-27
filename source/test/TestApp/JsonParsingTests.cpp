#include "TestAppPch.h"
#include "JsonParsingTests.h"
#include <Client/ClientDataModels.h>

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
        struct SubObjectModel : public BaseModel
        {
            PlayFabClientCountryCode CountryCode;

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

        struct ObjectModel : public BaseModel
        {
            PlayFabEnum EnumValue;
            PointerArray<int, int> ArrayValue;
            SubObjectModel SubObjectValue;

            void FromJson(const JsonValue& input)
            {
                JsonUtils::ObjectGetMember(input, "EnumValue", EnumValue);
                uint32_t arraySize;
                int** arrayPtr;
                JsonUtils::ObjectGetMember(input, "ArrayValue", ArrayValue, arrayPtr, arraySize);
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
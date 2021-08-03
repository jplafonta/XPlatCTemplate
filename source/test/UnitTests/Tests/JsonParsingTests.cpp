#include "pch.h"
#include "EnumTraits.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PlayFab
{

enum class PFEnum
{
    Value1,
    Value2
};

template<> struct EnumRange<PFEnum> { static constexpr PFEnum maxValue = PFEnum::Value2; };
template<> struct EnumRange<PFCountryCode> { static constexpr PFCountryCode maxValue = PFCountryCode::ZW; };

namespace UnitTests
{

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

TEST_CLASS(JsonParsingTests)
{
public:
    TEST_METHOD(BasicJsonParsing)
    {
        struct SubObjectModel : public BaseModel
        {
            PFCountryCode CountryCode;

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
            PFEnum EnumValue;
            PointerArrayModel<int, int> ArrayValue;
            SubObjectModel SubObjectValue;

            void FromJson(const JsonValue& input)
            {
                JsonUtils::ObjectGetMember(input, "EnumValue", EnumValue);
                uint32_t arraySize;
                int const* const* arrayPtr;
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
        Assert::IsTrue(inputJson == outputJson);
    }
};

} // UnitTests
} // PlayFab
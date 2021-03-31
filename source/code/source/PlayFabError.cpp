#include "stdafx.h"

#include "PlayFabError.h"
#include "JsonUtils.h"

namespace PlayFab
{
    void PlayFabError::FromJson(const JsonValue& input)
    {
        JsonUtils::ObjectGetMember(input, "code", HttpCode);
        JsonUtils::ObjectGetMember(input, "status", HttpStatus);
        StdExtra::optional<int64_t> errorCode{};
        JsonUtils::ObjectGetMember(input, "errorCode", errorCode);
        if (errorCode)
        {
            ErrorCode = static_cast<PlayFabErrorCode>(*errorCode);
        }
        JsonUtils::ObjectGetMember(input, "error", ErrorName);
        JsonUtils::ObjectGetMember(input, "errorMessage", ErrorMessage);
        JsonUtils::ObjectGetMember(input, "errorDetails", ErrorDetails);
        JsonUtils::ObjectGetMember(input, "data", Data);
    }

    JsonValue PlayFabError::ToJson() const
    {
        // This is not expected to be used, but implemented for completeness
        JsonValue output{ rapidjson::kObjectType };
        //JsonUtils::ObjectAddMember(output, "code", HttpCode);
        //JsonUtils::ObjectAddMember(output, "status", HttpStatus);
        //JsonUtils::ObjectAddMember(output, "errorCode", static_cast<int>(ErrorCode));
        //JsonUtils::ObjectAddMember(output, "error", ErrorName);
        //JsonUtils::ObjectAddMember(output, "errorMessage", ErrorMessage);
        //JsonUtils::ObjectAddMember(output, "errorDetails", ErrorDetails);
        //JsonUtils::ObjectAddMember(output, "data", Data);
        return output;
    }

    String PlayFabError::GenerateErrorReport() const
    {
        Stringstream output;
        output << ErrorMessage;
        if (ErrorDetails.IsObject())
        {
            for (auto& pair : ErrorDetails.GetObject())
            {
                if (!pair.value.IsArray())
                {
                    continue;
                }

                output << "\n" << pair.name.GetString() << ": ";
                int valueIndex = 0;
                for (auto& value : pair.value.GetArray())
                {
                    if (!value.IsString())
                    {
                        continue;
                    }

                    if (valueIndex++ != 0)
                    {
                        output << ", ";
                    }

                    output << value.GetString();
                }
            }
        }

        if(!RequestId.empty())
        {
            output << " \n RequestId: " << RequestId << "\n";
        }

        return output.str();
    }

    PlayFabException::PlayFabException(PlayFabExceptionCode code, const char* const message) : std::runtime_error(message)
    {
        this->Code = code;
    }
}

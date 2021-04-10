#pragma once

#include <httpClient/httpClient.h>
#include "TaskQueue.h"
#include "AsyncOp.h"

namespace PlayFab
{
    struct ServiceResponse;

    // An Http client for make PlayFab service requests
    class HttpClient
    {
    public:
        HttpClient(String&& titleId);
        HttpClient(String&& baseServiceHost, String&& titleId);
        HttpClient(const HttpClient&) = default;
        ~HttpClient() = default;

        AsyncOp<ServiceResponse> MakePostRequest(
            const char* path,
            const UnorderedMap<String, String>& headers,
            const JsonValue& requestBody,
            const TaskQueue& queue
        ) const;

    private:
        String GetUrl(const char* path) const;

        Map<String, String> m_requestGetParams; // TODO Should these be configurable somehow?
        String m_baseServiceHost;
        String m_titleId;
    };

    // Wrapper around PlayFab service response.
    // See https://docs.microsoft.com/en-us/rest/api/playfab/client/authentication/loginwithcustomid?view=playfab-rest#apierrorwrapper for
    // more information.
    struct ServiceResponse
    {
        ServiceResponse() = default;
        ServiceResponse(const ServiceResponse&);
        ServiceResponse(ServiceResponse&&) = default;
        ServiceResponse& operator=(const ServiceResponse&);
        ServiceResponse& operator=(ServiceResponse&&) = default;
        ~ServiceResponse() = default;

        void FromJson(const JsonValue& input);

        // Fields from response body
        int HttpCode;
        String HttpStatus;
        ServiceErrorCode ErrorCode;
        String ErrorName;
        String ErrorMessage;
        JsonValue ErrorDetails;
        JsonValue Data;

        // From response header
        String RequestId;
    };
}

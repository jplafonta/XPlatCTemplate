#pragma once

#include <httpClient/httpClient.h>
#include <playfab/PlayFabError.h>
#include <playfab/InternalMemory.h>
#include <playfab/TaskQueue.h>

namespace PlayFab
{
    struct HttpResult
    {
        // Result indicating whether the request was successfully sent
        HRESULT networkErrorCode;
        String requestUrl;
        Json::Value requestBody;
        PlayFabError serviceResponse;
    };

    // An Http client for make PlayFab service requests
    class HttpClient
    {
    public:
        using HttpCallback = std::function<void(const HttpResult&)>;

        HttpClient(SharedPtr<PlayFabApiSettings> settings);
        HttpClient(const HttpClient&) = default;
        ~HttpClient() = default;

        void MakePostRequest(
            const char* url,
            const UnorderedMap<String, String>& headers,
            const char* requestBody,
            const TaskQueue& queue,
            HttpCallback&& callback
        ) const;

    private:
        SharedPtr<PlayFabApiSettings> m_settings;
    };
}
#include <stdafx.h>
#include <playfab/HttpClient.h>
#include <playfab/PlayFabSettings.h>

namespace PlayFab
{
    // RAII wrapper around HCCallHandle
    class HCHttpCall
    {
    public:
        static void Perform(
            const char* url,
            const UnorderedMap<String, String>& headers,
            const char* requestBody,
            HttpClient::HttpCallback&& callback
        );

        virtual ~HCHttpCall() noexcept;

    private:
        HCHttpCall(HttpClient::HttpCallback&& callback);
        HCHttpCall(const HCHttpCall& other) = delete;
        HCHttpCall& operator=(HCHttpCall other) = delete;

        HRESULT Initialize(
            const char* url,
            const UnorderedMap<String, String>& headers,
            const char* requestBody
        );
        void CompleteRequest(HRESULT networkErrorCode);

        static void CALLBACK HCPerformComplete(XAsyncBlock* async);

        HCCallHandle m_callHandle{ nullptr };
        HttpClient::HttpCallback m_callback;
        HttpResult m_result;
        XAsyncBlock m_asyncBlock{};
    };

    HttpClient::HttpClient(SharedPtr<PlayFabApiSettings> settings) :
        m_settings(std::move(settings))
    {
    }

    void HttpClient::MakePostRequest(
        const char* path,
        const UnorderedMap<String, String>& headers,
        const char* requestBody,
        HttpCallback&& callback
    ) const
    {
        if (m_settings->titleId.empty())
        {
            throw new PlayFabException(PlayFabExceptionCode::TitleNotSet, "PlayFabSettings::staticSettings->titleId has not been set properly. It must not be empty.");
        }
        auto fullUrl = m_settings->GetUrl(path);
        HCHttpCall::Perform(fullUrl.data(), headers, requestBody, std::move(callback));
    }

    HCHttpCall::HCHttpCall(HttpClient::HttpCallback&& callback) :
        m_callback(std::move(callback))
    {
    }

    HCHttpCall::~HCHttpCall() noexcept
    {
        if (m_callHandle)
        {
            HCHttpCallCloseHandle(m_callHandle);
        }
    }

    void HCHttpCall::Perform(
        const char* url,
        const UnorderedMap<String, String>& headers,
        const char* requestBody,
        HttpClient::HttpCallback&& callback
    )
    {
        auto call = UniquePtr<HCHttpCall>(new (Allocator<HCHttpCall>{}.allocate(1)) HCHttpCall(std::move(callback)));
        HRESULT hr = call->Initialize(url, headers, requestBody);
        if (FAILED(hr))
        {
            call->CompleteRequest(hr);
            return;
        }

        call->m_asyncBlock.callback = HCPerformComplete;
        call->m_asyncBlock.context = call.get();
        call->m_asyncBlock.queue = nullptr; // TODO

        hr = HCHttpCallPerformAsync(call->m_callHandle, &call->m_asyncBlock);
        if (SUCCEEDED(hr))
        {
            call.release();
        }
        else
        {
            call->CompleteRequest(hr);
        }
    }

    HRESULT HCHttpCall::Initialize(
        const char* url,
        const UnorderedMap<String, String>& headers,
        const char* requestBody
    )
    {
        // Initialize result
        m_result.requestUrl = url;

        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader()); // Non memhook

        if (!reader->parse(requestBody, requestBody + strlen(requestBody), &m_result.requestBody, nullptr))
        {
            // Set request body to null if parsing fails. Request likely to fail, but continue anyhow.
            m_result.requestBody = Json::Value::null;
        }

        RETURN_IF_FAILED(HCHttpCallCreate(&m_callHandle));
        RETURN_IF_FAILED(HCHttpCallRequestSetUrl(m_callHandle, "POST", url));

        // Add default PlayFab headers
        RETURN_IF_FAILED(HCHttpCallRequestSetHeader(m_callHandle, "Accept", "application/json", true));
        RETURN_IF_FAILED(HCHttpCallRequestSetHeader(m_callHandle, "Content-Type", "application/json; charset=utf-8", true));
        RETURN_IF_FAILED(HCHttpCallRequestSetHeader(m_callHandle, "X-PlayFabSDK", PlayFabSettings::versionString.data(), true));
        RETURN_IF_FAILED(HCHttpCallRequestSetHeader(m_callHandle, "X-ReportErrorAsSuccess", "true", true));

        for (const auto& pair : headers)
        {
            if (!pair.first.empty() && !pair.second.empty())
            {
                RETURN_IF_FAILED(HCHttpCallRequestSetHeader(m_callHandle, pair.first.data(), pair.second.data(), true));
            }
        }

        RETURN_IF_FAILED(HCHttpCallRequestSetRequestBodyString(m_callHandle, requestBody));
        return S_OK;
    }

    void HCHttpCall::CompleteRequest(HRESULT networkErrorCode)
    {
        m_result.networkErrorCode = networkErrorCode;
        m_callback(m_result);

        // Invoke the callback on the correct TaskQueue port
        /*queue.RunCompletion([requestContainer{ m_requestContainer.release() }]()
        {
            CallRequestContainerCallback callback = requestContainer->GetCallback();
            if (callback)
            {
                callback(
                    requestContainer->responseJson.get("code", Json::Value::null).asInt(),
                    requestContainer->responseString,
                    std::unique_ptr<CallRequestContainerBase>(static_cast<CallRequestContainerBase*>(requestContainer))
                );
            }
        });*/
    }

    void HCHttpCall::HCPerformComplete(XAsyncBlock* async)
    {
        // Re-Take ownership of HCHttpCall released in Perform
        UniquePtr<HCHttpCall> call{ static_cast<HCHttpCall*>(async->context) };

        // Get Http code and response string
        uint32_t httpCode{ 0 };
        HRESULT hr = HCHttpCallResponseGetStatusCode(call->m_callHandle, &httpCode);
        if (FAILED(hr))
        {
            call->CompleteRequest(hr);
            return;
        }

        const char* responseString{ nullptr };
        hr = HCHttpCallResponseGetResponseString(call->m_callHandle, &responseString);
        if (FAILED(hr))
        {
            call->CompleteRequest(hr);
            return;
        }

        Json::Value responseJson;
        Json::CharReaderBuilder jsonReaderFactory;
        std::unique_ptr<Json::CharReader> jsonReader(jsonReaderFactory.newCharReader()); // Non memhook
        JSONCPP_STRING jsonParseErrors; // Non memhook
        const bool parsedSuccessfully = jsonReader->parse(responseString, responseString + strlen(responseString), &responseJson, &jsonParseErrors);
        if (parsedSuccessfully)
        {
            // fully successful response
            call->m_result.serviceResponse.HttpCode = responseJson.get("code", Json::Value::null).asInt();
            call->m_result.serviceResponse.HttpStatus = responseJson.get("status", Json::Value::null).asCString();
            call->m_result.serviceResponse.Data = responseJson.get("data", Json::Value::null);
            call->m_result.serviceResponse.ErrorName = responseJson.get("error", Json::Value::null).asCString();
            call->m_result.serviceResponse.ErrorCode = static_cast<PlayFabErrorCode>(responseJson.get("errorCode", Json::Value::null).asInt());
            call->m_result.serviceResponse.ErrorMessage = responseJson.get("errorMessage", Json::Value::null).asCString();
            call->m_result.serviceResponse.ErrorDetails = responseJson.get("errorDetails", Json::Value::null);
        }
        else
        {
            call->m_result.serviceResponse.HttpCode = httpCode;
            call->m_result.serviceResponse.HttpStatus = responseString;
            call->m_result.serviceResponse.ErrorCode = PlayFabErrorCode::PlayFabErrorPartialFailure;
            call->m_result.serviceResponse.ErrorName = "Failed to parse PlayFab response";
            call->m_result.serviceResponse.ErrorMessage = jsonParseErrors.data();
        }

        // Get requestId response header
        const char* requestId;
        hr = HCHttpCallResponseGetHeader(call->m_callHandle, "X-RequestId", &requestId);
        if (FAILED(hr))
        {
            call->CompleteRequest(hr);
            return;
        }
        else if (requestId)
        {
            call->m_result.serviceResponse.RequestId = requestId;
        }

        call->CompleteRequest(S_OK);

        // HCHttpCall will be destroyed now
    }

}

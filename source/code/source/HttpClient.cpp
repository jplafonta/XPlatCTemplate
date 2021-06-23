#include "stdafx.h"
#include "HttpClient.h"
#include "JsonUtils.h"
#include "SdkVersion.h"

namespace PlayFab
{
constexpr char productionEnvironmentURL[]{ ".playfabapi.com" };

// RAII wrapper around HCCallHandle.
class HCHttpCall
{
public:
    static AsyncOp<ServiceResponse> Perform(
        const char* url,
        const UnorderedMap<String, String>& headers,
        const JsonValue& requestBody,
        const TaskQueue& queue
    );

    virtual ~HCHttpCall() noexcept;

private:
    HCHttpCall(const TaskQueue& queue);
    HCHttpCall(const HCHttpCall& other) = delete;
    HCHttpCall& operator=(HCHttpCall other) = delete;

    static void CALLBACK HCPerformComplete(XAsyncBlock* async);

    TaskQueue const m_queue;
    HCCallHandle m_callHandle{ nullptr };
    XAsyncBlock m_asyncBlock{};
    SharedPtr<AsyncOpContext<ServiceResponse>> m_asyncContext;
};

HttpClient::HttpClient(String&& titleId) :
    m_baseServiceHost{ productionEnvironmentURL },
    m_titleId{ std::move(titleId) }
{
}

HttpClient::HttpClient(String&& baseServiceHost, String&& titleId) :
    m_baseServiceHost{ std::move(baseServiceHost) },
    m_titleId{ std::move(titleId) }
{
}

String HttpClient::GetUrl(const char* path) const
{
    Stringstream fullUrl;
    fullUrl << "https://" << m_titleId << m_baseServiceHost << path;

    // Add sdk version param (used by service for telemetry)
    fullUrl << "?sdk=" << versionString;

    return fullUrl.str();
}

AsyncOp<ServiceResponse> HttpClient::MakePostRequest(
    const char* path,
    const UnorderedMap<String, String>& headers,
    const JsonValue& requestBody,
    const TaskQueue& queue
) const
{
    return HCHttpCall::Perform(GetUrl(path).data(), headers, requestBody, queue);
}

HCHttpCall::HCHttpCall(const TaskQueue& queue) :
    m_queue{ queue },
    m_asyncContext{ MakeShared<AsyncOpContext<ServiceResponse>>() }
{
}

HCHttpCall::~HCHttpCall() noexcept
{
    if (m_callHandle)
    {
        HCHttpCallCloseHandle(m_callHandle);
    }
}

AsyncOp<ServiceResponse> HCHttpCall::Perform(
    const char* url,
    const UnorderedMap<String, String>& headers,
    const JsonValue& requestBody,
    const TaskQueue& queue
)
{
    UniquePtr<HCHttpCall> call{ new (Allocator<HCHttpCall>{}.allocate(1)) HCHttpCall(queue) };

    // Consider adding a helper to schedule the completion to correct queue port. Currently if failures happen 
    // synchronously, the continuation will be invoked synchronously as well.

    // Set up HCHttpCallHandle
    RETURN_IF_FAILED(HCHttpCallCreate(&call->m_callHandle));
    RETURN_IF_FAILED(HCHttpCallRequestSetUrl(call->m_callHandle, "POST", url));

    // Add default PlayFab headers
    RETURN_IF_FAILED(HCHttpCallRequestSetHeader(call->m_callHandle, "Accept", "application/json", true));
    RETURN_IF_FAILED(HCHttpCallRequestSetHeader(call->m_callHandle, "Content-Type", "application/json; charset=utf-8", true));
    RETURN_IF_FAILED(HCHttpCallRequestSetHeader(call->m_callHandle, "X-PlayFabSDK", versionString, true));
    RETURN_IF_FAILED(HCHttpCallRequestSetHeader(call->m_callHandle, "X-ReportErrorAsSuccess", "true", true));

    for (const auto& pair : headers)
    {
        if (!pair.first.empty() && !pair.second.empty())
        {
            RETURN_IF_FAILED(HCHttpCallRequestSetHeader(call->m_callHandle, pair.first.data(), pair.second.data(), true));
        }
    }

    String jsonString = JsonUtils::WriteToString(requestBody);
    RETURN_IF_FAILED(HCHttpCallRequestSetRequestBodyString(call->m_callHandle, jsonString.data()));

    call->m_asyncBlock.callback = HCPerformComplete;
    call->m_asyncBlock.context = call.get();
    call->m_asyncBlock.queue = call->m_queue.GetHandle();

    RETURN_IF_FAILED(HCHttpCallPerformAsync(call->m_callHandle, &call->m_asyncBlock));

    auto asyncOp = AsyncOp<ServiceResponse>{ call->m_asyncContext };

    // At this point HCPerfromComplete will be called. Release call and reclaim
    // ownership in callback.
    call.release();

    return asyncOp;
}

void HCHttpCall::HCPerformComplete(XAsyncBlock* async)
{
    // Retake ownership of asyncContext
    UniquePtr<HCHttpCall> call{ static_cast<HCHttpCall*>(async->context) };
    auto& asyncOpContext{ call->m_asyncContext };

    try
    {
        // Try to parse the response body no matter what. PlayFab often returns a response body even
        // on failure and they can provide more details about what went wrong. If we are unable to parse the response
        // body correctly, fall back to returning the Http status code.
        const char* responseString{ nullptr };
        HRESULT hr = HCHttpCallResponseGetResponseString(call->m_callHandle, &responseString);
        if (FAILED(hr))
        {
            asyncOpContext->Complete(hr);
            return;
        }

        JsonDocument responseJson{ &JsonUtils::allocator };
        responseJson.Parse(responseString);
        if (responseJson.HasParseError())
        {
            // Couldn't parse response body, fall back to Http status code
            uint32_t httpCode{ 0 };
            hr = HCHttpCallResponseGetStatusCode(call->m_callHandle, &httpCode);
            if (FAILED(hr))
            {
                asyncOpContext->Complete(hr);
                return;
            }

            hr = HttpStatusToHR(httpCode);
            if (FAILED(hr))
            {
                asyncOpContext->Complete(hr);
                return;
            }
            
            // This is an unusal case. We weren't able to parse the response body, but the Http status code indicates that the
            // call was successful. Return the Json parse error in this case.
            Stringstream errorMessage;
            errorMessage << "Failed to parse PlayFab service response: " << rapidjson::GetParseError_En(responseJson.GetParseError());
            TRACE_ERROR(errorMessage.str().data());
            asyncOpContext->Complete(Result<ServiceResponse>{ E_FAIL, errorMessage.str() });
            return;
        }

        // Successful response from service (doesn't always indicate the call was successful, just that the service responded successfully)
        ServiceResponse response{};
        response.FromJson(responseJson);

        // Get requestId response header
        const char* requestId;
        hr = HCHttpCallResponseGetHeader(call->m_callHandle, "X-RequestId", &requestId);
        if (FAILED(hr))
        {
            asyncOpContext->Complete(hr);
            return;
        }
        else if (requestId)
        {
            response.RequestId = requestId;
        }

        asyncOpContext->Complete(std::move(response));
    }
    catch (...)
    {
        asyncOpContext->Complete(std::current_exception());
    }
}

ServiceResponse::ServiceResponse(const ServiceResponse& src) :
    HttpCode{ src.HttpCode },
    HttpStatus{ src.HttpStatus },
    ErrorCode{ src.ErrorCode },
    ErrorName{ src.ErrorName },
    ErrorMessage{ src.ErrorMessage },
    RequestId{ src.RequestId }
{
    JsonUtils::FromJson(src.ErrorDetails, ErrorDetails);
    JsonUtils::FromJson(src.Data, Data);
}

ServiceResponse& ServiceResponse::operator=(const ServiceResponse& src)
{
    HttpCode = src.HttpCode;
    HttpStatus = src.HttpStatus;
    ErrorCode = src.ErrorCode;
    ErrorName = src.ErrorName;
    ErrorMessage = src.ErrorMessage;
    JsonUtils::FromJson(src.ErrorDetails, ErrorDetails);
    JsonUtils::FromJson(src.Data, Data);
    RequestId = src.RequestId;
    return *this;
}

void ServiceResponse::FromJson(const JsonValue& input)
{
    JsonUtils::ObjectGetMember(input, "code", HttpCode);
    JsonUtils::ObjectGetMember(input, "status", HttpStatus);
    StdExtra::optional<int64_t> errorCode{};
    JsonUtils::ObjectGetMember(input, "errorCode", errorCode);
    if (errorCode.has_value())
    {
        ErrorCode = static_cast<ServiceErrorCode>(*errorCode);
    }
    else
    {
        ErrorCode = ServiceErrorCode::Success;
    }
    JsonUtils::ObjectGetMember(input, "error", ErrorName);
    JsonUtils::ObjectGetMember(input, "errorMessage", ErrorMessage);
    JsonUtils::ObjectGetMember(input, "errorDetails", ErrorDetails);
    JsonUtils::ObjectGetMember(input, "data", Data);
}

}

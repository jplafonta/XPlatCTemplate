#include "stdafx.h"
#include "Entity.h"

namespace PlayFab
{

using Wrappers::SafeString;

Entity::Entity(SharedPtr<PlayFab::HttpClient const> httpClient, SharedPtr<QoS::QoSAPI const> qosAPI, const Authentication::EntityTokenResponse& result) :
    m_httpClient{ std::move(httpClient) },
    m_qosAPI{ std::move(qosAPI) },
    m_entityToken{ MakeShared<PlayFab::EntityToken>(result) },
    m_id{ SafeString(result.Model().entity->id) },
    m_type{ SafeString(result.Model().entity->type) }
{
}

Entity::Entity(SharedPtr<PlayFab::HttpClient const> httpClient, SharedPtr<QoS::QoSAPI const> qosAPI, const Authentication::GetEntityTokenResponse& result) :
    m_httpClient{ std::move(httpClient) },
    m_qosAPI{ std::move(qosAPI) },
    m_entityToken{ MakeShared<PlayFab::EntityToken>(result) },
    m_id{ SafeString(result.entity->Model().id) },
    m_type{ SafeString(result.entity->Model().type) }
{
}

String const& Entity::EntityId() const
{
    return m_id;
}

String const& Entity::EntityType() const
{
    return m_type;
}

SharedPtr<PlayFab::EntityToken const> Entity::EntityToken() const
{
    return m_entityToken;
}

SharedPtr<PlayFab::HttpClient const> Entity::HttpClient() const
{
    return m_httpClient;
}

SharedPtr<QoS::QoSAPI const> Entity::QoSAPI() const
{
    return m_qosAPI;
}

AsyncOp<void> Entity::RefreshToken(const TaskQueue& queue)
{
    // GetEntityToken for 'this' entity
    PFAuthenticationGetEntityTokenRequest request;
    return GetEntityToken(request, queue).Then([](Result<SharedPtr<Entity>> result)
    {
        return Result<void>{result.hr, String{ result.errorMessage }};
    });
}

AsyncOp<SharedPtr<Entity>> Entity::GetEntityToken(
    const Authentication::GetEntityTokenRequest& request,
    const TaskQueue& queue
)
{
    if (!m_entityToken)
    {
        // Entity token should never be null
        assert(false);
        return E_UNEXPECTED;
    }

    UnorderedMap<String, String> headers;
    headers.emplace("X-EntityToken", m_entityToken->token);

    return m_httpClient->MakePostRequest(
        "/Authentication/GetEntityToken",
        headers,
        request.ToJson(),
        queue
    ).Then([ sharedThis{ shared_from_this() } ](Result<ServiceResponse> result) mutable -> Result<SharedPtr<Entity>>
    {
        RETURN_IF_FAILED(result.hr);

        auto serviceResponse = result.ExtractPayload();
        if (serviceResponse.HttpCode == 200)
        {
            Authentication::GetEntityTokenResponse resultModel;
            resultModel.FromJson(serviceResponse.Data);

            if (sharedThis->m_id == SafeString(resultModel.entity->Model().id))
            {
                // If we requested an EntityToken for ourselves, update m_authTokens and return "this"
                assert(sharedThis->m_type == SafeString(resultModel.entity->Model().type));
                sharedThis->m_entityToken = MakeShared<PlayFab::EntityToken>(resultModel);
                sharedThis->TokenRefreshedCallbacks.Invoke(sharedThis->m_entityToken);
                return Result<SharedPtr<Entity>>{ std::move(sharedThis) };
            }
            else
            {
                // Otherwise create and return a new Entity object
                return MakeShared<Entity>(sharedThis->m_httpClient, sharedThis->m_qosAPI, std::move(resultModel));
            }
        }
        else
        {
            return Result<SharedPtr<Entity>>{ ServiceErrorToHR(serviceResponse.ErrorCode), std::move(serviceResponse.ErrorMessage) };
        }
    });
}

EntityToken::EntityToken(const Authentication::EntityTokenResponse& tokenResponse) :
    PFEntityToken{},
    m_token{ SafeString(tokenResponse.Model().entityToken) },
    m_expiration{ tokenResponse.Model().tokenExpiration ? *tokenResponse.Model().tokenExpiration : StdExtra::optional<time_t>{} }
{
    token = m_token.empty() ? nullptr : m_token.data();
    expiration = m_expiration ? m_expiration.operator->() : nullptr;
}

EntityToken::EntityToken(const Authentication::GetEntityTokenResponse& tokenResponse) :
    PFEntityToken{},
    m_token{ tokenResponse.entityToken },
    m_expiration{ tokenResponse.tokenExpiration }
{
    token = m_token.empty() ? nullptr : m_token.data();
    expiration = m_expiration ? m_expiration.operator->() : nullptr;
}

EntityToken::EntityToken(const EntityToken& src) :
    m_token{ src.m_token },
    m_expiration{ src.m_expiration }
{
    token = m_token.empty() ? nullptr : m_token.data();
    expiration = m_expiration ? m_expiration.operator->() : nullptr;
}

EntityToken::EntityToken(EntityToken&& src) :
    PFEntityToken{ src },
    m_token{ std::move(src.m_token) },
    m_expiration{ src.m_expiration }
{
    expiration = m_expiration ? m_expiration.operator->() : nullptr;
}

}

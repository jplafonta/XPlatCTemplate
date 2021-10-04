#include "TestAppPch.h"
#include "TestContext.h"
#include "TestApp.h"
#include "AutoGenProfilesTests.h"
#include "XAsyncHelper.h"

namespace PlayFabUnit
{

using namespace PlayFab::Wrappers;

#pragma region GetGlobalPolicy

void AutoGenProfilesTests::FillGetGlobalPolicyRequest(PFProfilesGetGlobalPolicyRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{}"
    UNREFERENCED_PARAMETER(request); // TODO
}

HRESULT AutoGenProfilesTests::ValidatePFProfilesGetGlobalPolicyResponse(PFProfilesGetGlobalPolicyResponse* result)
{
    // result.permissions = PFProfilesEntityPermissionStatement const* const*;
    // result.permissionsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion

#pragma region GetProfile

void AutoGenProfilesTests::FillGetEntityProfileRequest(PFProfilesGetEntityProfileRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"Entity\": { \"Id\": \"1234567787392\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    PFEntityKeyWrapper<> entity{};
    entity.SetId("B64BE91E5DBD5597");
    entity.SetType("title_player_account");
    request.SetEntity(entity);
}

HRESULT AutoGenProfilesTests::ValidatePFProfilesGetEntityProfileResponse(PFProfilesGetEntityProfileResponse* result)
{
    // result.profile = PFProfilesEntityProfileBody const*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion

#pragma region GetProfiles

void AutoGenProfilesTests::FillGetEntityProfilesRequest(PFProfilesGetEntityProfilesRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"Entities\": [ {  \"Id\": \"1234567787392\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" }, {  \"Id\": \"42434567785265\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}"
    PFEntityKeyWrapper<> entity1{};
    entity1.SetId("B64BE91E5DBD5597");
    entity1.SetType("title_player_account");

    PFEntityKeyWrapper<> entity2{};
    entity2.SetId("61E37494004A216C");
    entity2.SetType("title_player_account");

    ModelVector<PFEntityKeyWrapper<>> entities{};
    entities.push_back(entity1);
    entities.push_back(entity2);
    request.SetEntities(entities);
}

HRESULT AutoGenProfilesTests::ValidatePFProfilesGetEntityProfilesResponse(PFProfilesGetEntityProfilesResponse* result)
{
    // result.profiles = PFProfilesEntityProfileBody const* const*;
    // result.profilesCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion

#pragma region GetTitlePlayersFromMasterPlayerAccountIds

void AutoGenProfilesTests::FillGetTitlePlayersFromMasterPlayerAccountIdsRequest(PFProfilesGetTitlePlayersFromMasterPlayerAccountIdsRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"MasterPlayerAccountIds\": [ \"1233455677\" ], \"TitleId\": \"{{TitleId}}\"}"
    UNREFERENCED_PARAMETER(request); // TODO
}

HRESULT AutoGenProfilesTests::ValidatePFProfilesGetTitlePlayersFromMasterPlayerAccountIdsResponse(PFProfilesGetTitlePlayersFromMasterPlayerAccountIdsResponse* result)
{
    // result.titleId = const char*;
    // result.titlePlayerAccounts = struct PFEntityKeyDictionaryEntry const*;
    // result.titlePlayerAccountsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion

#pragma region SetGlobalPolicy

void AutoGenProfilesTests::FillSetGlobalPolicyRequest(PFProfilesSetGlobalPolicyRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"Permissions\": [ {  \"Resource\": \"pfrn:data--title_player_account!90901000/Profile/SomethingCool\",  \"Action\": \"*\",  \"Effect\": \"Allow\",  \"Principal\": {  \"ChildOf\": {   \"EntityType\": \"[SELF]\"  }  },  \"Comment\": \"An example policy\" } ]}"
    UNREFERENCED_PARAMETER(request); // TODO
}

#pragma endregion

#pragma region SetProfileLanguage

void AutoGenProfilesTests::FillSetProfileLanguageRequest(PFProfilesSetProfileLanguageRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"Language\": \"en\", \"ExpectedVersion\": 123, \"Entity\": { \"Id\": \"1234\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    UNREFERENCED_PARAMETER(request); // TODO
}

HRESULT AutoGenProfilesTests::ValidatePFProfilesSetProfileLanguageResponse(PFProfilesSetProfileLanguageResponse* result)
{
    // result.operationResult = PFOperationTypes const*;
    // result.versionNumber = int32_t const*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion

#pragma region SetProfilePolicy

void AutoGenProfilesTests::FillSetEntityProfilePolicyRequest(PFProfilesSetEntityProfilePolicyRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"Statements\": [ {  \"Resource\": \"pfrn:data--*!*/Profile/Files/avatar.png\",  \"Action\": \"Read\",  \"Effect\": \"Allow\",  \"Principal\": {  \"FriendOf\": \"true\"  },  \"Comment\": \"Allow my friends to read my avatar\" } ], \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    UNREFERENCED_PARAMETER(request); // TODO
}

HRESULT AutoGenProfilesTests::ValidatePFProfilesSetEntityProfilePolicyResponse(PFProfilesSetEntityProfilePolicyResponse* result)
{
    // result.permissions = PFProfilesEntityPermissionStatement const* const*;
    // result.permissionsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion
 

}

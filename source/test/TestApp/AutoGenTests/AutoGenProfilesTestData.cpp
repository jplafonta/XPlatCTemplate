#include "TestAppPch.h"
#include "TestContext.h"
#include "TestApp.h"
#include "AutoGenProfilesTests.h"
#include "XAsyncHelper.h"
#include <playfab/PlayFabClientAuthApi.h>
#include <playfab/PlayFabClientApi.h>
#include <playfab/PlayFabProfilesApi.h>
#include <playfab/PlayFabAdminApi.h>
#include <playfab/PlayFabAuthenticationAuthApi.h>
#include <playfab/PlayFabClientDataModels.h>

namespace PlayFabUnit
{

void AutoGenProfilesTests::FillPlayFabProfilesGetGlobalPolicyRequest( PlayFab::ProfilesModels::GetGlobalPolicyRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenProfilesTests::ValidatePlayFabProfilesGetGlobalPolicyResponse( PlayFabProfilesGetGlobalPolicyResponse* result )
{
    // result.permissions = PlayFabProfilesEntityPermissionStatement const* const*;
    // result.permissionsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenProfilesTests::FillPlayFabProfilesGetEntityProfileRequest( PlayFab::ProfilesModels::GetEntityProfileRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"Entity\": { \"Id\": \"1234567787392\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenProfilesTests::ValidatePlayFabProfilesGetEntityProfileResponse( PlayFabProfilesGetEntityProfileResponse* result )
{
    // result.profile = PlayFabProfilesEntityProfileBody const*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenProfilesTests::FillPlayFabProfilesGetEntityProfilesRequest( PlayFab::ProfilesModels::GetEntityProfilesRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"Entities\": [ {  \"Id\": \"1234567787392\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" }, {  \"Id\": \"42434567785265\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenProfilesTests::ValidatePlayFabProfilesGetEntityProfilesResponse( PlayFabProfilesGetEntityProfilesResponse* result )
{
    // result.profiles = PlayFabProfilesEntityProfileBody const* const*;
    // result.profilesCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenProfilesTests::FillPlayFabProfilesGetTitlePlayersFromMasterPlayerAccountIdsRequest( PlayFab::ProfilesModels::GetTitlePlayersFromMasterPlayerAccountIdsRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"MasterPlayerAccountIds\": [ \"1233455677\" ], \"TitleId\": \"{{TitleId}}\"}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenProfilesTests::ValidatePlayFabProfilesGetTitlePlayersFromMasterPlayerAccountIdsResponse( PlayFabProfilesGetTitlePlayersFromMasterPlayerAccountIdsResponse* result )
{
    // result.titleId = const char*;
    // result.titlePlayerAccounts = struct PlayFabEntityKeyDictionaryEntry const*;
    // result.titlePlayerAccountsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenProfilesTests::FillPlayFabProfilesSetGlobalPolicyRequest( PlayFab::ProfilesModels::SetGlobalPolicyRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"Permissions\": [ {  \"Resource\": \"pfrn:data--title_player_account!90901000/Profile/SomethingCool\",  \"Action\": \"*\",  \"Effect\": \"Allow\",  \"Principal\": {  \"ChildOf\": {   \"EntityType\": \"[SELF]\"  }  },  \"Comment\": \"An example policy\" } ]}" );
    request->FromJson(inputJson);
}

void AutoGenProfilesTests::FillPlayFabProfilesSetProfileLanguageRequest( PlayFab::ProfilesModels::SetProfileLanguageRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"Language\": \"en\", \"ExpectedVersion\": 123, \"Entity\": { \"Id\": \"1234\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenProfilesTests::ValidatePlayFabProfilesSetProfileLanguageResponse( PlayFabProfilesSetProfileLanguageResponse* result )
{
    // result.operationResult = PlayFabProfilesOperationTypes const*;
    // result.versionNumber = int32_t const*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenProfilesTests::FillPlayFabProfilesSetEntityProfilePolicyRequest( PlayFab::ProfilesModels::SetEntityProfilePolicyRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"Statements\": [ {  \"Resource\": \"pfrn:data--*!*/Profile/Files/avatar.png\",  \"Action\": \"Read\",  \"Effect\": \"Allow\",  \"Principal\": {  \"FriendOf\": \"true\"  },  \"Comment\": \"Allow my friends to read my avatar\" } ], \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenProfilesTests::ValidatePlayFabProfilesSetEntityProfilePolicyResponse( PlayFabProfilesSetEntityProfilePolicyResponse* result )
{
    // result.permissions = PlayFabProfilesEntityPermissionStatement const* const*;
    // result.permissionsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

 

}

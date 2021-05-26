#include "TestAppPch.h"
#include "TestContext.h"
#include "TestApp.h"
#include "AutoGenDataTests.h"
#include "XAsyncHelper.h"
#include <playfab/PlayFabClientAuthApi.h>
#include <playfab/PlayFabClientApi.h>
#include <playfab/PlayFabProfilesApi.h>
#include <playfab/PlayFabAdminApi.h>
#include <playfab/PlayFabAuthenticationAuthApi.h>
#include <playfab/PlayFabClientDataModels.h>

namespace PlayFabUnit
{

void AutoGenDataTests::FillPlayFabDataAbortFileUploadsRequest( PlayFab::DataModels::AbortFileUploadsRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"FileNames\": [ \"avatar\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenDataTests::ValidatePlayFabDataAbortFileUploadsResponse( PlayFabDataAbortFileUploadsResponse* result )
{
    // result.entity = PlayFabEntityKey const*;
    // result.profileVersion = int32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenDataTests::FillPlayFabDataDeleteFilesRequest( PlayFab::DataModels::DeleteFilesRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"FileNames\": [ \"game_state\", \"avatar\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenDataTests::ValidatePlayFabDataDeleteFilesResponse( PlayFabDataDeleteFilesResponse* result )
{
    // result.entity = PlayFabEntityKey const*;
    // result.profileVersion = int32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenDataTests::FillPlayFabDataFinalizeFileUploadsRequest( PlayFab::DataModels::FinalizeFileUploadsRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"ProfileVersion\": 0, \"FileNames\": [ \"profile_configuration\", \"game_state\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenDataTests::ValidatePlayFabDataFinalizeFileUploadsResponse( PlayFabDataFinalizeFileUploadsResponse* result )
{
    // result.entity = PlayFabEntityKey const*;
    // result.metadata = struct PlayFabDataGetFileMetadataDictionaryEntry const*;
    // result.metadataCount = uint32_t;
    // result.profileVersion = int32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenDataTests::FillPlayFabDataGetFilesRequest( PlayFab::DataModels::GetFilesRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenDataTests::ValidatePlayFabDataGetFilesResponse( PlayFabDataGetFilesResponse* result )
{
    // result.entity = PlayFabEntityKey const*;
    // result.metadata = struct PlayFabDataGetFileMetadataDictionaryEntry const*;
    // result.metadataCount = uint32_t;
    // result.profileVersion = int32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenDataTests::FillPlayFabDataGetObjectsRequest( PlayFab::DataModels::GetObjectsRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"Entity\": { \"Id\": \"ABC90712ABC\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenDataTests::ValidatePlayFabDataGetObjectsResponse( PlayFabDataGetObjectsResponse* result )
{
    // result.entity = PlayFabEntityKey const*;
    // result.objects = struct PlayFabDataObjectResultDictionaryEntry const*;
    // result.objectsCount = uint32_t;
    // result.profileVersion = int32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenDataTests::FillPlayFabDataInitiateFileUploadsRequest( PlayFab::DataModels::InitiateFileUploadsRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"FileNames\": [ \"profile_configuration\", \"game_state\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenDataTests::ValidatePlayFabDataInitiateFileUploadsResponse( PlayFabDataInitiateFileUploadsResponse* result )
{
    // result.entity = PlayFabEntityKey const*;
    // result.profileVersion = int32_t;
    // result.uploadDetails = PlayFabDataInitiateFileUploadMetadata const* const*;
    // result.uploadDetailsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenDataTests::FillPlayFabDataSetObjectsRequest( PlayFab::DataModels::SetObjectsRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"ExpectedProfileVersion\": 5, \"Objects\": [ {  \"ObjectName\": \"SaveSate\",  \"DataObject\": {  \"PlayerDetails\": {   \"LastMissionSuccess\": \"2017-06-15T11:05:19Z\",   \"LastMissionFailure\": \"2017-06-12T11:05:19Z\",   \"MapPosition\": [   22,   37.78   ],   \"IsPaidUpgrade\": true  },  \"GameSettings\": {   \"Screen\": \"FriendsList\",   \"Favorites\": [   \"Place 1\",   \"Place 2\"   ]  }  },  \"SimpleStatements\": {  \"Read\": [   {   \"Friend\": true   }  ],  \"Write\": [   \"Self\"  ]  } } ], \"Entity\": { \"Id\": \"A8140AB9109712B\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenDataTests::ValidatePlayFabDataSetObjectsResponse( PlayFabDataSetObjectsResponse* result )
{
    // result.profileVersion = int32_t;
    // result.setResults = PlayFabDataSetObjectInfo const* const*;
    // result.setResultsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

 

}

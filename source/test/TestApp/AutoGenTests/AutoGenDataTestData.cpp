#include "TestAppPch.h"
#include "TestContext.h"
#include "TestApp.h"
#include "AutoGenDataTests.h"
#include "XAsyncHelper.h"

namespace PlayFabUnit
{

using namespace PlayFab::Wrappers;

#pragma region AbortFileUploads

void AutoGenDataTests::FillAbortFileUploadsPrerequisiteInitiateFileUploadsRequest(PFDataInitiateFileUploadsRequestWrapper<>& request)
{
    // Example request: "{ \"FileNames\": [ \"profile_configuration\", \"game_state\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetFileNames({ "TestFileAbort" });
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::StoreAbortFileUploadsPrerequisitePFDataInitiateFileUploadsResponse(std::shared_ptr<InitiateFileUploadsResponseHolder> result)
{
    testData.m_InitiateFileUploadsResponse = result;
    return S_OK;
}

void AutoGenDataTests::FillAbortFileUploadsRequest(PFDataAbortFileUploadsRequestWrapper<>& request)
{
    // Example Request: "{ \"FileNames\": [ \"avatar\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetFileNames({ "TestFileAbort" });
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::ValidatePFDataAbortFileUploadsResponse(PFDataAbortFileUploadsResponse* result)
{
    // result.entity = PFEntityKey const*;
    // result.profileVersion = int32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion

#pragma region DeleteFiles

void AutoGenDataTests::FillDeleteFilesPrerequisiteInitiateFileUploadsRequest(PFDataInitiateFileUploadsRequestWrapper<>& request)
{
    // Example request: "{ \"FileNames\": [ \"profile_configuration\", \"game_state\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetFileNames({ "TestFileDelete" });
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::StoreDeleteFilesPrerequisitePFDataInitiateFileUploadsResponse(std::shared_ptr<InitiateFileUploadsResponseHolder> result)
{
    testData.m_InitiateFileUploadsResponse = result;
    return S_OK;
}

void AutoGenDataTests::FillDeleteFilesPrerequisiteFinalizeFileUploadsRequest(PFDataFinalizeFileUploadsRequestWrapper<>& request)
{
    // Example request: "{ \"ProfileVersion\": 0, \"FileNames\": [ \"profile_configuration\", \"game_state\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetFileNames({ "TestFileDelete" });
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::StoreDeleteFilesPrerequisitePFDataFinalizeFileUploadsResponse(std::shared_ptr<FinalizeFileUploadsResponseHolder> result)
{
    testData.m_FinalizeFileUploadsResponse = result;
    return S_OK;
}

void AutoGenDataTests::FillDeleteFilesRequest(PFDataDeleteFilesRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"FileNames\": [ \"game_state\", \"avatar\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetFileNames({ "TestFileDelete" });
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::ValidatePFDataDeleteFilesResponse(PFDataDeleteFilesResponse* result)
{
    // result.entity = PFEntityKey const*;
    // result.profileVersion = int32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion

#pragma region FinalizeFileUploads

void AutoGenDataTests::FillFinalizeFileUploadsPrerequisiteInitiateFileUploadsRequest(PFDataInitiateFileUploadsRequestWrapper<>& request)
{
    // Example request: "{ \"FileNames\": [ \"profile_configuration\", \"game_state\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetFileNames({ "TestFileFinalize" });
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::StoreFinalizeFileUploadsPrerequisitePFDataInitiateFileUploadsResponse(std::shared_ptr<InitiateFileUploadsResponseHolder> result)
{
    testData.m_InitiateFileUploadsResponse = result;
    return S_OK;
}

void AutoGenDataTests::FillFinalizeFileUploadsRequest(PFDataFinalizeFileUploadsRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"ProfileVersion\": 0, \"FileNames\": [ \"profile_configuration\", \"game_state\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetFileNames({ "TestFileFinalize" });
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::ValidatePFDataFinalizeFileUploadsResponse(PFDataFinalizeFileUploadsResponse* result)
{
    // result.entity = PFEntityKey const*;
    // result.metadata = struct PFDataGetFileMetadataDictionaryEntry const*;
    // result.metadataCount = uint32_t;
    // result.profileVersion = int32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenDataTests::FillFinalizeFileUploadsCleanupDeleteFilesRequest(PFDataDeleteFilesRequestWrapper<>& request)
{
    // Example Request: "{ \"FileNames\": [ \"game_state\", \"avatar\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetFileNames({ "TestFileFinalize" });
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::StoreFinalizeFileUploadsCleanupPFDataDeleteFilesResponse(std::shared_ptr<DeleteFilesResponseHolder> result)
{
    testData.m_DeleteFilesResponse = result;
    return S_OK;
}

#pragma endregion

#pragma region GetFiles

void AutoGenDataTests::FillGetFilesRequest(PFDataGetFilesRequestWrapper<>& request)
{
    // Example Request: "{ \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::ValidatePFDataGetFilesResponse(PFDataGetFilesResponse* result)
{
    // result.entity = PFEntityKey const*;
    // result.metadata = struct PFDataGetFileMetadataDictionaryEntry const*;
    // result.metadataCount = uint32_t;
    // result.profileVersion = int32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion

#pragma region GetObjects

void AutoGenDataTests::FillGetObjectsRequest(PFDataGetObjectsRequestWrapper<>& request)
{
    // Example Request: "{ \"Entity\": { \"Id\": \"ABC90712ABC\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::ValidatePFDataGetObjectsResponse(PFDataGetObjectsResponse* result)
{
    // result.entity = PFEntityKey const*;
    // result.objects = struct PFDataObjectResultDictionaryEntry const*;
    // result.objectsCount = uint32_t;
    // result.profileVersion = int32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion

#pragma region InitiateFileUploads

void AutoGenDataTests::FillInitiateFileUploadsRequest(PFDataInitiateFileUploadsRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"FileNames\": [ \"profile_configuration\", \"game_state\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetFileNames({ "TestFileInitiate" });
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::ValidatePFDataInitiateFileUploadsResponse(PFDataInitiateFileUploadsResponse* result)
{
    // result.entity = PFEntityKey const*;
    // result.profileVersion = int32_t;
    // result.uploadDetails = PFDataInitiateFileUploadMetadata const* const*;
    // result.uploadDetailsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenDataTests::FillInitiateFileUploadsCleanupAbortFileUploadsRequest(PFDataAbortFileUploadsRequestWrapper<>& request)
{
    // Example Request: "{ \"FileNames\": [ \"avatar\" ], \"Entity\": { \"Id\": \"ID\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    request.SetFileNames({ "TestFileInitiate" });
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::StoreInitiateFileUploadsCleanupPFDataAbortFileUploadsResponse(std::shared_ptr<AbortFileUploadsResponseHolder> result)
{
    testData.m_AbortFileUploadsResponse = result;
    return S_OK;
}

#pragma endregion

#pragma region SetObjects

void AutoGenDataTests::FillSetObjectsPrerequisiteSetObjectsRequest(PFDataSetObjectsRequestWrapper<>& request)
{
    // Example request: "{ \"ExpectedProfileVersion\": 5, \"Objects\": [ {  \"ObjectName\": \"SaveSate\",  \"DataObject\": {  \"PlayerDetails\": {   \"LastMissionSuccess\": \"2017-06-15T11:05:19Z\",   \"LastMissionFailure\": \"2017-06-12T11:05:19Z\",   \"MapPosition\": [   22,   37.78   ],   \"IsPaidUpgrade\": true  },  \"GameSettings\": {   \"Screen\": \"FriendsList\",   \"Favorites\": [   \"Place 1\",   \"Place 2\"   ]  }  },  \"SimpleStatements\": {  \"Read\": [   {   \"Friend\": true   }  ],  \"Write\": [   \"Self\"  ]  } } ], \"Entity\": { \"Id\": \"A8140AB9109712B\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    ModelVector<PFDataSetObjectWrapper<>> objects;
    objects.push_back(PFDataSetObject{});
    request.SetObjects(std::move(objects));
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::StoreSetObjectsPrerequisitePFDataSetObjectsResponse(std::shared_ptr<SetObjectsResponseHolder> result)
{
    testData.m_SetObjectsResponse = result;
    return S_OK;
}

void AutoGenDataTests::FillSetObjectsRequest(PFDataSetObjectsRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"ExpectedProfileVersion\": 5, \"Objects\": [ {  \"ObjectName\": \"SaveSate\",  \"DataObject\": {  \"PlayerDetails\": {   \"LastMissionSuccess\": \"2017-06-15T11:05:19Z\",   \"LastMissionFailure\": \"2017-06-12T11:05:19Z\",   \"MapPosition\": [   22,   37.78   ],   \"IsPaidUpgrade\": true  },  \"GameSettings\": {   \"Screen\": \"FriendsList\",   \"Favorites\": [   \"Place 1\",   \"Place 2\"   ]  }  },  \"SimpleStatements\": {  \"Read\": [   {   \"Friend\": true   }  ],  \"Write\": [   \"Self\"  ]  } } ], \"Entity\": { \"Id\": \"A8140AB9109712B\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}"
    ModelVector<PFDataSetObjectWrapper<>> objects;
    objects.push_back(PFDataSetObject{});
    request.SetObjects(std::move(objects));
    request.SetEntity(PFEntityKey{ "B64BE91E5DBD5597", "title_player_account" });
}

HRESULT AutoGenDataTests::ValidatePFDataSetObjectsResponse(PFDataSetObjectsResponse* result)
{
    // result.profileVersion = int32_t;
    // result.setResults = PFDataSetObjectInfo const* const*;
    // result.setResultsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion
 

}

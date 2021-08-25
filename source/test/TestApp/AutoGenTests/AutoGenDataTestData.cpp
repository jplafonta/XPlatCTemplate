#include "TestAppPch.h"
#include "TestContext.h"
#include "TestApp.h"
#include "AutoGenDataTests.h"
#include "XAsyncHelper.h"

namespace PlayFabUnit
{

#pragma region AbortFileUploads

    void AutoGenDataTests::FillAbortFileUploadsPrerequisiteInitiateFileUploadsRequest(PlayFab::DataModels::InitiateFileUploadsRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"FileNames\": [ \"TestFileAbort\" ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenDataTests::StoreAbortFileUploadsPrerequisitePFDataInitiateFileUploadsResponse(PFDataInitiateFileUploadsResponse* result)
    {
        PlayFab::DataModels::InitiateFileUploadsResponse internalResult(*result);
        testData.m_InitiateFileUploadsResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenDataTests::FillAbortFileUploadsRequest(PlayFab::DataModels::AbortFileUploadsRequest* request)
    {
        // TODO: debug Failing test
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"FileNames\": [ \"TestFileAbort\" ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
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

    void AutoGenDataTests::FillDeleteFilesPrerequisiteInitiateFileUploadsRequest(PlayFab::DataModels::InitiateFileUploadsRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"FileNames\": [ \"TestFileDelete\" ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenDataTests::StoreDeleteFilesPrerequisitePFDataInitiateFileUploadsResponse(PFDataInitiateFileUploadsResponse* result)
    {
        PlayFab::DataModels::InitiateFileUploadsResponse internalResult(*result);
        testData.m_InitiateFileUploadsResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenDataTests::FillDeleteFilesPrerequisiteFinalizeFileUploadsRequest(PlayFab::DataModels::FinalizeFileUploadsRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"FileNames\": [ \"TestFileDelete\" ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenDataTests::StoreDeleteFilesPrerequisitePFDataFinalizeFileUploadsResponse(PFDataFinalizeFileUploadsResponse* result)
    {
        PlayFab::DataModels::FinalizeFileUploadsResponse internalResult(*result);
        testData.m_FinalizeFileUploadsResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenDataTests::FillDeleteFilesRequest(PlayFab::DataModels::DeleteFilesRequest* request)
    {
        // TODO: debug Failing test
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"FileNames\": [ \"TestFileDelete\" ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
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

    void AutoGenDataTests::FillFinalizeFileUploadsPrerequisiteInitiateFileUploadsRequest(PlayFab::DataModels::InitiateFileUploadsRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"FileNames\": [ \"TestFileFinalize\" ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenDataTests::StoreFinalizeFileUploadsPrerequisitePFDataInitiateFileUploadsResponse(PFDataInitiateFileUploadsResponse* result)
    {
        PlayFab::DataModels::InitiateFileUploadsResponse internalResult(*result);
        testData.m_InitiateFileUploadsResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenDataTests::FillFinalizeFileUploadsRequest(PlayFab::DataModels::FinalizeFileUploadsRequest* request)
    {
        // TODO: debug Failing test
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"FileNames\": [ \"TestFileFinalize\" ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
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

    void AutoGenDataTests::FillFinalizeFileUploadsCleanupDeleteFilesRequest(PlayFab::DataModels::DeleteFilesRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"FileNames\": [ \"TestFileFinalize\" ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenDataTests::StoreFinalizeFileUploadsCleanupPFDataDeleteFilesResponse(PFDataDeleteFilesResponse* result)
    {
        PlayFab::DataModels::DeleteFilesResponse internalResult(*result);
        testData.m_DeleteFilesResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

#pragma endregion

#pragma region GetFiles

    void AutoGenDataTests::FillGetFilesRequest(PlayFab::DataModels::GetFilesRequest* request)
    {
        // TODO: debug Failing test
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
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

    void AutoGenDataTests::FillGetObjectsRequest(PlayFab::DataModels::GetObjectsRequest* request)
    {
        // TODO: debug Failing test
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
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

    void AutoGenDataTests::FillInitiateFileUploadsRequest(PlayFab::DataModels::InitiateFileUploadsRequest* request)
    {
        // TODO: debug Failing test
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"FileNames\": [ \"TestFileInitiate\" ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
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

    void AutoGenDataTests::FillInitiateFileUploadsCleanupAbortFileUploadsRequest(PlayFab::DataModels::AbortFileUploadsRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"FileNames\": [ \"TestFileInitiate\" ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenDataTests::StoreInitiateFileUploadsCleanupPFDataAbortFileUploadsResponse(PFDataAbortFileUploadsResponse* result)
    {
        PlayFab::DataModels::AbortFileUploadsResponse internalResult(*result);
        testData.m_AbortFileUploadsResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

#pragma endregion

#pragma region SetObjects

    void AutoGenDataTests::FillSetObjectsPrerequisiteSetObjectsRequest(PlayFab::DataModels::SetObjectsRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Objects\": [ {  \"TestObject\": \"PrereqValue\" } ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenDataTests::StoreSetObjectsPrerequisitePFDataSetObjectsResponse(PFDataSetObjectsResponse* result)
    {
        PlayFab::DataModels::SetObjectsResponse internalResult(*result);
        testData.m_SetObjectsResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenDataTests::FillSetObjectsRequest(PlayFab::DataModels::SetObjectsRequest* request)
    {
        // TODO: debug Failing test
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Objects\": [ {  \"TestObject\": \"TestValue\" } ], \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        request->FromJson(inputJson);
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

#include "TestAppPch.h"
#include "TestContext.h"
#include "TestApp.h"
#include "AutoGenSharedGroupsTests.h"
#include "XAsyncHelper.h"

namespace PlayFabUnit
{

using namespace PlayFab::Wrappers;

#pragma region AdminSetPublisherData

void AutoGenSharedGroupsTests::FillSetPublisherDataRequest(PFSetPublisherDataRequestWrapper<>& request)
{
    // TODO: debug PassingButNoData test
    // Example Request: "{ \"Key\": \"Running Speed\", \"Value\": \"1.03\"}"
    UNREFERENCED_PARAMETER(request); // TODO
}

#pragma endregion

#pragma region ClientAddSharedGroupMembers

void AutoGenSharedGroupsTests::FillAddSharedGroupMembersRequest(PFSharedGroupsAddSharedGroupMembersRequestWrapper<>& request)
{
    // TODO: debug Failing test
    // Example Request: "{ \"SharedGroupId\": \"Clan Data\", \"PlayFabIds\": [ \"D984A64B832\", \"F74A523E1562\" ]}"
    UNREFERENCED_PARAMETER(request); // TODO
}

#pragma endregion

#pragma region ClientCreateSharedGroup

void AutoGenSharedGroupsTests::FillCreateSharedGroupRequest(PFSharedGroupsCreateSharedGroupRequestWrapper<>& request)
{
    // Example Request: "{ \"SharedGroupId\": \"Clan Data\"}"
    request.SetSharedGroupId("CreateSharedGroup");
}

HRESULT AutoGenSharedGroupsTests::ValidatePFSharedGroupsCreateSharedGroupResult(PFSharedGroupsCreateSharedGroupResult* result)
{
    // result.sharedGroupId = const char*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenSharedGroupsTests::FillClientCreateSharedGroupCleanupDeleteSharedGroupRequest(PFSharedGroupsDeleteSharedGroupRequestWrapper<>& request)
{
    request.SetSharedGroupId("CreateSharedGroup");
}

#pragma endregion

#pragma region ClientGetSharedGroupData

void AutoGenSharedGroupsTests::FillGetSharedGroupDataRequest(PFSharedGroupsGetSharedGroupDataRequestWrapper<>& request)
{
    // TODO: debug PassingButNoData test
    // Example Request: "{ \"SharedGroupId\": \"Clan Data\", \"Keys\": [ \"ClanKills\", \"LastClanUpdate\" ], \"GetMembers\": true}"
    UNREFERENCED_PARAMETER(request); // TODO
}

HRESULT AutoGenSharedGroupsTests::ValidatePFSharedGroupsGetSharedGroupDataResult(PFSharedGroupsGetSharedGroupDataResult* result)
{
    // result.data = struct PFSharedGroupsSharedGroupDataRecordDictionaryEntry const*;
    // result.dataCount = uint32_t;
    // result.members = const char* const*;
    // result.membersCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

#pragma endregion

#pragma region ClientRemoveSharedGroupMembers

void AutoGenSharedGroupsTests::FillRemoveSharedGroupMembersRequest(PFSharedGroupsRemoveSharedGroupMembersRequestWrapper<>& request)
{
    // TODO: debug PassingButNoData test
    // Example Request: "{ \"SharedGroupId\": \"Clan Data\", \"PlayFabIds\": [ \"D984A64B832\", \"F74A523E1562\" ]}"
    UNREFERENCED_PARAMETER(request); // TODO
}

#pragma endregion

#pragma region ClientUpdateSharedGroupData

void AutoGenSharedGroupsTests::FillUpdateSharedGroupDataRequest(PFSharedGroupsUpdateSharedGroupDataRequestWrapper<>& request)
{
    // TODO: debug PassingButNoData test
    // Example Request: "{ \"SharedGroupId\": \"Clan Data\", \"Data\": { \"ClanKills\": \"34\", \"LastClanUpdate\": \"2014-10-03T09:21:14Z\" }, \"Permission\": \"Public\"}"
    UNREFERENCED_PARAMETER(request); // TODO
}

#pragma endregion

#pragma region ServerAddSharedGroupMembers


#pragma endregion

#pragma region ServerCreateSharedGroup


void AutoGenSharedGroupsTests::FillServerCreateSharedGroupCleanupDeleteSharedGroupRequest(PFSharedGroupsDeleteSharedGroupRequestWrapper<>& request)
{
    request.SetSharedGroupId("CreateSharedGroup");
}

#pragma endregion

#pragma region ServerDeleteSharedGroup

void AutoGenSharedGroupsTests::FillServerDeleteSharedGroupPrerequisiteCreateSharedGroupRequest(PFSharedGroupsCreateSharedGroupRequestWrapper<>& request)
{
    // Example request: "{ \"SharedGroupId\": \"Clan Data\"}"
    UNREFERENCED_PARAMETER(request); // TODO
}

HRESULT AutoGenSharedGroupsTests::StoreServerDeleteSharedGroupPrerequisitePFSharedGroupsCreateSharedGroupResult(std::shared_ptr<CreateSharedGroupResultHolder> result)
{
    testData.m_CreateSharedGroupResult = result;
    return S_OK;
}

void AutoGenSharedGroupsTests::FillDeleteSharedGroupRequest(PFSharedGroupsDeleteSharedGroupRequestWrapper<>& request)
{
    // TODO: debug PassingButNoData test
    // Example Request: "{ \"SharedGroupId\": \"Clan Data\"}"
    UNREFERENCED_PARAMETER(request); // TODO
}

#pragma endregion

#pragma region ServerGetSharedGroupData


#pragma endregion

#pragma region ServerRemoveSharedGroupMembers


#pragma endregion

#pragma region ServerUpdateSharedGroupData


#pragma endregion
 

}

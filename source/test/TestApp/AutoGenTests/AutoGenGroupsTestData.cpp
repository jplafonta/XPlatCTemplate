#include "TestAppPch.h"
#include "TestContext.h"
#include "TestApp.h"
#include "AutoGenGroupsTests.h"
#include "XAsyncHelper.h"

namespace PlayFabUnit
{

#pragma region AcceptGroupApplication

    void AutoGenGroupsTests::FillAcceptGroupApplicationPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreAcceptGroupApplicationPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillAcceptGroupApplicationPrerequisiteApplyToGroupRequest(PlayFab::GroupsModels::ApplyToGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreAcceptGroupApplicationPrerequisitePFGroupsApplyToGroupResponse(PFGroupsApplyToGroupResponse* result)
    {
        PlayFab::GroupsModels::ApplyToGroupResponse internalResult(*result);
        testData.m_ApplyToGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillAcceptGroupApplicationRequest(PlayFab::GroupsModels::AcceptGroupApplicationRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        inputJson["Entity"]["Id"].SetString(testData.m_ApplyToGroupResponse.entity->key->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    void AutoGenGroupsTests::FillAcceptGroupApplicationCleanupRemoveMembersRequest(PlayFab::GroupsModels::RemoveMembersRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Members\": [ {  \"Id\": \"90901000\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        inputJson["Members"][0]["Id"].SetString(testData.m_ApplyToGroupResponse.entity->key->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region AcceptGroupInvitation

    void AutoGenGroupsTests::FillAcceptGroupInvitationPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreAcceptGroupInvitationPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillAcceptGroupInvitationPrerequisiteInviteToGroupRequest(PlayFab::GroupsModels::InviteToGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"33D5E873FB4D8629\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreAcceptGroupInvitationPrerequisitePFGroupsInviteToGroupResponse(PFGroupsInviteToGroupResponse* result)
    {
        PlayFab::GroupsModels::InviteToGroupResponse internalResult(*result);
        testData.m_InviteToGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillAcceptGroupInvitationRequest(PlayFab::GroupsModels::AcceptGroupInvitationRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    void AutoGenGroupsTests::FillAcceptGroupInvitationCleanupRemoveMembersRequest(PlayFab::GroupsModels::RemoveMembersRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Members\": [ {  \"Id\": \"90901000\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        inputJson["Members"][0]["Id"].SetString(testData.m_InviteToGroupResponse.invitedEntity->key->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region AddMembers

    void AutoGenGroupsTests::FillAddMembersPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreAddMembersPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillAddMembersRequest(PlayFab::GroupsModels::AddMembersRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Members\": [ {  \"Id\": \"FED18FB0293BA445\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    void AutoGenGroupsTests::FillAddMembersCleanupRemoveMembersRequest(PlayFab::GroupsModels::RemoveMembersRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Members\": [ {  \"Id\": \"FED18FB0293BA445\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region ApplyToGroup

    void AutoGenGroupsTests::FillApplyToGroupPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreApplyToGroupPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillApplyToGroupRequest(PlayFab::GroupsModels::ApplyToGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsApplyToGroupResponse(PFGroupsApplyToGroupResponse* result)
    {
        // result.entity = PFGroupsEntityWithLineage const*;
        // result.expires = time_t;
        // result.group = PFEntityKey const*;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

    void AutoGenGroupsTests::FillApplyToGroupCleanupRemoveGroupApplicationRequest(PlayFab::GroupsModels::RemoveGroupApplicationRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"33D5E873FB4D8629\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region BlockEntity

    void AutoGenGroupsTests::FillBlockEntityPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreBlockEntityPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillBlockEntityRequest(PlayFab::GroupsModels::BlockEntityRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"93402701D5183557\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    void AutoGenGroupsTests::FillBlockEntityCleanupUnblockEntityRequest(PlayFab::GroupsModels::UnblockEntityRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"93402701D5183557\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region ChangeMemberRole

    void AutoGenGroupsTests::FillChangeMemberRolePrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreChangeMemberRolePrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillChangeMemberRoleRequest(PlayFab::GroupsModels::ChangeMemberRoleRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"OriginRoleId\": \"members\", \"DestinationRoleId\": \"testrole\", \"Members\": [ {  \"Id\": \"6037570B9CA839BF\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    void AutoGenGroupsTests::FillChangeMemberRoleCleanupChangeMemberRoleRequest(PlayFab::GroupsModels::ChangeMemberRoleRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"OriginRoleId\": \"testrole\", \"DestinationRoleId\": \"members\", \"Members\": [ {  \"Id\": \"6037570B9CA839BF\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region CreateGroup

    void AutoGenGroupsTests::FillCreateGroupRequest(PlayFab::GroupsModels::CreateGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Example Group\"}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsCreateGroupResponse(PFGroupsCreateGroupResponse* result)
    {
        // result.adminRoleId = const char*;
        // result.created = time_t;
        // result.group = PFEntityKey const*;
        // result.groupName = const char*;
        // result.memberRoleId = const char*;
        // result.profileVersion = int32_t;
        // result.roles = struct PFStringDictionaryEntry const*;
        // result.rolesCount = uint32_t;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

    void AutoGenGroupsTests::FillCreateGroupCleanupGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Example Group\"}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreCreateGroupCleanupPFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillCreateGroupCleanupDeleteGroupRequest(PlayFab::GroupsModels::DeleteGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region CreateRole

    void AutoGenGroupsTests::FillCreateRolePrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreCreateRolePrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillCreateGroupRoleRequest(PlayFab::GroupsModels::CreateGroupRoleRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"RoleId\": \"example\", \"RoleName\": \"Example Role\", \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsCreateGroupRoleResponse(PFGroupsCreateGroupRoleResponse* result)
    {
        // result.profileVersion = int32_t;
        // result.roleId = const char*;
        // result.roleName = const char*;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

    void AutoGenGroupsTests::FillCreateRoleCleanupDeleteRoleRequest(PlayFab::GroupsModels::DeleteRoleRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"example\"}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region DeleteGroup

    void AutoGenGroupsTests::FillDeleteGroupPrerequisiteCreateGroupRequest(PlayFab::GroupsModels::CreateGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Example Group2\"}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreDeleteGroupPrerequisitePFGroupsCreateGroupResponse(PFGroupsCreateGroupResponse* result)
    {
        PlayFab::GroupsModels::CreateGroupResponse internalResult(*result);
        testData.m_CreateGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillDeleteGroupRequest(PlayFab::GroupsModels::DeleteGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_CreateGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region DeleteRole

    void AutoGenGroupsTests::FillDeleteRolePrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreDeleteRolePrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillDeleteRolePrerequisiteCreateGroupRoleRequest(PlayFab::GroupsModels::CreateGroupRoleRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"RoleId\": \"example\", \"RoleName\": \"Example Role\", \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreDeleteRolePrerequisitePFGroupsCreateGroupRoleResponse(PFGroupsCreateGroupRoleResponse* result)
    {
        PlayFab::GroupsModels::CreateGroupRoleResponse internalResult(*result);
        testData.m_CreateGroupRoleResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillDeleteRoleRequest(PlayFab::GroupsModels::DeleteRoleRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"example\"}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region GetGroup

    void AutoGenGroupsTests::FillGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        // result.adminRoleId = const char*;
        // result.created = time_t;
        // result.group = PFEntityKey const*;
        // result.groupName = const char*;
        // result.memberRoleId = const char*;
        // result.profileVersion = int32_t;
        // result.roles = struct PFStringDictionaryEntry const*;
        // result.rolesCount = uint32_t;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

#pragma endregion

#pragma region InviteToGroup

    void AutoGenGroupsTests::FillInviteToGroupPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreInviteToGroupPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillInviteToGroupRequest(PlayFab::GroupsModels::InviteToGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"33D5E873FB4D8629\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsInviteToGroupResponse(PFGroupsInviteToGroupResponse* result)
    {
        // result.expires = time_t;
        // result.group = PFEntityKey const*;
        // result.invitedByEntity = PFGroupsEntityWithLineage const*;
        // result.invitedEntity = PFGroupsEntityWithLineage const*;
        // result.roleId = const char*;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

    void AutoGenGroupsTests::FillInviteToGroupCleanupRemoveGroupInvitationRequest(PlayFab::GroupsModels::RemoveGroupInvitationRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"33D5E873FB4D8629\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region IsMember

    void AutoGenGroupsTests::FillIsMemberPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreIsMemberPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillIsMemberRequest(PlayFab::GroupsModels::IsMemberRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"B64BE91E5DBD5597\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsIsMemberResponse(PFGroupsIsMemberResponse* result)
    {
        // result.isMember = bool;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

#pragma endregion

#pragma region ListGroupApplications

    void AutoGenGroupsTests::FillListGroupApplicationsPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreListGroupApplicationsPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillListGroupApplicationsRequest(PlayFab::GroupsModels::ListGroupApplicationsRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsListGroupApplicationsResponse(PFGroupsListGroupApplicationsResponse* result)
    {
        // result.applications = PFGroupsGroupApplication const* const*;
        // result.applicationsCount = uint32_t;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

#pragma endregion

#pragma region ListGroupBlocks

    void AutoGenGroupsTests::FillListGroupBlocksPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreListGroupBlocksPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillListGroupBlocksRequest(PlayFab::GroupsModels::ListGroupBlocksRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsListGroupBlocksResponse(PFGroupsListGroupBlocksResponse* result)
    {
        // result.blockedEntities = PFGroupsGroupBlock const* const*;
        // result.blockedEntitiesCount = uint32_t;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

#pragma endregion

#pragma region ListGroupInvitations

    void AutoGenGroupsTests::FillListGroupInvitationsPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreListGroupInvitationsPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillListGroupInvitationsRequest(PlayFab::GroupsModels::ListGroupInvitationsRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsListGroupInvitationsResponse(PFGroupsListGroupInvitationsResponse* result)
    {
        // result.invitations = PFGroupsGroupInvitation const* const*;
        // result.invitationsCount = uint32_t;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

#pragma endregion

#pragma region ListGroupMembers

    void AutoGenGroupsTests::FillListGroupMembersPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreListGroupMembersPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillListGroupMembersRequest(PlayFab::GroupsModels::ListGroupMembersRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsListGroupMembersResponse(PFGroupsListGroupMembersResponse* result)
    {
        // result.members = PFGroupsEntityMemberRole const* const*;
        // result.membersCount = uint32_t;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

#pragma endregion

#pragma region ListMembership

    void AutoGenGroupsTests::FillListMembershipPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreListMembershipPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillListMembershipRequest(PlayFab::GroupsModels::ListMembershipRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsListMembershipResponse(PFGroupsListMembershipResponse* result)
    {
        // result.groups = PFGroupsGroupWithRoles const* const*;
        // result.groupsCount = uint32_t;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

#pragma endregion

#pragma region ListMembershipOpportunities

    void AutoGenGroupsTests::FillListMembershipOpportunitiesPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreListMembershipOpportunitiesPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillListMembershipOpportunitiesPrerequisiteApplyToGroupRequest(PlayFab::GroupsModels::ApplyToGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreListMembershipOpportunitiesPrerequisitePFGroupsApplyToGroupResponse(PFGroupsApplyToGroupResponse* result)
    {
        PlayFab::GroupsModels::ApplyToGroupResponse internalResult(*result);
        testData.m_ApplyToGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillListMembershipOpportunitiesRequest(PlayFab::GroupsModels::ListMembershipOpportunitiesRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsListMembershipOpportunitiesResponse(PFGroupsListMembershipOpportunitiesResponse* result)
    {
        // result.applications = PFGroupsGroupApplication const* const*;
        // result.applicationsCount = uint32_t;
        // result.invitations = PFGroupsGroupInvitation const* const*;
        // result.invitationsCount = uint32_t;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

    void AutoGenGroupsTests::FillListMembershipOpportunitiesCleanupRemoveGroupApplicationRequest(PlayFab::GroupsModels::RemoveGroupApplicationRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        inputJson["Entity"]["Id"].SetString(testData.m_ApplyToGroupResponse.entity->key->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region RemoveGroupApplication

    void AutoGenGroupsTests::FillRemoveGroupApplicationPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreRemoveGroupApplicationPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillRemoveGroupApplicationPrerequisiteApplyToGroupRequest(PlayFab::GroupsModels::ApplyToGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreRemoveGroupApplicationPrerequisitePFGroupsApplyToGroupResponse(PFGroupsApplyToGroupResponse* result)
    {
        PlayFab::GroupsModels::ApplyToGroupResponse internalResult(*result);
        testData.m_ApplyToGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillRemoveGroupApplicationRequest(PlayFab::GroupsModels::RemoveGroupApplicationRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        inputJson["Entity"]["Id"].SetString(testData.m_ApplyToGroupResponse.entity->key->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region RemoveGroupInvitation

    void AutoGenGroupsTests::FillRemoveGroupInvitationPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreRemoveGroupInvitationPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillRemoveGroupInvitationPrerequisiteInviteToGroupRequest(PlayFab::GroupsModels::InviteToGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"33D5E873FB4D8629\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreRemoveGroupInvitationPrerequisitePFGroupsInviteToGroupResponse(PFGroupsInviteToGroupResponse* result)
    {
        PlayFab::GroupsModels::InviteToGroupResponse internalResult(*result);
        testData.m_InviteToGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillRemoveGroupInvitationRequest(PlayFab::GroupsModels::RemoveGroupInvitationRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        inputJson["Entity"]["Id"].SetString(testData.m_InviteToGroupResponse.invitedEntity->key->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region RemoveMembers

    void AutoGenGroupsTests::FillRemoveMembersPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreRemoveMembersPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillRemoveMembersPrerequisiteAddMembersRequest(PlayFab::GroupsModels::AddMembersRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Members\": [ {  \"Id\": \"FED18FB0293BA445\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    void AutoGenGroupsTests::FillRemoveMembersRequest(PlayFab::GroupsModels::RemoveMembersRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Members\": [ {  \"Id\": \"FED18FB0293BA445\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region UnblockEntity

    void AutoGenGroupsTests::FillUnblockEntityPrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreUnblockEntityPrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillUnblockEntityPrerequisiteBlockEntityRequest(PlayFab::GroupsModels::BlockEntityRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"93402701D5183557\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    void AutoGenGroupsTests::FillUnblockEntityRequest(PlayFab::GroupsModels::UnblockEntityRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"93402701D5183557\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region UpdateGroup

    void AutoGenGroupsTests::FillUpdateGroupPrerequisiteCreateGroupRequest(PlayFab::GroupsModels::CreateGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Example Group\"}");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreUpdateGroupPrerequisitePFGroupsCreateGroupResponse(PFGroupsCreateGroupResponse* result)
    {
        PlayFab::GroupsModels::CreateGroupResponse internalResult(*result);
        testData.m_CreateGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillUpdateGroupRequest(PlayFab::GroupsModels::UpdateGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"GroupName\": \"My New Group Name\" }");
        inputJson["Group"]["Id"].SetString(testData.m_CreateGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsUpdateGroupResponse(PFGroupsUpdateGroupResponse* result)
    {
        // result.operationReason = const char*;
        // result.profileVersion = int32_t;
        // result.setResult = PFOperationTypes const*;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

    void AutoGenGroupsTests::FillUpdateGroupCleanupDeleteGroupRequest(PlayFab::GroupsModels::DeleteGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
        inputJson["Group"]["Id"].SetString(testData.m_CreateGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

#pragma endregion

#pragma region UpdateRole

    void AutoGenGroupsTests::FillUpdateRolePrerequisiteGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"GroupName\": \"Permanent Group\" }");
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreUpdateRolePrerequisitePFGroupsGetGroupResponse(PFGroupsGetGroupResponse* result)
    {
        PlayFab::GroupsModels::GetGroupResponse internalResult(*result);
        testData.m_GetGroupResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillUpdateRolePrerequisiteUpdateGroupRoleRequest(PlayFab::GroupsModels::UpdateGroupRoleRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"testrole\", \"RoleName\": \"Prereq Role\" }");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::StoreUpdateRolePrerequisitePFGroupsUpdateGroupRoleResponse(PFGroupsUpdateGroupRoleResponse* result)
    {
        PlayFab::GroupsModels::UpdateGroupRoleResponse internalResult(*result);
        testData.m_UpdateGroupRoleResponse.FromJson(internalResult.ToJson());

        return S_OK;
    }

    void AutoGenGroupsTests::FillUpdateGroupRoleRequest(PlayFab::GroupsModels::UpdateGroupRoleRequest* request)
    {
        PlayFab::JsonDocument inputJson;
        inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"testrole\", \"RoleName\": \"Test Role\" }");
        inputJson["Group"]["Id"].SetString(testData.m_GetGroupResponse.group->id, inputJson.GetAllocator());
        request->FromJson(inputJson);
    }

    HRESULT AutoGenGroupsTests::ValidatePFGroupsUpdateGroupRoleResponse(PFGroupsUpdateGroupRoleResponse* result)
    {
        // result.operationReason = const char*;
        // result.profileVersion = int32_t;
        // result.setResult = PFOperationTypes const*;

        UNREFERENCED_PARAMETER(result);
        return S_OK;
    }

#pragma endregion


}

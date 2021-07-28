#include "TestAppPch.h"
#include "TestContext.h"
#include "TestApp.h"
#include "AutoGenGroupsTests.h"
#include "XAsyncHelper.h"
#include <playfab/PlayFabClientAuthApi.h>
#include <playfab/PlayFabClientApi.h>
#include <playfab/PlayFabProfilesApi.h>
#include <playfab/PlayFabAdminApi.h>
#include <playfab/PlayFabAuthenticationAuthApi.h>
#include <playfab/PlayFabClientDataModels.h>

namespace PlayFabUnit
{

void AutoGenGroupsTests::FillPlayFabPrerequisiteGroupsCreateGroupRequest(PlayFab::GroupsModels::CreateGroupRequest* request)
{
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"GroupName\": \"Prerequisite Group\"}");
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::StorePlayFabPrerequisiteGroupsCreateGroupResponse(PlayFabGroupsCreateGroupResponse* result)
{
    // TODO: store required fields
    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabPrerequisiteGroupsGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
{
    // TODO: debug Failing prereq
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"GroupName\": \"Prerequisite Group\"}");
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::StorePlayFabPrerequisiteGroupsGetGroupResponse(PlayFabGroupsGetGroupResponse* result)
{
    // TODO: store required fields
    Data()->m_groupId = result->group->id;

    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabPrerequisiteGroupsApplyToGroupRequest(PlayFab::GroupsModels::ApplyToGroupRequest* request)
{
    // TODO: debug Failing prereq
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"506624566823DCEE\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::StorePlayFabPrerequisiteGroupsApplyToGroupResponse(PlayFabGroupsApplyToGroupResponse* result)
{
    // TODO: store required fields
    //Data()->m_groupApplyingEntityId = result->entity->key->id;

    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabPrerequisiteGroupsInviteToGroupRequest(PlayFab::GroupsModels::InviteToGroupRequest* request)
{
    // TODO: debug Failing prereq
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"members\", \"AutoAcceptOutstandingApplication\": false, \"Entity\": { \"Id\": \"506624566823DCEE\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::StorePlayFabPrerequisiteGroupsInviteToGroupResponse(PlayFabGroupsInviteToGroupResponse* result)
{
    // TODO: store required fields
    //Data()->m_expires = result->expires;
    //Data()->m_group = result->group;
    //Data()->m_invitedByEntity = result->invitedByEntity;
    //Data()->m_invitedEntity = result->invitedEntity;
    //Data()->m_roleId = result->roleId;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabPrerequisiteGroupsBlockEntityRequest(PlayFab::GroupsModels::BlockEntityRequest* request)
{
    // TODO: debug Failing prereq
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"FED18FB0293BA445\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}


void AutoGenGroupsTests::FillPlayFabGroupsAcceptGroupApplicationRequest(PlayFab::GroupsModels::AcceptGroupApplicationRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsAcceptGroupInvitationRequest(PlayFab::GroupsModels::AcceptGroupInvitationRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsAddMembersRequest(PlayFab::GroupsModels::AddMembersRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Members\": [ {  \"Id\": \"90901000\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsApplyToGroupRequest(PlayFab::GroupsModels::ApplyToGroupRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsApplyToGroupResponse(PlayFabGroupsApplyToGroupResponse* result)
{
    // result.entity = PlayFabGroupsEntityWithLineage const*;
    // result.expires = time_t;
    // result.group = PlayFabEntityKey const*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsBlockEntityRequest(PlayFab::GroupsModels::BlockEntityRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsChangeMemberRoleRequest(PlayFab::GroupsModels::ChangeMemberRoleRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"OriginRoleId\": \"awesomepeople\", \"DestinationRoleId\": \"members\", \"Members\": [ {  \"Id\": \"90901000\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsCreateGroupRequest(PlayFab::GroupsModels::CreateGroupRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"GroupName\": \"Example Group\"}");
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsCreateGroupResponse(PlayFabGroupsCreateGroupResponse* result)
{
    // result.adminRoleId = const char*;
    // result.created = time_t;
    // result.group = PlayFabEntityKey const*;
    // result.groupName = const char*;
    // result.memberRoleId = const char*;
    // result.profileVersion = int32_t;
    // result.roles = struct PlayFabStringDictionaryEntry const*;
    // result.rolesCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsCreateGroupRoleRequest(PlayFab::GroupsModels::CreateGroupRoleRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"RoleId\": \"example\", \"RoleName\": \"Example Role\", \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsCreateGroupRoleResponse(PlayFabGroupsCreateGroupRoleResponse* result)
{
    // result.profileVersion = int32_t;
    // result.roleId = const char*;
    // result.roleName = const char*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsDeleteGroupRequest(PlayFab::GroupsModels::DeleteGroupRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsDeleteRoleRequest(PlayFab::GroupsModels::DeleteRoleRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"ABC1234DEF\"}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsGetGroupRequest(PlayFab::GroupsModels::GetGroupRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsGetGroupResponse(PlayFabGroupsGetGroupResponse* result)
{
    // result.adminRoleId = const char*;
    // result.created = time_t;
    // result.group = PlayFabEntityKey const*;
    // result.groupName = const char*;
    // result.memberRoleId = const char*;
    // result.profileVersion = int32_t;
    // result.roles = struct PlayFabStringDictionaryEntry const*;
    // result.rolesCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsInviteToGroupRequest(PlayFab::GroupsModels::InviteToGroupRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"awesomepeople\", \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsInviteToGroupResponse(PlayFabGroupsInviteToGroupResponse* result)
{
    // result.expires = time_t;
    // result.group = PlayFabEntityKey const*;
    // result.invitedByEntity = PlayFabGroupsEntityWithLineage const*;
    // result.invitedEntity = PlayFabGroupsEntityWithLineage const*;
    // result.roleId = const char*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsIsMemberRequest(PlayFab::GroupsModels::IsMemberRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"ABC1234DEF\", \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsIsMemberResponse(PlayFabGroupsIsMemberResponse* result)
{
    // result.isMember = bool;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsListGroupApplicationsRequest(PlayFab::GroupsModels::ListGroupApplicationsRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsListGroupApplicationsResponse(PlayFabGroupsListGroupApplicationsResponse* result)
{
    // result.applications = PlayFabGroupsGroupApplication const* const*;
    // result.applicationsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsListGroupBlocksRequest(PlayFab::GroupsModels::ListGroupBlocksRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsListGroupBlocksResponse(PlayFabGroupsListGroupBlocksResponse* result)
{
    // result.blockedEntities = PlayFabGroupsGroupBlock const* const*;
    // result.blockedEntitiesCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsListGroupInvitationsRequest(PlayFab::GroupsModels::ListGroupInvitationsRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsListGroupInvitationsResponse(PlayFabGroupsListGroupInvitationsResponse* result)
{
    // result.invitations = PlayFabGroupsGroupInvitation const* const*;
    // result.invitationsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsListGroupMembersRequest(PlayFab::GroupsModels::ListGroupMembersRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsListGroupMembersResponse(PlayFabGroupsListGroupMembersResponse* result)
{
    // result.members = PlayFabGroupsEntityMemberRole const* const*;
    // result.membersCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsListMembershipRequest(PlayFab::GroupsModels::ListMembershipRequest* request)
{
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{}");
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsListMembershipResponse(PlayFabGroupsListMembershipResponse* result)
{
    // result.groups = PlayFabGroupsGroupWithRoles const* const*;
    // result.groupsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsListMembershipOpportunitiesRequest(PlayFab::GroupsModels::ListMembershipOpportunitiesRequest* request)
{
    // TODO: debug PassingButNoData test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{}");
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsListMembershipOpportunitiesResponse(PlayFabGroupsListMembershipOpportunitiesResponse* result)
{
    // result.applications = PlayFabGroupsGroupApplication const* const*;
    // result.applicationsCount = uint32_t;
    // result.invitations = PlayFabGroupsGroupInvitation const* const*;
    // result.invitationsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsRemoveGroupApplicationRequest(PlayFab::GroupsModels::RemoveGroupApplicationRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsRemoveGroupInvitationRequest(PlayFab::GroupsModels::RemoveGroupInvitationRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsRemoveMembersRequest(PlayFab::GroupsModels::RemoveMembersRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"ABC1234DEF\", \"Members\": [ {  \"Id\": \"90901000\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsUnblockEntityRequest(PlayFab::GroupsModels::UnblockEntityRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillPlayFabGroupsUpdateGroupRequest(PlayFab::GroupsModels::UpdateGroupRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"GroupName\": \"My New Group Name\", \"ExpectedProfileVersion\": 17}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsUpdateGroupResponse(PlayFabGroupsUpdateGroupResponse* result)
{
    // result.operationReason = const char*;
    // result.profileVersion = int32_t;
    // result.setResult = PlayFabGroupsOperationTypes const*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPlayFabGroupsUpdateGroupRoleRequest(PlayFab::GroupsModels::UpdateGroupRoleRequest* request)
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"ABC1234DEF\", \"RoleName\": \"My New Role Name\", \"ExpectedProfileVersion\": 17}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePlayFabGroupsUpdateGroupRoleResponse(PlayFabGroupsUpdateGroupRoleResponse* result)
{
    // result.operationReason = const char*;
    // result.profileVersion = int32_t;
    // result.setResult = PlayFabGroupsOperationTypes const*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}



}

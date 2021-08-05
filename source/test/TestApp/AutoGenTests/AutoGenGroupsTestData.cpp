#include "TestAppPch.h"
#include "TestContext.h"
#include "TestApp.h"
#include "AutoGenGroupsTests.h"
#include "XAsyncHelper.h"

namespace PlayFabUnit
{

void AutoGenGroupsTests::FillPrerequisiteCreateGroupRequest( PlayFab::GroupsModels::CreateGroupRequest* request )
{
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"GroupName\": \"Example Group\"}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::StorePrerequisitePFGroupsCreateGroupResponse( PFGroupsCreateGroupResponse* result )
{
    // TODO: store required fields
    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPrerequisiteGetGroupRequest( PlayFab::GroupsModels::GetGroupRequest* request )
{
    // TODO: debug Failing prereq
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"GroupName\": \"Prerequisite Group\"}");
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::StorePrerequisitePFGroupsGetGroupResponse( PFGroupsGetGroupResponse* result )
{
    // TODO: store required fields
    Data()->m_groupId = result->group->id;

    return S_OK;
}

void AutoGenGroupsTests::FillPrerequisiteApplyToGroupRequest( PlayFab::GroupsModels::ApplyToGroupRequest* request )
{
    // TODO: debug Failing prereq
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"506624566823DCEE\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::StorePrerequisitePFGroupsApplyToGroupResponse( PFGroupsApplyToGroupResponse* result )
{
    // TODO: store required fields
    //Data()->m_groupApplyingEntityId = result->entity->key->id;
    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillPrerequisiteInviteToGroupRequest( PlayFab::GroupsModels::InviteToGroupRequest* request )
{
    // TODO: debug Failing prereq
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"members\", \"AutoAcceptOutstandingApplication\": false, \"Entity\": { \"Id\": \"506624566823DCEE\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::StorePrerequisitePFGroupsInviteToGroupResponse( PFGroupsInviteToGroupResponse* result )
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

void AutoGenGroupsTests::FillPrerequisiteBlockEntityRequest( PlayFab::GroupsModels::BlockEntityRequest* request )
{
    // TODO: debug Failing prereq
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"FED18FB0293BA445\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

 
void AutoGenGroupsTests::FillSetPublisherDataRequest( PlayFab::SetPublisherDataRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"Key\": \"Running Speed\", \"Value\": \"1.03\"}" );
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillAddSharedGroupMembersRequest( PlayFab::GroupsModels::AddSharedGroupMembersRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"SharedGroupId\": \"Clan Data\", \"PlayFabIds\": [ \"D984A64B832\", \"F74A523E1562\" ]}" );
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillCreateSharedGroupRequest( PlayFab::GroupsModels::CreateSharedGroupRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"SharedGroupId\": \"Clan Data\"}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsCreateSharedGroupResult( PFGroupsCreateSharedGroupResult* result )
{    // result.sharedGroupId = const char*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillGetSharedGroupDataRequest( PlayFab::GroupsModels::GetSharedGroupDataRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"SharedGroupId\": \"Clan Data\", \"Keys\": [ \"ClanKills\", \"LastClanUpdate\" ], \"GetMembers\": true}" );
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsGetSharedGroupDataResult( PFGroupsGetSharedGroupDataResult* result )
{    // result.data = struct PFGroupsSharedGroupDataRecordDictionaryEntry const*;
    // result.dataCount = uint32_t;
    // result.members = const char* const*;
    // result.membersCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillRemoveSharedGroupMembersRequest( PlayFab::GroupsModels::RemoveSharedGroupMembersRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"SharedGroupId\": \"Clan Data\", \"PlayFabIds\": [ \"D984A64B832\", \"F74A523E1562\" ]}" );
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillUpdateSharedGroupDataRequest( PlayFab::GroupsModels::UpdateSharedGroupDataRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"SharedGroupId\": \"Clan Data\", \"Data\": { \"ClanKills\": \"34\", \"LastClanUpdate\": \"2014-10-03T09:21:14Z\" }, \"Permission\": \"Public\"}" );
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillDeleteSharedGroupRequest( PlayFab::GroupsModels::DeleteSharedGroupRequest* request )
{
    // TODO: debug  test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse( "{ \"SharedGroupId\": \"Clan Data\"}" );
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillAcceptGroupApplicationRequest( PlayFab::GroupsModels::AcceptGroupApplicationRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillAcceptGroupInvitationRequest( PlayFab::GroupsModels::AcceptGroupInvitationRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillAddMembersRequest( PlayFab::GroupsModels::AddMembersRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Members\": [ {  \"Id\": \"90901000\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillApplyToGroupRequest( PlayFab::GroupsModels::ApplyToGroupRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsApplyToGroupResponse( PFGroupsApplyToGroupResponse* result )
{
    // result.entity = PlayFabGroupsEntityWithLineage const*;
    // result.expires = time_t;
    // result.group = PlayFabEntityKey const*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillBlockEntityRequest( PlayFab::GroupsModels::BlockEntityRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillChangeMemberRoleRequest( PlayFab::GroupsModels::ChangeMemberRoleRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"OriginRoleId\": \"awesomepeople\", \"DestinationRoleId\": \"members\", \"Members\": [ {  \"Id\": \"90901000\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillCreateGroupRequest( PlayFab::GroupsModels::CreateGroupRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"GroupName\": \"Example Group\"}");
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsCreateGroupResponse( PFGroupsCreateGroupResponse* result )
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

void AutoGenGroupsTests::FillCreateGroupRoleRequest( PlayFab::GroupsModels::CreateGroupRoleRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"RoleId\": \"example\", \"RoleName\": \"Example Role\", \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsCreateGroupRoleResponse( PFGroupsCreateGroupRoleResponse* result )
{
    // result.profileVersion = int32_t;
    // result.roleId = const char*;
    // result.roleName = const char*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillDeleteGroupRequest( PlayFab::GroupsModels::DeleteGroupRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillDeleteRoleRequest( PlayFab::GroupsModels::DeleteRoleRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"ABC1234DEF\"}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillGetGroupRequest( PlayFab::GroupsModels::GetGroupRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsGetGroupResponse( PFGroupsGetGroupResponse* result )
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

void AutoGenGroupsTests::FillInviteToGroupRequest( PlayFab::GroupsModels::InviteToGroupRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"awesomepeople\", \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsInviteToGroupResponse( PFGroupsInviteToGroupResponse* result )
{
    // result.expires = time_t;
    // result.group = PlayFabEntityKey const*;
    // result.invitedByEntity = PlayFabGroupsEntityWithLineage const*;
    // result.invitedEntity = PlayFabGroupsEntityWithLineage const*;
    // result.roleId = const char*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillIsMemberRequest( PlayFab::GroupsModels::IsMemberRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"ABC1234DEF\", \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsIsMemberResponse( PFGroupsIsMemberResponse* result )
{
    // result.isMember = bool;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillListGroupApplicationsRequest( PlayFab::GroupsModels::ListGroupApplicationsRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsListGroupApplicationsResponse( PFGroupsListGroupApplicationsResponse* result )
{
    // result.applications = PlayFabGroupsGroupApplication const* const*;
    // result.applicationsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillListGroupBlocksRequest( PlayFab::GroupsModels::ListGroupBlocksRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsListGroupBlocksResponse( PFGroupsListGroupBlocksResponse* result )
{
    // result.blockedEntities = PlayFabGroupsGroupBlock const* const*;
    // result.blockedEntitiesCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillListGroupInvitationsRequest( PlayFab::GroupsModels::ListGroupInvitationsRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsListGroupInvitationsResponse( PFGroupsListGroupInvitationsResponse* result )
{
    // result.invitations = PlayFabGroupsGroupInvitation const* const*;
    // result.invitationsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillListGroupMembersRequest( PlayFab::GroupsModels::ListGroupMembersRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsListGroupMembersResponse( PFGroupsListGroupMembersResponse* result )
{
    // result.members = PlayFabGroupsEntityMemberRole const* const*;
    // result.membersCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillListMembershipRequest( PlayFab::GroupsModels::ListMembershipRequest* request )
{
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{}");
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsListMembershipResponse( PFGroupsListMembershipResponse* result )
{
    // result.groups = PlayFabGroupsGroupWithRoles const* const*;
    // result.groupsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillListMembershipOpportunitiesRequest( PlayFab::GroupsModels::ListMembershipOpportunitiesRequest* request )
{
    // TODO: debug PassingButNoData test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{}");
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsListMembershipOpportunitiesResponse( PFGroupsListMembershipOpportunitiesResponse* result )
{
    // result.applications = PlayFabGroupsGroupApplication const* const*;
    // result.applicationsCount = uint32_t;
    // result.invitations = PlayFabGroupsGroupInvitation const* const*;
    // result.invitationsCount = uint32_t;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillRemoveGroupApplicationRequest( PlayFab::GroupsModels::RemoveGroupApplicationRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillRemoveGroupInvitationRequest( PlayFab::GroupsModels::RemoveGroupInvitationRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillRemoveMembersRequest( PlayFab::GroupsModels::RemoveMembersRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"ABC1234DEF\", \"Members\": [ {  \"Id\": \"90901000\",  \"Type\": \"title_player_account\",  \"TypeString\": \"title_player_account\" } ]}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillUnblockEntityRequest( PlayFab::GroupsModels::UnblockEntityRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"Entity\": { \"Id\": \"90901000\", \"Type\": \"title_player_account\", \"TypeString\": \"title_player_account\" }}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

void AutoGenGroupsTests::FillUpdateGroupRequest( PlayFab::GroupsModels::UpdateGroupRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"GroupName\": \"My New Group Name\", \"ExpectedProfileVersion\": 17}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsUpdateGroupResponse( PFGroupsUpdateGroupResponse* result )
{
    // result.operationReason = const char*;
    // result.profileVersion = int32_t;
    // result.setResult = PlayFabGroupsOperationTypes const*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

void AutoGenGroupsTests::FillUpdateGroupRoleRequest( PlayFab::GroupsModels::UpdateGroupRoleRequest* request )
{
    // TODO: debug Failing test
    PlayFab::JsonDocument inputJson;
    inputJson.Parse("{ \"Group\": { \"Id\": \"ABC1234ABC\" }, \"RoleId\": \"ABC1234DEF\", \"RoleName\": \"My New Role Name\", \"ExpectedProfileVersion\": 17}");
    inputJson["Group"]["Id"].SetString(Data()->m_groupId.c_str(), inputJson.GetAllocator());
    request->FromJson(inputJson);
}

HRESULT AutoGenGroupsTests::ValidatePFGroupsUpdateGroupRoleResponse( PFGroupsUpdateGroupRoleResponse* result )
{
    // result.operationReason = const char*;
    // result.profileVersion = int32_t;
    // result.setResult = PlayFabGroupsOperationTypes const*;

    UNREFERENCED_PARAMETER(result);
    return S_OK;
}

 

}

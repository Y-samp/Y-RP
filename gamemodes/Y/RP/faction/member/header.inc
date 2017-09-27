#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI\y_groups>
#include <YSI\y_commands>
#include <YSI\y_dialog>
#include <YSI\y_text>

enum {
	E_FACTION_INVALID_PLAYER = 1,
	E_FACTION_PLAYER_IN_ANY_FACTION,
	E_FACTION_PLAYER_ALREADY_ADDED
}

enum E_FACTION_MEMBER {
	DBID:E_FACTION_Member_player_id,
	E_FACTION_Member_player,
	E_FACTION_Member_player_name[MAX_PLAYER_NAME + 1],
	E_FACTION_Member_rank
}

new FactionMember[Y_RP_FACTION_LIMIT][Y_RP_FACTION_MEMBER_LIMIT][E_FACTION_MEMBER];
new Iterator:FactionMember[Y_RP_FACTION_LIMIT]<Y_RP_FACTION_MEMBER_LIMIT>;
new Iterator:FactionLeader<Y_RP_FACTION_LIMIT>;
new Iterator:OnlineFactionMember[Y_RP_FACTION_LIMIT]<Y_RP_FACTION_MEMBER_LIMIT>;

new Group:AllFactionGroup;
new Group:FactionLeaderGroup;
new Group:FactionGroup[Y_RP_FACTION_LIMIT];
new Group:FactionRankGroup[Y_RP_FACTION_LIMIT][Y_RP_FACTION_MAX_RANK];

#define FACTION_MEMBER_TABLE "rp_faction_members"

#define IsFactionLeader(%0) Group_GetPlayer(FactionLeaderGroup, %0)
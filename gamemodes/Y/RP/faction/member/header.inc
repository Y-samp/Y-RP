#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI\y_groups>
#include <YSI\y_commands>
#include <YSI\y_dialog>
#include <YSI\y_text>

enum E_FACTION_MEMBER {
	DBID:E_FACTION_Member_player_id,
	E_FACTION_Member_player_name[MAX_PLAYER_NAME + 1],
	E_FACTION_Member_rank
}

new FactionMember[Y_RP_FACTION_LIMIT][Y_RP_FACTION_MEMBER_LIMIT][E_FACTION_MEMBER];
new Iterator:FactionMember[Y_RP_FACTION_LIMIT]<Y_RP_FACTION_MEMBER_LIMIT>;
new Iterator:OnlineFactionMember[Y_RP_FACTION_LIMIT]<Y_RP_FACTION_MEMBER_LIMIT>;

#define FACTION_MEMBER_TABLE "faction_members"
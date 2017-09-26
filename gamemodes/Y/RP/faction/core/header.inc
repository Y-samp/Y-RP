#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI\y_groups>
#include <YSI\y_commands>
#include <YSI\y_dialog>

#define Faction. OO_TYPE(FACTION,Faction)

enum {
	E_FACTION_INSPECT_NAME,
	E_FACTION_INSPECT_MONEY,
	E_FACTION_INSPECT_MEMBERS
}

#define INVALID_FACTION_ID 	-1

enum E_FACTION_META {
	DBID:E_FACTION_Meta_id,
	E_FACTION_Meta_name[Y_RP_FACTION_MAX_NAME + 1],
	DBID:E_FACTION_Meta_creator_id,
	E_FACTION_Meta_creator_name[MAX_PLAYER_NAME + 1],
	DBID:E_FACTION_Meta_owner_id,
	E_FACTION_Meta_owner_name[MAX_PLAYER_NAME + 1]
}

new FactionMeta[Y_RP_FACTION_LIMIT][E_FACTION_META];
new Iterator:Faction<Y_RP_FACTION_LIMIT>;

enum E_FACTION_RANK {
	E_FACTION_Rank_name[Y_RP_FACTION_MAX_RANK_NAME + 1],
	E_FACTION_Rank_colour
}

new FactionRank[Y_RP_FACTION_LIMIT][Y_RP_FACTION_MAX_RANK][E_FACTION_RANK];

#define FACTION_TABLE "rp_factions"
#define FACTION_RANK_TABLE "rp_faction_ranks"
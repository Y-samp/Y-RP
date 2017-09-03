#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI\y_groups>
#include <YSI\y_commands>
#include <YSI\y_dialog>

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

#define Faction.%0[%1][%2] Faction%0[%1][E_FACTION_%0_%2]

#define FACTION_TABLE "factions"
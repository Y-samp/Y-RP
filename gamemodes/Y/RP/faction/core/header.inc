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
	E_FACTION_INSPECT_MEMBERS,
	E_FACTION_INSPECT_RANKS,
	E_FACTION_INSPECT_COLOURS
}

#define INVALID_FACTION_ID 	-1

enum E_FACTION_META {
	DBID:E_FACTION_Meta_id,
	E_FACTION_Meta_name[Y_RP_FACTION_MAX_NAME + 1],
	DBID:E_FACTION_Meta_creator_id,
	E_FACTION_Meta_creator_name[MAX_PLAYER_NAME + 1],
	DBID:E_FACTION_Meta_owner_id,
	E_FACTION_Meta_owner_name[MAX_PLAYER_NAME + 1],
	E_FACTION_Meta_colour
}

new FactionMeta[Y_RP_FACTION_LIMIT][E_FACTION_META];
new Iterator:Faction<Y_RP_FACTION_LIMIT>;

#define FACTION_TABLE "rp_factions"

new
	bool:EditingFactionColours[MAX_PLAYERS char],
	EditingFactionColoursFID[MAX_PLAYERS] = { INVALID_FACTION_ID, ... }
;

new Text:ChangeColor[22];

new AllFactionColours[20] = {
	0xE6194BFF, 0x3CB44BFF, 0xFFE119FF, 0x0082C8FF, 0xF58231FF, 0x911EB4FF, 0x46F0F0FF, 0xF032E6FF, 0xD2F53CFF, 0xFABEBEFF,
	0x008080FF, 0xE6BEFFFF, 0xAA6E28FF, 0xFFFAC8FF, 0x800000FF, 0xAAFFC3FF, 0x808000FF, 0xFFD8B1FF, 0x000080FF, 0x808080FF
};
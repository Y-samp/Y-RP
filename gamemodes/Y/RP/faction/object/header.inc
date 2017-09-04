#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI\y_inline>

enum E_FACTION_OBJECT {
	DBID:E_FACTION_Object_id,
	DBID:E_FACTION_Object_faction_id,
	DBID:E_FACTION_Object_object_id,
	E_FACTION_Object_object
}

new FactionObject[Y_RP_FACTION_LIMIT][Y_RP_FACTION_OBJECT_LIMIT][E_FACTION_OBJECT];
new Iterator:FactionObject[Y_RP_FACTION_LIMIT]<Y_RP_FACTION_OBJECT_LIMIT>;

#define FACTION_OBJECT_TABLE "faction_objects"
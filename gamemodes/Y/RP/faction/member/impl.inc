#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_hooks>

loadtext y[core], y_rp_factions[core];

YCMD:factions(playerid, params[], help)
{
	if(help) return SCM(playerid, -1, "Not supported");

	inline Noop()
	{}

	if (Iter_Count(Faction) == 0) {
		log("No factions found");
		return Text_MessageBox(playerid, using inline Noop, $FACTIONS_LIST_EMPTY, $FACTIONS_LIST_EMPTY, $OK, $CLOSE);
	}

	new factions[2048];

	if (IsAdminLevel(playerid, 1)) {
		Text_Format_Other(factions, sizeof factions, Langs_GetPlayerLanguage(playerid), $FACTIONS_LIST_HEADERS_ADMIN, "\t", "\t", "\t");
	} else {
		Text_Format_Other(factions, sizeof factions, Langs_GetPlayerLanguage(playerid), $FACTIONS_LIST_HEADERS, "\t", "\t");
	}

	foreach(new faction:Faction) {
		new
			id = _:Faction.Meta[faction][id],
			numMemebers = Iter_Count(FactionMember[faction]),
			name[Y_RP_FACTION_MAX_NAME],
			ownerName[MAX_PLAYER_NAME + 1],
			money = Faction.Status[faction][money]
		;

		strcpy(name, Faction.Meta[faction][name]);
		strcpy(ownerName, Faction.Meta[faction][owner_name]);

		if (IsAdminLevel(playerid, 1)) {
			format(factions, sizeof factions, "%s\nid: %d) %s\t%d\t%s\t$%d", factions, id, name, numMemebers, ownerName, money);
		} else {
			format(factions, sizeof factions, "%s\nid: %d) %s\t%d\t%s", factions, id, name, numMemebers, ownerName);				
		}
	}

	Text_FormattedTablistHeadersBox(playerid, using inline Noop, $FACTIONS_LIST_TITLE, factions, $OK, $CLOSE);

	return 1;
}
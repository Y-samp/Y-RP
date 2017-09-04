#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_hooks>

loadtext y[core], y_rp_factions[core];

ListFactionMembers(playerid, factionid)
{
	new members[2048];

	Text_Format_Other(members, sizeof members, Langs_GetPlayerLanguage(playerid), $FACTIONS_MEMBERS_LIST_HEADER, "\t", "\t");

	foreach (new member:FactionMember[factionid]) {
		new
			DBID:playerDBid = Faction.Member[factionid][member][@player_id],
			playername[MAX_PLAYER_NAME + 1],
			rank = Faction.Member[factionid][member][@rank],
			online[16]
		;

		strcpy(online, Iter_Contains(OnlineFactionMember[factionid], member) ? "Yes" : "No");
		strcpy(playername, Faction.Member[factionid][member][@player_name]);

		log("WTF %s %s", playername, Faction.Member[factionid][member][@player_name]);

		format(members, sizeof members, "%s\nid: %d) %s\t%d\t%s", members, _:playerDBid, playername, rank, online);
	}

	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if(!response) {
			InspectFaction(playerid, factionid);

			@return 1;
		}

		

		@return 1;
	}

	Text_FormattedTablistHeadersBox(playerid, using inline Response, $FACTIONS_MEMBERS_LIST_TITLE, members, $INSPECT, $BACK);

	return 1;
}
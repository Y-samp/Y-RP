#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_text>

loadtext y[core], y_rp_factions[core];

EditFactionRanks(playerid, factionid)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			@return InspectFaction(playerid, factionid);
		}

		@return 1;
	}

	new ranks[1024];

	for (new i = 0; i != Y_RP_FACTION_MAX_RANK; ++i) {
		format(ranks, sizeof ranks, "%s{%06x}Rank %d: {%06x}%s\n", ranks, GetColour("NORMAL") >>> 8, i + 1, Faction.Rank[factionid][i][@colour], Faction.Rank[factionid][i][@name]);
	}

	return Text_FormattedListBox(playerid, using inline Response, $FACTIONS_RANKS_TITLE, ranks, $OK, $BACK);
}
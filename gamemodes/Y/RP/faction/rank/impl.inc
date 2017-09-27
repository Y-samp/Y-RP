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

		@return InspectFactionRank(playerid, factionid, listitem);
	}

	new ranks[1024];

	for (new i = 0; i != Y_RP_FACTION_MAX_RANK; ++i) {
		format(ranks, sizeof ranks, "%s{%06x}Rank %d: {%06x}%s\n", ranks, GetColour("NORMAL") >>> 8, i + 1, Faction.Rank[factionid][i][@colour], Faction.Rank[factionid][i][@name]);
	}

	return Text_FormattedListBox(playerid, using inline Response, $FACTIONS_RANKS_TITLE, ranks, $OK, $BACK);
}

InspectFactionRank(playerid, factionid, rank)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			@return EditFactionRanks(playerid, factionid);
		}
	
		if (listitem == E_FACTION_INSPECT_RANK_NAME) {
			@return EditFactionRankName(playerid, factionid, rank);
		}	
	
		@return 1;
	}
	
	Text_ListBox(playerid, using inline Response, $FACTIONS_RANK_INSPECT_TITLE, $FACTIONS_RANK_INSPECT_OPTIONS, $SELECT, $BACK);

	return 1;
}

EditFactionRankName(playerid, factionid, rank, error = 0)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			@return InspectFactionRank(playerid, factionid, rank);
		}

		if (isnull(inputtext)) {
			@return EditFactionRankName(playerid, factionid, rank, ERR_RANK_NAME_EMPTY);			
		}

		if (strlen(inputtext) > Y_RP_FACTION_MAX_RANK_NAME) {
			@return EditFactionRankName(playerid, factionid, rank, ERR_RANK_NAME_TOO_LONG);			
		}

		UpdateRankName(factionid, rank, inputtext);

		Text_Send(playerid, $FACTIONS_RANK_CHANGEED_NAME, inputtext);

		@return InspectFactionRank(playerid, factionid, rank);
	}

	new errorText[80];
	if (error == ERR_RANK_NAME_EMPTY) {
		Text_Format_Dialog(errorText, sizeof errorText, Langs_GetPlayerLanguage(playerid), $FACTIONS_RANK_CHANGE_NAME_ERROR_EMPTY);
	} else if (error == ERR_RANK_NAME_TOO_LONG) {
		Text_Format_Dialog(errorText, sizeof errorText, Langs_GetPlayerLanguage(playerid), $FACTIONS_RANK_CHANGE_NAME_ERROR_LONG, Y_RP_FACTION_MAX_RANK_NAME);
	}

	new body[256];
	Text_Format_Dialog(body, sizeof body, Langs_GetPlayerLanguage(playerid), $FACTIONS_RANK_CHANGE_NAME_BODY, errorText, Faction.Rank[factionid][rank][@name]);
	
	Text_FormattedInputBox(playerid, using inline Response, $FACTIONS_RANK_CHANGE_NAME_TITLE, body, $OK, $BACK);

	return 1;
}

UpdateRankName(factionid, rank, name[])
{
	new query[128];
	mysql_format(dbhandle, query, sizeof query, "UPDATE " FACTION_RANK_TABLE " SET name = '%e' WHERE faction_id = %d AND rank = %d", name, _:Faction.Meta[factionid][@id], rank);
	mysql_tquery(dbhandle, query);
}
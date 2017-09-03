#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_hooks>

loadtext y[core], y_rp_factions[core];

ListFactions(playerid)
{
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

	inline Inspect(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if(!response) @return 1;
	
		new
			idx = 0,
			faction = INVALID_FACTION_ID
		;

		foreach (new factionid:Faction) {
			if (idx++ == listitem) {
				faction = factionid;
			}
		}

		if (faction == INVALID_FACTION_ID) {
			err("Something went wrong while inspecting factions by %d", playerid);
		}

		InspectFaction(playerid, faction);
	
		@return 1;
	}
	
	Text_FormattedTablistHeadersBox(playerid, using inline Inspect, $FACTIONS_LIST_TITLE, factions, $INSPECT, $CLOSE);

	return 1;
}

InspectFaction(playerid, factionid)
{
	log("Player %d inspecting faction %d", playerid, factionid);

	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			ListFactions(playerid);

			@return 1;
		}

		switch (listitem) {
			case E_FACTION_INSPECT_NAME: {
				RenameFaction(playerid, factionid);
			}
			case E_FACTION_INSPECT_MONEY: {
				EditFactionMoney(playerid, factionid);
			}
			case E_FACTION_INSPECT_MEMBERS: {
				ListFactionMembers(playerid, factionid);
			}
			default: {
				err("Player %d chose invalid option %d", playerid, listitem);
			}
		}
	
		@return 1;
	}
	
	Text_ListBox(playerid, using inline Response, $FACTIONS_INSPECT_TITLE, $FACTIONS_INSPECT_OPTIONS, $SELECT, $BACK);

	return 1;
}

RenameFaction(playerid, factionid, bool:error = false)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if(!response) {
			InspectFaction(playerid, factionid);
			@return 1;
		}

		if(isnull(inputtext) || strlen(inputtext) >= Y_RP_FACTION_MAX_NAME) {
			RenameFaction(playerid, factionid, true);
			@return 1;
		}
	
		Text_Send(playerid, $FACTIONS_RENAMED, inputtext);
		log("Faction %s renamed to %s", Faction.Meta[factionid][name], inputtext);

		strcpy(Faction.Meta[factionid][name], inputtext);

		new query[60 + Y_RP_FACTION_MAX_NAME + 12 + 1];
		mysql_format(dbhandle, query, sizeof query, "UPDATE " FACTION_TABLE " SET name = '%e' WHERE id = %d", inputtext, _:Faction.Meta[factionid][id]);
		mysql_tquery(dbhandle, query);

		InspectFaction(playerid, factionid);
		@return 1;
	}
	
	new errorText[128];
	if (error) {
		Text_Format_Dialog(errorText, sizeof errorText, Langs_GetPlayerLanguage(playerid), $FACTIONS_RENAME_ERROR, 1, Y_RP_FACTION_MAX_NAME);
	}

	Text_InputBox(playerid, using inline Response, $FACTIONS_RENAME_TITLE, $FACTIONS_RENAME_BODY, $OK, $BACK, errorText, Faction.Meta[factionid][name]);

	return 1;
}

EditFactionMoney(playerid, factionid, bool:error = false)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if(!response) {
			InspectFaction(playerid, factionid);
			@return 1;
		}

		new money = 0;
		if(isnull(inputtext) || sscanf(inputtext, "d", money) || !(Y_RP_FACTION_MAX_DEBT < money < Y_RP_FACTION_MAX_MONEY)) {
			EditFactionMoney(playerid, factionid, true);
			@return 1;
		}
		
		Faction.Status[factionid][money] = money;

		Text_Send(playerid, $FACTIONS_SET_MONEY_DONE, money);
		log("Faction %s money set to %d", Faction.Meta[factionid][name], money);

		new query[60 + Y_RP_FACTION_MAX_NAME + 12 + 1];
		mysql_format(dbhandle, query, sizeof query, "UPDATE " FACTION_TABLE " SET money = %d WHERE id = %d", money, _:Faction.Meta[factionid][id]);
		mysql_tquery(dbhandle, query);

		InspectFaction(playerid, factionid);	
		@return 1;
	}
	
	new errorText[128];
	if (error) {
		Text_Format_Dialog(errorText, sizeof errorText, Langs_GetPlayerLanguage(playerid), $FACTIONS_SET_MONEY_ERROR, Y_RP_FACTION_MAX_DEBT, Y_RP_FACTION_MAX_MONEY);
	}

	Text_InputBox(playerid, using inline Response, $FACTIONS_SET_MONEY_TITLE, $FACTIONS_SET_MONEY_BODY, $OK, $BACK, errorText, Faction.Meta[factionid][name]);

	return 1;
}

ListFactionMembers(playerid, factionid)
{

}


YCMD:factions(playerid, params[], help)
{
	if(help) return SCM(playerid, -1, "Not supported");

	ListFactions(playerid);

	return 1;
}
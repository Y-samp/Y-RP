#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_hooks>

loadtext y[core], y_rp_factions[core];

hook OnGroupInit()
{
	GROUP_ADD<AdminGroup[0]>
	{
		@YCMD:createfaction;
	}

	GROUP_ADD<LoggedGroup>
	{
		@YCMD:factions;
	}
}

hook OnGameModeInit()
{
	inline Handler()
	{
		new
			rows = cache_num_rows(),
			idx = INVALID_ITERATOR_SLOT,
			lastId = 0,
			midx
		;

		for (new row = 0; row != rows; ++row) {
			new id;
			cache_get_value_name_int(row, "id", id);

			if (id != lastId) {
				idx = Iter_Free(Faction);
				if (idx == INVALID_ITERATOR_SLOT) {
					err("Number of factions (%d) is greater than the limit (%d)", rows, Y_RP_FACTION_LIMIT);
					break;
				}

				Faction.Meta[idx][@id] = DBID:id;
				cache_get_value_name(row, "name", Faction.Meta[idx][@name]);
				cache_get_value_name_int(row, "creator_id", _:Faction.Meta[idx][@creator_id]);
				cache_get_value_name(row, "creator", Faction.Meta[idx][@creator_name]);
				cache_get_value_name_int(row, "owner_id", _:Faction.Meta[idx][@owner_id]);
				cache_get_value_name(row, "owner", Faction.Meta[idx][@owner_name]);
				cache_get_value_name_int(row, "money", Faction.Status[idx][@money]);

				RPizeName(Faction.Meta[idx][@creator_name]);
				RPizeName(Faction.Meta[idx][@owner_name]);

				Iter_Add(Faction, idx);
				lastId = id;
			}

			new bool:hasMember;
			cache_is_value_name_null(row, "player_id", hasMember);
			hasMember = !hasMember;

			if (hasMember) {
				midx = Iter_Free(FactionMember[idx]);
				cache_get_value_name_int(row, "player_id", _:Faction.Member[idx][midx][@player_id]);
				cache_get_value_name(row, "member_name", Faction.Member[idx][midx][@player_name]);

				RPizeName(Faction.Member[idx][midx][@player_name]);

				Iter_Add(FactionMember[idx], midx);
			}
		}

		foreach (new faction:Faction) {
			foreach (new member:FactionMember[faction]) {
				printf("Boop %d %s", faction, Faction.Member[faction][member][@player_name]);
			}
		}

		log("Loaded %d factions", Iter_Count(Faction));

		LoadedSystems++;
	}

	new query[512];
	mysql_format(
		dbhandle,
		query,
		sizeof query,
		"SELECT f.*, \
		fm.player_id, fm.rank, p3.name AS member_name, \
		p1.name AS creator, \
		p2.name AS owner \
		FROM " FACTION_TABLE " f \
		LEFT JOIN " FACTION_MEMBER_TABLE " fm ON fm.faction_id = f.id \
		LEFT JOIN " PLAYER_TABLE " p1 ON p1.id = f.creator_id \
		LEFT JOIN " PLAYER_TABLE " p2 ON p2.id = f.owner_id \
		LEFT JOIN " PLAYER_TABLE " p3 ON p3.id = fm.player_id \
		GROUP BY fm.id \
		ORDER BY f.id ASC, fm.id ASC");
	mysql_pquery_inline(dbhandle, query, using inline Handler);
	TotalSystems++;

	return 1;
}

GetFactionIndexFromDBID(DBID:dbid)
{
	foreach (new faction:Faction) {
		if ((DBID:Faction.Meta[faction][@id]) == dbid) {
			return _:faction;
		}
	}

	return INVALID_FACTION_ID;
}

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
			id = _:Faction.Meta[faction][@id],
			numMemebers = Iter_Count(FactionMember[faction]),
			name[Y_RP_FACTION_MAX_NAME],
			ownerName[MAX_PLAYER_NAME + 1],
			money = Faction.Status[faction][@money]
		;

		strcpy(name, Faction.Meta[faction][@name]);
		strcpy(ownerName, Faction.Meta[faction][@owner_name]);

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
		log("Faction %s renamed to %s", Faction.Meta[factionid][@name], inputtext);

		strcpy(Faction.Meta[factionid][@name], inputtext);

		new query[60 + Y_RP_FACTION_MAX_NAME + 12 + 1];
		mysql_format(dbhandle, query, sizeof query, "UPDATE " FACTION_TABLE " SET name = '%e' WHERE id = %d", inputtext, _:Faction.Meta[factionid][@id]);
		mysql_tquery(dbhandle, query);

		InspectFaction(playerid, factionid);
		@return 1;
	}
	
	new errorText[128];
	if (error) {
		Text_Format_Dialog(errorText, sizeof errorText, Langs_GetPlayerLanguage(playerid), $FACTIONS_RENAME_ERROR, 1, Y_RP_FACTION_MAX_NAME);
	}

	Text_InputBox(playerid, using inline Response, $FACTIONS_RENAME_TITLE, $FACTIONS_RENAME_BODY, $OK, $BACK, errorText, Faction.Meta[factionid][@name]);

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
		
		Faction.Status[factionid][@money] = money;

		Text_Send(playerid, $FACTIONS_SET_MONEY_DONE, money);
		log("Faction %s money set to %d", Faction.Meta[factionid][@name], money);

		new query[60 + Y_RP_FACTION_MAX_NAME + 12 + 1];
		mysql_format(dbhandle, query, sizeof query, "UPDATE " FACTION_TABLE " SET money = %d WHERE id = %d", money, _:Faction.Meta[factionid][@id]);
		mysql_tquery(dbhandle, query);

		InspectFaction(playerid, factionid);	
		@return 1;
	}
	
	new errorText[128];
	if (error) {
		Text_Format_Dialog(errorText, sizeof errorText, Langs_GetPlayerLanguage(playerid), $FACTIONS_SET_MONEY_ERROR, Y_RP_FACTION_MAX_DEBT, Y_RP_FACTION_MAX_MONEY);
	}

	Text_InputBox(playerid, using inline Response, $FACTIONS_SET_MONEY_TITLE, $FACTIONS_SET_MONEY_BODY, $OK, $BACK, errorText, Faction.Meta[factionid][@name]);

	return 1;
}

YCMD:factions(playerid, params[], help)
{
	if(help) return SCM(playerid, -1, "Not supported");

	ListFactions(playerid);

	return 1;
}

YCMD:createfaction(playerid, params[], help)
{
	if (help) return SCM(playerid, -1, "Not supported");

	new
		ownerid,
		name[32 + 1],
		budget = 0
	;

	if (sscanf(params, "uds[32]", ownerid, budget, name)) return Text_Send(playerid, $INVALID_COMMAND_PARAMS_CF, YCMD:createfaction);
	if (ownerid == INVALID_PLAYER_ID) return Text_Send(playerid, $INVALID_PLAYER_SPECIFIED);

	new slot = Iter_Free(Faction);
	if (slot == INVALID_ITERATOR_SLOT) return Text_Send(playerid, $FACTIONS_LIMIT_REACHED);

	inline FactionCreated()
	{
		Iter_Add(Faction, slot);
		Faction.Meta[slot][@id] = DBID:cache_insert_id();
	
		new query[128 + 1];
		mysql_format(dbhandle, query, sizeof query, "INSERT INTO faction_members(faction_id, player_id, rank) VALUES(%d, %d, %d)", _:Faction.Meta[slot][@id], _:Player.Auth[ownerid][@id], Y_RP_FACTION_MAX_RANK);
		mysql_pquery(dbhandle, query);
	}

	new query[80 + 32 + 32 + 1];
	mysql_format(dbhandle, query, sizeof query, "INSERT INTO factions(creator_id, owner_id, name, money) VALUES(%d, %d, '%e', %d)", _:Player.Auth[playerid][@id], _:Player.Auth[ownerid][@id], name, budget);
	mysql_pquery_inline(dbhandle, query, using inline FactionCreated);

	Text_Send(playerid, $ACTION_SUCCESS);
	Text_Send(ownerid, $OWNER_FACTION, name, budget);
	Text_Send(ALL_PLAYERS, $NEW_FACTION, name, Player.RP[playerid][@name], Player.RP[ownerid][@name]);

	return 1;
}
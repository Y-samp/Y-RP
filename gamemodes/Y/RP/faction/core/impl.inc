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
			lastMember,
			lastRank = -1,
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
				cache_get_value_name_int(row, "colour", Faction.Meta[idx][@colour]);
				cache_get_value_name_int(row, "money", Faction.Status[idx][@money]);

				RPizeName(Faction.Meta[idx][@creator_name]);
				RPizeName(Faction.Meta[idx][@owner_name]);

				Iter_Add(Faction, idx);
				lastId = id;
			}

			new
				bool:hasMember,
				member
			;
			cache_is_value_name_null(row, "player_id", hasMember);
			hasMember = !hasMember;

			if (hasMember) {
				midx = Iter_Free(FactionMember[idx]);
				
				cache_get_value_name_int(row, "player_id", member);

				if (lastMember != member) {
					lastMember = member;
					
					Faction.Member[idx][midx][@player_id] = DBID:member;

					cache_get_value_name_int(row, "player_id", _:Faction.Member[idx][midx][@player_id]);
					cache_get_value_name(row, "member_name", Faction.Member[idx][midx][@player_name]);
					cache_get_value_name_int(row, "rank", Faction.Member[idx][midx][@rank]);

					RPizeName(Faction.Member[idx][midx][@player_name]);

					Iter_Add(FactionMember[idx], midx);
				}
			}

			new
				bool:hasRank,
				ridx
			;
			cache_is_value_name_null(row, "rank_meta", hasRank);
			hasRank = !hasRank;

			if (hasRank) {
				cache_get_value_name_int(row, "rank_meta", ridx);

				if (lastRank != ridx) {
					lastRank = ridx;

					if (!(0 <= ridx < Y_RP_FACTION_MAX_RANK)) {
						err("Invalid rank (%d) loaded from database for faction %s", ridx, Faction.Meta[idx][@name]);
						continue;
					}

					cache_get_value_name_int(row, "rank_colour", Faction.Rank[idx][ridx][@colour]);
					cache_get_value_name(row, "rank_name", Faction.Rank[idx][ridx][@name]);

				}
			}
		}

		log("Loaded %d factions", Iter_Count(Faction));

		LoadedSystems++;
	}

	new query[640];
	mysql_format(
		dbhandle,
		query,
		sizeof query,
		"SELECT f.*, \
		fm.player_id, fm.rank, p3.name AS member_name, \
		fr.name AS rank_name, fr.rank AS rank_meta, fr.colour AS rank_colour, \
		p1.name AS creator, \
		p2.name AS owner \
		FROM " FACTION_TABLE " f \
		LEFT JOIN " FACTION_MEMBER_TABLE " fm ON fm.faction_id = f.id \
		LEFT JOIN " FACTION_RANK_TABLE " fr ON fr.faction_id = f.id \
		LEFT JOIN " PLAYER_TABLE " p1 ON p1.id = f.creator_id \
		LEFT JOIN " PLAYER_TABLE " p2 ON p2.id = f.owner_id \
		LEFT JOIN " PLAYER_TABLE " p3 ON p3.id = fm.player_id \
		GROUP BY fm.id, fr.id, f.id \
		ORDER BY f.id ASC, fm.id ASC, fr.rank ASC");
	mysql_pquery_inline(dbhandle, query, using inline Handler);
	TotalSystems++;

	CreateColourPicker();

	return 1;
}

hook OnPlayerClickTextDraw(playerid, Text:clickedid)
{
	if (EditingFactionColours{playerid}) {
		if (clickedid == Text:INVALID_TEXT_DRAW || clickedid == ChangeColor[1]) {
			HideColourPicker(playerid);

			InspectFaction(playerid, EditingFactionColoursFID[playerid]);
		} else {
			for (new i = 2; i < sizeof(ChangeColor); i++)	{
				if (clickedid == ChangeColor[i])	{
					HideColourPicker(playerid);

					ChangeFactionColour(playerid, EditingFactionColoursFID[playerid], AllFactionColours[i - 2] >>> 8);
				}
			}
		}

		EditingFactionColours{playerid} = false;
		EditingFactionColoursFID[playerid] = INVALID_FACTION_ID;

		return Y_HOOKS_BREAK_RETURN_1;
	}

	return 0;
}

GetFactionIndexFromDBID(DBID:dbid)
{
	foreach (new faction:Faction) {
		if (Faction.Meta[faction][@id] == dbid) {
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
			money = Faction.Status[faction][@money],
			colour = Faction.Meta[faction][@colour],
			normal = GetColour("NORMAL") >>> 8
		;

		strcpy(name, Faction.Meta[faction][@name]);
		strcpy(ownerName, Faction.Meta[faction][@owner_name]);

		if (IsAdminLevel(playerid, 1)) {
			format(factions, sizeof factions, "%s\nid: %d) {%06x}%s{%06x}\t%d\t%s\t$%d", factions, id, colour, name, normal, numMemebers, ownerName, money);
		} else {
			format(factions, sizeof factions, "%s\nid: %d) {%06x}%s{%06x}\t%d\t%s", factions, id, colour, name, normal, numMemebers, ownerName);				
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

		new bool:authorized = false;

		if (!IsAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL)) {
			if (IsFactionLevel(playerid, faction, Y_RP_FACTION_MANAGE_PLAYER_LEVEL)) {
				authorized = true;
			}

			if (!authorized) {
				Text_Send(playerid, $FACTIONS_LIST_NOT_YOURS);
				@return ListFactions(playerid);
			}
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

		new
			bool:authorizedName = false,
			bool:authorizedMoney = false,
			bool:authorizedRanks = false,
			bool:authorizedColour = false
		;

		if (IsAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL)) {
			authorizedName = true;
			authorizedMoney = true;
			authorizedRanks = true;
			authorizedColour = true;
		}

		if (IsFactionLevel(playerid, factionid, Y_RP_FACTION_CHANGE_NAME_LEVEL)) {
			authorizedName = true;
		}

		if (IsFactionLevel(playerid, factionid, Y_RP_FACTION_MANAGE_RANKS_LEVEL)) {
			authorizedRanks = true;
		}

		if (IsFactionLevel(playerid, factionid, Y_RP_FACTION_CHANGE_COLOUR_LEVEL)) {
			authorizedColour = true;
		}

		// DO NOT ALLOW FACITON TO EDIT ITS OWN MONEY. Seriously, why would you want that
		// if (IsFactionLevel(playerid, factionid, INFINITY)) {
		// 	authorizedMoney = true;
		// }

		switch (listitem) {
			case E_FACTION_INSPECT_NAME: {
				if (authorizedName) {
					@return RenameFaction(playerid, factionid);
				}
			}
			case E_FACTION_INSPECT_MONEY: {
				if (authorizedMoney) {
					@return EditFactionMoney(playerid, factionid);
				}
			}
			case E_FACTION_INSPECT_MEMBERS: {
				@return ListFactionMembers(playerid, factionid);
			}
			case E_FACTION_INSPECT_RANKS: {
				if (authorizedRanks) {
					@return EditFactionRanks(playerid, factionid);
				}
			}
			case E_FACTION_INSPECT_COLOURS: {
				if (authorizedColour) {
					@return EditFactionColour(playerid, factionid);
				}	
			} 
			default: {
				err("Player %d chose invalid option %d", playerid, listitem);
			}
		}

		Text_Send(playerid, $UNAUTHORIZED_ACTION);
	
		@return InspectFaction(playerid, factionid);
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

EditFactionColour(playerid, factionid)
{
	EditingFactionColours{playerid} = true;
	EditingFactionColoursFID[playerid] = factionid;

	ShowColourPicker(playerid);

	return 1;	
}

CreateColourPicker()
{
	new
		inRow = 5,
		Float:tilesize = 40.0,
		Float:padding = 3.0,
		Float:offsetX = (640.0 - inRow * tilesize - (inRow - 1) * padding) / 2,
		Float:offsetY = 138.0,
		Float:X = offsetX,
		Float:Y = offsetY,
		count = 0
	;

	ChangeColor[0] = TextDrawCreate(320.0, offsetY, "box");
	TextDrawLetterSize(ChangeColor[0], 0.0, 19);
	TextDrawTextSize(ChangeColor[0], 0.0, inRow * tilesize + (inRow - 1) * padding);
	TextDrawAlignment(ChangeColor[0], 2);
	TextDrawColor(ChangeColor[0], -1);
	TextDrawUseBox(ChangeColor[0], 1);
	TextDrawBoxColor(ChangeColor[0], 102);
	TextDrawSetOutline(ChangeColor[0], 0);
	TextDrawBackgroundColor(ChangeColor[0], 255);
	TextDrawFont(ChangeColor[0], 1);
	TextDrawSetProportional(ChangeColor[0], 1);
	TextDrawSetShadow(ChangeColor[0], 0);

	ChangeColor[1] = TextDrawCreate(320.0, 360.116699, "Close");
	TextDrawLetterSize(ChangeColor[1], 0.600000, 2.100000);
	TextDrawTextSize(ChangeColor[1], 19.0, 82.327926);
	TextDrawAlignment(ChangeColor[1], 2);
	TextDrawColor(ChangeColor[1], -1);
	TextDrawUseBox(ChangeColor[1], 1);
	TextDrawBoxColor(ChangeColor[1], 102);
	TextDrawSetOutline(ChangeColor[1], 0);
	TextDrawBackgroundColor(ChangeColor[1], 255);
	TextDrawFont(ChangeColor[1], 2);
	TextDrawSetProportional(ChangeColor[1], 1);
	TextDrawSetShadow(ChangeColor[1], 0);
	TextDrawSetSelectable(ChangeColor[1], 1);
	
	for (new i = 2; i < sizeof(ChangeColor); i++)
	{
		ChangeColor[i] = TextDrawCreate(X, Y, "box");
		TextDrawBackgroundColor(ChangeColor[i], AllFactionColours[i-2]);
		TextDrawLetterSize(ChangeColor[i], 0.0, tilesize);
		TextDrawTextSize(ChangeColor[i], tilesize, tilesize);
		TextDrawAlignment(ChangeColor[i], 1);
		TextDrawColor(ChangeColor[i], -1);
		TextDrawUseBox(ChangeColor[i], 1);
		TextDrawBoxColor(ChangeColor[i], 0);
		TextDrawSetOutline(ChangeColor[i], 0);
		TextDrawFont(ChangeColor[i], 5);
		TextDrawSetProportional(ChangeColor[i], 1);
		TextDrawSetShadow(ChangeColor[i], 1);
		TextDrawSetPreviewModel(ChangeColor[i], 19349);
		TextDrawSetPreviewRot(ChangeColor[i], -16.0, 0.0, -180.0, 0.7);
		TextDrawSetSelectable(ChangeColor[i], 1);

		X += tilesize + padding;
		count++;

		if (count == inRow)
		{
			Y += tilesize + padding;
			X = offsetX;
			count = 0;
		}
	}
}

ShowColourPicker(playerid)
{
	for (new i; i < sizeof(ChangeColor); i++) {
		TextDrawShowForPlayer(playerid, ChangeColor[i]);
	}

	SelectTextDraw(playerid, 0xFFFFFF00);
}

HideColourPicker(playerid)
{
	for (new i; i < sizeof(ChangeColor); i++) {
		TextDrawHideForPlayer(playerid, ChangeColor[i]);
	}

	CancelSelectTextDraw(playerid);
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
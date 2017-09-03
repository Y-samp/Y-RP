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

				Faction.Meta[idx][id] = DBID:id;
				cache_get_value_name(row, "name", Faction.Meta[idx][name]);
				cache_get_value_name_int(row, "creator_id", _:Faction.Meta[idx][creator_id]);
				cache_get_value_name(row, "creator", Faction.Meta[idx][creator_name]);
				cache_get_value_name_int(row, "owner_id", _:Faction.Meta[idx][owner_id]);
				cache_get_value_name(row, "owner", Faction.Meta[idx][owner_name]);
				cache_get_value_name_int(row, "money", Faction.Status[idx][money]);

				RPizeName(Faction.Meta[idx][creator_name]);
				RPizeName(Faction.Meta[idx][owner_name]);

				Iter_Add(Faction, idx);
				lastId = id;
			}

			new bool:hasMember = true;
			cache_is_value_name_null(row, "player_id", hasMember);

			if (hasMember) {
				midx = Iter_Free(FactionMember[idx]);
				cache_get_value_name_int(row, "player_id", Faction.Member[idx][player_id][midx]);
				cache_get_value_name(row, "member_name", Faction.Member[idx][player][midx]);
				Iter_Add(FactionMember[idx], midx);
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
		if (Faction.Meta[faction][id] == dbid) {
			return _:faction;
		}
	}

	return INVALID_FACTION_ID;
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
		Faction.Meta[slot][id] = DBID:cache_insert_id();
	
		new query[128 + 1];
		mysql_format(dbhandle, query, sizeof query, "INSERT INTO faction_members(faction_id, player_id, rank) VALUES(%d, %d, %d)", _:Faction.Meta[slot][id], _:Player.Auth[ownerid][id], Y_RP_FACTION_MAX_RANK);
		mysql_pquery(dbhandle, query);
	}

	new query[80 + 32 + 32 + 1];
	mysql_format(dbhandle, query, sizeof query, "INSERT INTO factions(creator_id, owner_id, name, money) VALUES(%d, %d, '%e', %d)", _:Player.Auth[playerid][id], _:Player.Auth[ownerid][id], name, budget);
	mysql_pquery_inline(dbhandle, query, using inline FactionCreated);

	Text_Send(playerid, $ACTION_SUCCESS);
	Text_Send(ownerid, $OWNER_FACTION, name, budget);
	Text_Send(ALL_PLAYERS, $NEW_FACTION, name, Player.RP[playerid][name], Player.RP[ownerid][name]);

	return 1;
}
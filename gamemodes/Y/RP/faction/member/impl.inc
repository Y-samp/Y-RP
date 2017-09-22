#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_hooks>

loadtext y[core], y_rp_factions[core];

hook OnPlayerLogin(playerid)
{
	new bool:inAnyGroup = false;

	foreach (new faction:Faction) {
		if (Faction.Meta[faction][@owner_id] == Player.Auth[playerid][@id]) {
			Iter_Add(FactionLeader, faction);
			Group_SetPlayer(FactionLeaderGroup, playerid, true);
			inAnyGroup = true;
		}

		foreach (new member:FactionMember[faction]) {
			if (Faction.Member[faction][member][@player_id] == Player.Auth[playerid][@id]) {
				Iter_Add(OnlineFactionMember[faction], member);
				Faction.Member[faction][member][@player] = playerid;
				Group_SetPlayer(FactionGroup[faction], playerid, true);
				inAnyGroup = true;

				break;
			}
		}
	}

	if (inAnyGroup) {
		Group_SetPlayer(AllFactionGroup, playerid, true);
	}

	return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
	foreach (new faction:Faction) {
		foreach (new member:OnlineFactionMember[faction]) {
			if (Faction.Member[faction][member][@player] == playerid) {
				// No need for same remove as we break out the loop
				Iter_Remove(OnlineFactionMember[faction], member);
				Faction.Member[faction][member][@player] = INVALID_PLAYER_ID;
				break;
			}
		}
	}

	return 1;
}

IsFactionLevel(playerid, factionid, level)
{
	foreach (new member:FactionMember[factionid]) {
		if (Faction.Member[factionid][member][@player] == playerid) {
			if (Faction.Member[factionid][member][@rank] >= level) {
				return true;
			}
		}
	}

	return false;
}

IsPlayerInAnyFaction(playerid)
{
	return !!Group_GetPlayer(AllFactionGroup, playerid);
}

// GetPlayerFactionGroup(playerid)
// {
// 	foreach (new faction:Faction) {
// 		foreach (new member:OnlineFactionMember[faction]) {
// 	  	if (Faction.Member[faction][member][@player] == playerid) {
// 	  		return FactionGroup[faction];
// 	  	}
// 	  }
// 	}

// 	return INVALID_GROUP;
// }

ListFactionMembers(playerid, factionid)
{
	new
		members[2048],
		addMemberIndex = 0
	;

	Text_Format_Other(members, sizeof members, Langs_GetPlayerLanguage(playerid), $FACTIONS_MEMBERS_LIST_HEADER, "\t", "\t");

	foreach (new member:FactionMember[factionid]) {
		new
			DBID:playerDBid = Faction.Member[factionid][member][@player_id],
			playername[MAX_PLAYER_NAME + 1],
			rank = Faction.Member[factionid][member][@rank],
			online[16]
		;

		addMemberIndex++;

		strcpy(online, Iter_Contains(OnlineFactionMember[factionid], member) ? "Yes" : "No");
		strcpy(playername, Faction.Member[factionid][member][@player_name]);

		format(members, sizeof members, "%s\nid: %d) %s\t%d\t%s", members, _:playerDBid, playername, rank, online);
	}

	if (
		(
			IsAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL) ||
			IsFactionLevel(playerid, factionid, Y_RP_FACTION_MANAGE_PLAYER_LEVEL)
		) &&
		Iter_Free(FactionMember[factionid]) != INVALID_ITERATOR_SLOT
	) {
		new tmp[32];
		Text_Format_Other(tmp, sizeof tmp, Langs_GetPlayerLanguage(playerid), $FACTIONS_MEMBERS_LIST_ADD);
		format(members, sizeof members, "%s\n%s", members, tmp);
	}

	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if(!response) {
			InspectFaction(playerid, factionid);

			@return 1;
		}

		if (listitem == addMemberIndex) {
			FactionAddMemberDialog(playerid, factionid);

			@return 1;
		}


		new i;

		foreach (new member:FactionMember[factionid]) {
			if (listitem == i++) {
				@return FactionInspectMember(playerid, factionid, member);
			}
		}

		@return 1;
	}

	Text_FormattedTablistHeadersBox(playerid, using inline Response, $FACTIONS_MEMBERS_LIST_TITLE, members, $INSPECT, $BACK);

	return 1;
}

FactionAddMemberDialog(playerid, factionid, error = 0)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if(!response) {
			ListFactionMembers(playerid, factionid);

			@return 1;
		}

		if (isnull(inputtext) || strlen(inputtext) > MAX_PLAYER_NAME) {
			FactionAddMemberDialog(playerid, factionid, E_FACTION_INVALID_PLAYER);			

			@return 1;
		}

		new
			targetid = INVALID_PLAYER_ID,
			probableName[MAX_PLAYER_NAME + 1]
		;

		strcpy(probableName, inputtext);
		UnRPizeName(probableName);

		if (sscanf(probableName, "u", targetid) || targetid == INVALID_PLAYER_ID) {
			inline Handler()
			{
				new 
					DBID:targetDBid = INVALID_DB_ID
				;	

				if (!cache_num_rows()) {
					@return	FactionAddMemberDialog(playerid, factionid, E_FACTION_INVALID_PLAYER);
				}

				cache_get_value_name_int(0, "id", _:targetDBid);

				RPizeName(probableName);

				if (!FactionAddMember(factionid, targetDBid, probableName)) {
					Text_Send(playerid, $FACTIONS_MEMBERS_ADD_FULL);

					@return ListFactionMembers(playerid, factionid);
				}

				Text_Send(playerid, $FACTIONS_MEMBERS_ADDED, probableName, Faction.Meta[factionid][@name]);
				ListFactionMembers(playerid, factionid);
			}

			new query[50 + MAX_PLAYER_NAME + 1];
			mysql_format(dbhandle, query, sizeof query, "SELECT id FROM " PLAYER_TABLE " WHERE name = '%e'", probableName);
			mysql_tquery_inline(dbhandle, query, using inline Handler);

			@return 1;
		}

		foreach (new slot:OnlineFactionMember[factionid]) {
			if (Faction.Member[factionid][slot][@player] == targetid) {
				@return FactionAddMemberDialog(playerid, factionid, E_FACTION_PLAYER_ALREADY_ADDED);
			}
		}

		if (!FactionAddMember(factionid, .targetid = targetid)) {
			Text_Send(playerid, $FACTIONS_MEMBERS_ADD_FULL);

			@return ListFactionMembers(playerid, factionid);
		}

		Text_Send(playerid, $FACTIONS_MEMBERS_ADDED, Player.RP[targetid][@name], Faction.Meta[factionid][@name]);
		Text_Send(targetid, $FACTIONS_MEMBERS_ADDED_YOU, Player.RP[playerid][@name], Faction.Meta[factionid][@name]);
		ListFactionMembers(playerid, factionid);

		@return 1;
	}

	new errorText[64];
	if (error == 1) {
		Text_Format_Dialog(errorText, sizeof errorText, Langs_GetPlayerLanguage(playerid), $FACTIONS_MEMBERS_ADD_ERROR_PLAYER);
	} else if (error == 2){
		Text_Format_Dialog(errorText, sizeof errorText, Langs_GetPlayerLanguage(playerid), $FACTIONS_MEMBERS_ADD_ERROR_ALREADY_ADDED);
	}

	Text_InputBox(playerid, using inline Response, $FACTIONS_MEMBERS_ADD_TITLE, $FACTIONS_MEMBERS_ADD_BODY, $SELECT, $BACK, errorText);

	return 1;
}

FactionAddMember(factionid, DBID:targetDBid = INVALID_DB_ID, const targetName[] = "", targetid = INVALID_PLAYER_ID)
{
	new slot = Iter_Free(FactionMember[factionid]);
	if (slot == INVALID_ITERATOR_SLOT) return 0;

	Iter_Add(FactionMember[factionid], slot);

	if (targetid != INVALID_PLAYER_ID) {
		Iter_Add(OnlineFactionMember[factionid], slot);
		Faction.Member[factionid][slot][@player_id] = Player.Auth[targetid][@id];
		Faction.Member[factionid][slot][@player] = targetid;
		strcpy(Faction.Member[factionid][slot][@player_name], Player.RP[targetid][@name]);
	} else {
		Faction.Member[factionid][slot][@player_id] = targetDBid;
		Faction.Member[factionid][slot][@player] = INVALID_PLAYER_ID;
		strcpy(Faction.Member[factionid][slot][@player_name], targetName);
	}

	Faction.Member[factionid][slot][@rank] = 0;

	new query[128];
	mysql_format(
		dbhandle,
		query,
		sizeof query,
		"INSERT INTO " FACTION_MEMBER_TABLE " VALUES (null, %d, %d, %d)",
		_:Faction.Meta[factionid][@id],
		_:Faction.Member[factionid][slot][@player_id],
		Faction.Member[factionid][slot][@rank]		
	);
	mysql_tquery(dbhandle, query);

	return 1;
}

FactionInspectMember(playerid, factionid, member)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if(!response) {
			@return ListFactionMembers(playerid, factionid);
		}

		if (listitem == 0) {
			if (
				IsAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL) ||
				IsFactionLevel(playerid, factionid, Y_RP_FACTION_CHANGE_RANK_LEVEL)
			) {
				@return FactionChangeMemberRankDialog(playerid, factionid, member);
			}

			Text_Send(playerid, $UNAUTHORIZED_ACTION);

			@return FactionInspectMember(playerid, factionid, member);
		}

		if (
			IsAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL) ||
			IsFactionLevel(playerid, factionid, Y_RP_FACTION_MANAGE_PLAYER_LEVEL)
		) {
			@return FactionConfirmRemove(playerid, factionid, member);
		}
	
		@return Text_Send(playerid, $UNAUTHORIZED_ACTION);
	}
	
	Text_ListBox(playerid, using inline Response, $FACTION_INSPECT_MEMBER_TITLE, $FACTION_INSPECT_MEMBER_BODY, $INSPECT, $BACK);

	return 1;
}

FactionChangeMemberRankDialog(playerid, factionid, member, error = 0)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if(!response) {
			FactionInspectMember(playerid, factionid, member);

			@return 1;
		}

		new rank;
		if (sscanf(inputtext, "d", rank) || !(0 < rank < Y_RP_FACTION_MAX_RANK)) {
			@return FactionChangeMemberRankDialog(playerid, factionid, member, 1);
		}

		FactionChangeMemberRank(factionid, member, rank);
		Text_Send(playerid, $FACTION_MEMBER_CHANGED_RANK, Faction.Member[factionid][member][@player_name], rank);
		if (Iter_Contains(OnlineFactionMember[factionid], member)) {
			Text_Send(playerid, $FACTION_MEMBER_CHANGED_RANK_YOU, Player.RP[playerid][@name], rank);
		}

		ListFactionMembers(playerid, factionid);

		@return 1;
	}

	new errorText[64];
	if (error == 1) {
		Text_Format_Dialog(errorText, sizeof errorText, Langs_GetPlayerLanguage(playerid), $FACTION_INSPECT_MEMBER_CHANGE_RANK_INVALID);
	}
	
	return Text_InputBox(playerid, using inline Response, $FACTION_INSPECT_MEMBER_CHANGE_RANK_TITLE, $FACTION_INSPECT_MEMBER_CHANGE_RANK_BODY, $OK, $BACK, errorText, Faction.Member[factionid][member][@player_name], 0, Y_RP_FACTION_MAX_RANK - 1);
}

FactionChangeMemberRank(factionid, member, rank)
{
	Faction.Member[factionid][member][@rank] = rank;

	new query[128];
	mysql_format(dbhandle, query, sizeof query, "UPDATE " FACTION_MEMBER_TABLE " SET rank = %d WHERE faction_id = %d AND player_id = %d", rank, _:Faction.Meta[factionid][@id], _:Faction.Member[factionid][member][@player_id]);
	mysql_tquery(dbhandle, query);

	return 1;
}

FactionConfirmRemove(playerid, factionid, member)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if(!response) {
			FactionInspectMember(playerid, factionid, member);

			@return 1;
		}

		FactionRemovePlayer(factionid, member);
		Text_Send(playerid, $FACTION_INSPECT_MEMBER_REMOVED, Faction.Member[factionid][member][@player_name], Faction.Meta[factionid][@name]);
		if (Iter_Contains(OnlineFactionMember[factionid], member)) {
			Text_Send(playerid, $FACTION_INSPECT_MEMBER_REMOVED_YOU, Faction.Meta[factionid][@name], Player.RP[playerid][@name]);
		}
	
		ListFactionMembers(playerid, factionid);

		@return 1;
	}
	
	return Text_MessageBox(playerid, using inline Response, $FACTION_INSPECT_MEMBER_REMOVE_TITLE, $FACTION_INSPECT_MEMBER_REMOVE_BODY, $OK, $BACK, Faction.Member[factionid][member][@player_name]);
}

FactionRemovePlayer(factionid, member)
{
	Iter_Remove(FactionMember[factionid], member);

	if (Iter_Contains(OnlineFactionMember[factionid], member)) {
		new playerid = Faction.Member[factionid][member][@player];
		Group_SetPlayer(FactionGroup[factionid], playerid, false);

		new bool:isInAnyGroup = false;

		foreach (new faction:Faction) {
			if (Group_GetPlayer(FactionGroup[faction], playerid)) {
				isInAnyGroup = true;
				break;
			}
		}

		if (!isInAnyGroup) {
			Group_SetPlayer(AllFactionGroup, playerid, false);
		}
	}

	Iter_Remove(OnlineFactionMember[factionid], member);

	new query[128];
	mysql_format(dbhandle, query, sizeof query, "DELETE FROM " FACTION_MEMBER_TABLE " WHERE faction_id = %d AND player_id = %d", _:Faction.Meta[factionid][@id], _:Faction.Member[factionid][member][@player_id]);
	mysql_tquery(dbhandle, query);
}
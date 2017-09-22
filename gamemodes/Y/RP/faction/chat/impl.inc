#if defined _inc_impl
	#undef _inc_impl
#endif

// #include <YSI\y_text>

// loadtext y_rp_factions[core];

// hook OnGroupInit()
// {
// 	GROUP_ADD<AllFactionGroup>
// 	{
// 		@YCMD:factionchat;
// 		@YCMD:f;
// 	}

// 	Command_AddAltNamed("factionchat", "f");

// 	return 1;
// }

// YCMD:factionchat(playerid, params[], help)
// {
// 	if(help) return SCM(playerid, -1, "Not supported");

// 	new Group:factionGroup = GetPlayerFactionGroup(playerid);

// 	if (factionGroup == INVALID_GROUP) {
// 		return Text_Send(playerid, $NOT_IN_ANY_FACTION);
// 	}

// 	new
// 		message[128],
// 		factionPerson[Y_RP_FACTION_MAX_RANK_NAME + MAX_PLAYER_NAME + 32] // name + rank + brackets + colours
// 	;

// 	if (sscanf(params, "s[129]", message)) {
// 		return Text_Send(playerid, $INVALID_COMMAND_PARAMS_F, YCMD:factionchat);
// 	}

// 	strcpy(factionPerson, "{ERROR}WTF Nigguh{NORMAL}: ");

// 	Text_Send(factionGroup, $FACTION_GROUP_CHAT, factionPerson, message);

// 	return 1;
// }
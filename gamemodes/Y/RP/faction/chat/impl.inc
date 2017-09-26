#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_text>

loadtext y[core], y_rp_factions[core];

hook OnGroupInit()
{
	Command_AddAlt(YCMD:factionchat, "f");
	
	GROUP_ADD<AllFactionGroup>
	{
		@YCMD:factionchat;
		@YCMD:f;
	}

	return 1;
}

YCMD:factionchat(playerid, params[], help)
{
	if(help) return SCM(playerid, -1, "Not supported");

	new Group:factionGroup = GetPlayerFactionGroup(playerid);

	if (factionGroup == INVALID_GROUP) {
		return Text_Send(playerid, $NOT_IN_ANY_FACTION);
	}

	new message[129];

	if (sscanf(params, "s[129]", message)) {
		return Text_Send(playerid, $INVALID_COMMAND_PARAMS_F, YCMD:factionchat);
	}

	new
		factionid = INVALID_FACTION_ID,
		rank = 0
	;

	foreach (new faction:Faction) {
		new breakOut = false;
		foreach (new member:OnlineFactionMember[faction]) {
	  	if (Faction.Member[faction][member][@player] == playerid) {
	  		factionid = faction;
	  		rank = Faction.Member[faction][member][@rank];
	  		breakOut = true;
	  		break;
	  	}
	  }

		if (breakOut) {
	  	break;
	  }
	}

	if (factionid == INVALID_FACTION_ID) {
		err("Valid group but invalid faction index, should not happen");

		return Text_Send(playerid, $INTERNAL_ERROR);
	}

	new
		toBeSent[145]
	;

	format(toBeSent, sizeof toBeSent, "{%06x}[%s]{%06x}: %s", Faction.Rank[factionid][rank][@colour] >>> 8, Faction.Rank[factionid][rank][@name], GetColour("NORMAL") >>> 8, message);

	foreach (new member:GroupMember(factionGroup)) {
		SCM(member, -1, toBeSent);
	}

	return 1;
}
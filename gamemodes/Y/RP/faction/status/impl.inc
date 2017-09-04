#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_hooks>

loadtext y[core], y_rp_factions[core];

hook OnGroupInit()
{
	GROUP_ADD<AdminGroup[0]>
	{
		@YCMD:setfactionmoney;
	}
}

YCMD:setfactionmoney(playerid, params[], help)
{
	if (help) return SCM(playerid, -1, "Not supported");

	new
		DBID:factionDBid,
		money
	;

	if (sscanf(params, "dd", _:factionDBid, money)) return Text_Send(playerid, $INVALID_COMMAND_PARAMS_SFM, YCMD:setfactionmoney);

	new factionid = GetFactionIndexFromDBID(factionDBid);

	if (factionid == INVALID_FACTION_ID) return Text_Send(playerid, $INVALID_FACTION_ID_ERROR); 

	printf("Set faction %d money to %d", _:factionDBid, money);
	Faction.Status[factionid][@money] = money;

	new query[72];
	mysql_format(dbhandle, query, sizeof query, "UPDATE "FACTION_TABLE" SET money = %d WHERE id = %d", money, _:factionDBid);
	mysql_tquery(dbhandle, query);

	return 1;
}
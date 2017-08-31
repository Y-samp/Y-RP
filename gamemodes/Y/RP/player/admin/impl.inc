#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_hooks>

loadtext y_rp_players[admin];

hook OnGameModeInit()
{
	AdminOnDuty = Group_Create("Admins on duty");

	return 1;
}

hook OnGroupInit()
{	
	GROUP_ADD<AdminGroup[0]>
	{
		@YCMD:aduty;
	}

	return 1;
}

YCMD:aduty(playerid, params[], help)
{
	if(help) return SCM(playerid, -1, "Not supported");

	if (IsOnAdminDuty(playerid)) {
		Group_SetPlayer(AdminOnDuty, playerid, false);
		Text_Send(playerid, $ADUTY_OFF);
		Text_Send(LoggedGroup, $ADUTY_BROADCAST_OFF, Player.RP[playerid][name]);
		Text_Send(ALL_PLAYERS, $ADUTY_BROADCAST_OFF, Player.RP[playerid][name]);
	} else {
		Group_SetPlayer(AdminOnDuty, playerid, true);
		Text_Send(playerid, $ADUTY_ON);
		Text_Send(LoggedGroup, $ADUTY_BROADCAST_ON, Player.RP[playerid][name]);
		Text_Send(ALL_PLAYERS, $ADUTY_BROADCAST_ON, Player.RP[playerid][name]);
	}

	return 1;
}
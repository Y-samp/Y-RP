#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_hooks>

hook OnPlayerText(playerid, text[])
{
	new
		message[144 + 1]
	;

	format(message, sizeof message, "%s%s" GRAY ":" WHITE " %s", IsOnAdminDuty(playerid) ? RED : WHITE, Player.RP[playerid][@name], text);

	SendClientMessage(playerid, -1, message);

	return Y_HOOKS_BREAK_RETURN_0;
}
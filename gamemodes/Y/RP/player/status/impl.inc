#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_hooks>

hook OnPlayerLogin(playerid)
{
	strcpy(Player.RP[playerid][name], Player.Auth[playerid][name]);
	RPizeName(Player.RP[playerid][name]);
}

RPizeName(name[], len = sizeof name)
{
	for (new i = 0; i != len; ++i) {
		if (name[i] == '_') {
			name[i] = ' ';
		}
	}
}
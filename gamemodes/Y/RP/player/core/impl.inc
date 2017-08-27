#if defined _inc_impl
	#undef _inc_impl
#endif

loadtext y_rp_players[auth];

forward PlayerRestrictName(playerid);
public PlayerRestrictName(playerid)
{
	new
		name[MAX_PLAYER_NAME + 1],
		len,
		bool:success = true
	;
	GetPlayerName(playerid, name, sizeof name);
	len = strlen(name);

	new bool:hasUnderscore = false;
	for (new i = 0; i != len; ++i) {
		new isAlpha = 'a' < name[i] < 'z' || 'A' < name[i] < 'Z';
		new isUnderscore = name[i] == '_';

		if (isUnderscore) {
			hasUnderscore = true;
		}

		if (!isAlpha && !(isUnderscore && i != 0 && i != len - 1)) {
			success = false;
			break;
		}
	}

	if (!hasUnderscore) {
		success = false;
	}

	if (!success) {
		Text_Send(playerid, $INVALID_USERNAME);

		return 0;
	}
	
	return 1;
}
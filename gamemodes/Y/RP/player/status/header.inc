#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI\y_commands>

enum E_PLAYER_RP {
	E_PLAYER_RP_name[MAX_PLAYER_NAME + 1]
}

new PlayerRP[MAX_PLAYERS][E_PLAYER_RP];
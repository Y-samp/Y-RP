#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI\y_groups>
#include <YSI\y_commands>
#include <YSI\y_text>

new
	Group:AdminOnDuty
;

#define IsOnAdminDuty(%0) Group_GetPlayer(AdminOnDuty, %0)
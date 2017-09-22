#if defined _inc_RP
	#undef _inc_RP
#endif

#if !defined Y_PLAYER_ENABLED
	#error Y-RP requires player module to be enabled (Y_PLAYER_ENABLED)
#endif

#if !defined Y_PLAYER_REQUIRE_ACCOUNT
	#error Y-RP requires player accounts to be enabled (Y_PLAYER_REQUIRE_ACCOUNT)
#endif

#if !defined Y_RP_HEADER
	#include "player\core\header"
	#include "player\status\header"
	#include "player\admin\header"
	#if defined Y_RP_FACTION
		#include "faction\header"
	#endif
	#include "player\chat\header"
	#define Y_RP_HEADER
#elseif !defined Y_RP_END
	#include "player\core\impl"
	#include "player\status\impl"
	#include "player\admin\impl"
	#if defined Y_RP_FACTION
		#include "faction\impl"
	#endif
	#include "player\chat\impl"
	#define Y_RP_END
#else
	#error Y RP already set up
#endif
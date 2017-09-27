#if defined _inc_header
	#undef _inc_header
#endif

enum E_FACTION_RANK {
	E_FACTION_Rank_name[Y_RP_FACTION_MAX_RANK_NAME + 1],
	E_FACTION_Rank_colour
}

new FactionRank[Y_RP_FACTION_LIMIT][Y_RP_FACTION_MAX_RANK][E_FACTION_RANK];

#define FACTION_TABLE "rp_factions"
#define FACTION_RANK_TABLE "rp_faction_ranks"
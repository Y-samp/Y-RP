#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI\y_hooks>

hook OnLoadObjects()
{
	inline Handler()
	{
		new rows = cache_num_rows();

		for (new row = 0; row != rows; ++row) {
			new objectIndex = Iter_Free(Object);
			if (objectIndex == INVALID_ITERATOR_SLOT) {
				err("Limit of %d objects reached (Y_OBJECT_TOTAL)", Y_OBJECT_TOTAL);
				break;
			}

			new DBID:factionDBid;
			cache_get_value_name_int(row, "faction_id", _:factionDBid);

			new factionid = GetFactionIndexFromDBID(factionDBid);
			if (factionid == INVALID_FACTION_ID) {
				log("Tried to load objects for nonexisting faction (dbID: %d)", _:factionDBid);
				continue;
			}

			new slot = Iter_Free(FactionObject[factionid]);
			if (slot == INVALID_ITERATOR_SLOT) {
				err("Limit of %d faction objects reached (Y_RP_FACTION_OBJECT_LIMIT)", Y_RP_FACTION_OBJECT_LIMIT);
				break;
			}

			cache_get_value_name_int(row, "id", Faction.Object[factionid][id][slot]);
			Faction.Object[factionid][faction_id][slot] = _:factionDBid;

			cache_get_value_name_int(row, "object_id", _:Object[objectIndex][id]);

			Faction.Object[factionid][object_id][slot] = _:Object[objectIndex][id];
			Faction.Object[factionid][object][slot] = objectIndex;

			cache_get_value_name_int(row, "model", Object[objectIndex][model]);
			cache_get_value_name_float(row, "X", Object[objectIndex][pos][0]);
			cache_get_value_name_float(row, "Y", Object[objectIndex][pos][1]);
			cache_get_value_name_float(row, "Z", Object[objectIndex][pos][2]);
			cache_get_value_name_float(row, "RX", Object[objectIndex][rot][0]);
			cache_get_value_name_float(row, "RY", Object[objectIndex][rot][1]);
			cache_get_value_name_float(row, "RZ", Object[objectIndex][rot][2]);
			cache_get_value_name_int(row, "world", Object[objectIndex][world]);
			cache_get_value_name_int(row, "interior", Object[objectIndex][int]);

			Iter_Add(Object, objectIndex);
			Iter_Add(FactionObject[factionid], slot);

			CreateManagedObject(objectIndex);
		}

		printf("%d faction objects", rows);

		LoadedObjects++;
	}

	static const query[] = "SELECT fo.*, o.* FROM " FACTION_OBJECT_TABLE " fo LEFT JOIN " OBJECT_TABLE " o ON fo.object_id = o.id";
	mysql_pquery_inline(dbhandle,	query, using inline Handler);

	TotalObjects++;
}
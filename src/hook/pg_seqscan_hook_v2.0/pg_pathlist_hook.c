#include "common.h"
#include "pg_custom_pathlist_hook.h"
#include "hlog.h"
#include "func_id.h"

PG_MODULE_MAGIC;

#define PG_SEQSCAN_HOOK_VERSION "v2.0"

void    _PG_init(void);
static set_rel_pathlist_hook_type set_rel_pathlist_next = NULL;

unsigned short func_id_support_hash[65536] = {0};
unsigned short func_id_support_array[] = {
	FLOAT8_EQ, FLOAT8_NE, FLOAT8_LT, FLOAT8_GT, FLOAT8_LE, FLOAT8_GE,
	FLOAT8MUL, FLOAT8DIV, FLOAT8PL, FLOAT8MI,
	DATE_EQ, DATE_NE, DATE_LT,  DATE_GT, DATE_LE, DATE_GE,
	TIMESTAMP_LT_DATE, TIMESTAMP_LE_DATE, TIMESTAMP_EQ_DATE,
	TIMESTAMP_GT_DATE, TIMESTAMP_GE_DATE, TIMESTAMP_NE_DATE,
	DATE_LT_TIMESTAMP, DATE_LE_TIMESTAMP, DATE_EQ_TIMESTAMP,
	DATE_GT_TIMESTAMP, DATE_GE_TIMESTAMP, DATE_NE_TIMESTAMP,
};

void
_PG_init(void)
{

	hlog_init();

	DBG_INFO("PG pg_seqscan_hook versioin:%s\n", PG_SEQSCAN_HOOK_VERSION);

	memset(func_id_support_hash, 0, sizeof(func_id_support_hash));

        for(int i=0; i<sizeof(func_id_support_array)/sizeof(short); i++){
                func_id_support_hash[func_id_support_array[i]] = 1;
        }

	set_rel_pathlist_next = set_rel_pathlist_hook;
	set_rel_pathlist_hook = PGSetCustomRelPathlistHook;

}


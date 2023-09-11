#ifndef __QUERY_H__
#define __QUERY_H__

#include <string.h>
#include "pg_filter_common.h"
#include "hlog.h"

int get_file_request_cnt(char *filepath, int buf_cnt);
int get_query_request_cnt(qual_t *qual, int buf_cnt);

__debug_use void fill_tupdesc(tupdesc_t *tupledesc);
__debug_use void fill_filter_q6(filter_t *filter);
__debug_use void fill_qual(qual_t *qual, char *filepath, int file_cnt);
__debug_use void fill_filter_mul(filter_t *filter);

#endif

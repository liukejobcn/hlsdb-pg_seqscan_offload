#ifndef __QUERY_H__
#define __QUERY_H__

#include "fpga_common.h"

void fill_tupdesc(tupdesc_t *tupledesc);
void fill_filter_q6(filter_t *filter);
void fill_qual(qual_t *qual, char *filepath);


#endif

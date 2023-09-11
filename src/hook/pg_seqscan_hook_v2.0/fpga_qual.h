#ifndef __FPGA_QUAL_H__
#define __FPGA_QUAL_H__
#include "pg_filter_common.h"

int create_fpga_filter(RelOptInfo* baserel, filter_t *filter, int *max_attr_index, int *op_cnt);
void fpga_ops_check(Node* expr, int *is_support);
int create_tupdesc_for_fpga(struct tupleDesc *pg_tupdesc, tupdesc_t *fpga_tupdesc);
int create_qual_for_fpga(qual_t *qual, char *filepath, int file_cnt, int max_attr_index, int op_cnt);

#endif

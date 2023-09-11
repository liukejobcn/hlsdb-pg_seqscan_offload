#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "query.h"

void fill_tupdesc(tupdesc_t *tupledesc)
{
        int i=0;
        short *desc_len_in = tupledesc->desc_len_in;
        short *desc_align_in = tupledesc->desc_align_in;

        for(;i<4; i++){
                desc_len_in[i] = 4;
                desc_align_in[i] = 3;
        }

        for(;i<8; i++){
                desc_len_in[i] = 8;
                desc_align_in[i] = 7;
        }

        for(;i<10; i++){
                desc_len_in[i] = -1;
                desc_align_in[i] = 3;
        }

        for(;i<13; i++){
                desc_len_in[i] = 4;
                desc_align_in[i] = 3;
        }

        for(;i<16; i++){
                desc_len_in[i] = -1;
                desc_align_in[i] = 3;
        }
        return ;
}


void fill_filter_q6(filter_t *filter)
{
	int date_lo = -2191;
	int date_hi = -1826;
	double discount_lo = 0.05;
	double discount_hi = 0.07;
	double quantity_hi = 24.0;
	pg_clause_op_t *ops = filter->clause;

	ops[0].op_type = T_OpExpr;
	ops[0].op_class = C_INT;
	ops[0].func_id = DATE_GE;
	ops[0].arg0_tag = T_Var;
	ops[0].arg0_index = 10;
	ops[0].arg1_tag = T_Const;
	ops[0].arg1_index = 0;

	ops[1].op_type = T_OpExpr;
	ops[1].op_class = C_INT;
	ops[1].func_id = DATE_LT;
	ops[1].arg0_tag = T_Var;
	ops[1].arg0_index = 10;
	ops[1].arg1_tag = T_Const;
	ops[1].arg1_index = 1;

	ops[2].op_type = T_OpExpr;
	ops[2].op_class = C_FLOAT8;
	ops[2].func_id = FLOAT8_GE;
	ops[2].arg0_tag = T_Var;
	ops[2].arg0_index = 6;
	ops[2].arg1_tag = T_Const;
	ops[2].arg1_index = 0;

	ops[3].op_type = T_OpExpr;
	ops[3].op_class = C_FLOAT8;
	ops[3].func_id = FLOAT8_LE;
	ops[3].arg0_tag = T_Var;
	ops[3].arg0_index = 6;
	ops[3].arg1_tag = T_Const;
	ops[3].arg1_index = 1;

	ops[4].op_type = T_OpExpr;
	ops[4].op_class = C_FLOAT8;
	ops[4].func_id = FLOAT8_LT;
	ops[4].arg0_tag = T_Var;
	ops[4].arg0_index = 4;
	ops[4].arg1_tag = T_Const;
	ops[4].arg1_index = 2;


	ops[5].op_type = T_BoolExpr;
	ops[5].func_id = OP_AND;
	ops[5].nargs = 5;

	filter->const_int[0] = date_lo;
	filter->const_int[1] = date_hi;
	filter->const_float8[0] = discount_lo;
	filter->const_float8[1] = discount_hi;
	filter->const_float8[2] = quantity_hi;
}


void fill_filter_mul(filter_t *filter)
{
	int date_lo = -2191;
	int date_hi = -1826;
	double discount_lo = 0.05;
	double discount_hi = 0.07;
	double quantity_hi = 24.0;

	pg_clause_op_t *ops = filter->clause;

	ops[0].op_type = T_OpExpr;
	ops[0].op_class = C_INT;
	ops[0].func_id = DATE_GE;
	ops[0].arg0_tag = T_Var;
	ops[0].arg0_index = 10;
	ops[0].arg1_tag = T_Const;
	ops[0].arg1_index = 0;

	ops[1].op_type = T_OpExpr;
	ops[1].op_class = C_INT;
	ops[1].func_id = DATE_LT;
	ops[1].arg0_tag = T_Var;
	ops[1].arg0_index = 10;
	ops[1].arg1_tag = T_Const;
	ops[1].arg1_index = 1;

	ops[2].op_type = T_OpExpr;
	ops[2].op_class = C_FLOAT8;
	ops[2].func_id = FLOAT8_GE;
	ops[2].arg0_tag = T_Var;
	ops[2].arg0_index = 6;
	ops[2].arg1_tag = T_Const;
	ops[2].arg1_index = 0;

	ops[3].op_type = T_OpExpr;
	ops[3].op_class = C_FLOAT8;
	ops[3].func_id = FLOAT8_LE;
	ops[3].arg0_tag = T_Var;
	ops[3].arg0_index = 6;
	ops[3].arg1_tag = T_Const;
	ops[3].arg1_index = 1;

	ops[4].op_type = T_OpExpr;
	ops[4].op_class = C_FLOAT8;
	ops[4].func_id = FLOAT8_LT;
	ops[4].arg0_tag = T_Var;
	ops[4].arg0_index = 4;
	ops[4].arg1_tag = T_Const;
	ops[4].arg1_index = 2;

	ops[5].op_type = T_OpExpr;
	ops[5].op_class = C_FLOAT8;
	ops[5].func_id = FLOAT8MUL;
	ops[5].arg0_tag = T_Var;
	ops[5].arg0_index = 4;
	ops[5].arg1_tag = T_Var;
	ops[5].arg1_index = 5;

	ops[6].op_type = T_OpExpr;
	ops[6].op_class = C_FLOAT8;
	ops[6].func_id = FLOAT8_GT;
	ops[6].arg0_tag = 0;
	ops[6].arg1_tag = T_Const;
	ops[6].arg1_index = 3;


	ops[7].op_type = T_BoolExpr;
	ops[7].func_id = OP_AND;
	ops[7].nargs = 6;

	filter->const_int[0] = date_lo;
	filter->const_int[1] = date_hi;
	filter->const_float8[0] = discount_lo;
	filter->const_float8[1] = discount_hi;
	filter->const_float8[2] = quantity_hi;
	filter->const_float8[3] = 10000.0;
}

void fill_qual(qual_t *qual, char *filepath, int file_cnt)
{
	qual->natts = 11;
	qual->filter_cnt = 6;
	qual->file_cnt = file_cnt;
	strncpy(qual->filepath, filepath, sizeof(qual->filepath));
}

int get_file_request_cnt(char *filepath, int buf_cnt)
{
        int req_cnt;
        int filesize;
        int file_blk_cnt;
        struct stat statbuf;

        stat(filepath, &statbuf);
        filesize = statbuf.st_size;
        file_blk_cnt = filesize/8192;

        req_cnt = (file_blk_cnt < buf_cnt) ? file_blk_cnt : buf_cnt;

        return req_cnt;
}

int get_query_request_cnt(qual_t *qual, int buf_cnt)
{
        int req_cnt = 0;
        char filepath[512];

        snprintf(filepath, sizeof(filepath), "%s", qual->filepath);
        req_cnt += get_file_request_cnt(filepath, buf_cnt);

        for(int i=1; i<qual->file_cnt; i++){
                snprintf(filepath, sizeof(filepath), "%s.%d", qual->filepath, i);
                req_cnt += get_file_request_cnt(filepath, buf_cnt);
        }

        return req_cnt;
}


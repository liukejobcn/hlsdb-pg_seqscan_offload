#ifndef __PG_FILTER_H__
#define __PG_FILTER_H__

#include <ap_int.h>
#include <hls_stream.h>
#include <string.h>
#include "func_id.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char s8;
typedef short s16;
typedef int	s32;

typedef struct pd_linp{
	unsigned int lp_off:15;
	unsigned int lp_flags:2;
	unsigned int lp_len:15;
}pd_linp_t;

#define MAX_TUPLE_PER_PAGE	128
#define MAX_COL 16

typedef struct tupdesc{
	short desc_len_in[MAX_COL];
	short desc_align_in[MAX_COL];
}tupdesc_t;

//15B
typedef struct pg_clause_op{
	u8 	op_type;		//math|cmp, logic
	u8 op_class;		//double,int, bool
	u16 func_id;
	u8 nargs;
	u8 arg0_tag;
	u16 arg0_index;
	u16 arg0_len;
	u8 arg1_tag;
	u16 arg1_index;
	u16 arg1_len;
}pg_clause_op_t;

//should 64B align
typedef struct filter{
	pg_clause_op_t clause[64];	//1280B
	double  const_float8[64];	//512B
	int 	const_int[128];		//512B
	char	const_char[1024];	//1204B
}filter_t;


#define FLOAT8_CMP(cmp)		bool_stack[bool_stack_top] = (arg0 cmp arg1) ? 1 : 0; bool_stack_top++;
#define FLOAT8_MATH(math)	float8_stack[float8_stack_top] = (arg0 math arg1) ; float8_stack_top++;

#define INT_CMP(cmp)		bool_stack[bool_stack_top] = (arg0 cmp arg1) ? 1 : 0; bool_stack_top++;

#define PG_PAGE_SIZE	8192
#define PE_CNT			8
#define PAGE_BUF_WIDTH	64
#define PAGE_BUF_CNT	(PG_PAGE_SIZE*8/PAGE_BUF_WIDTH)


typedef ap_int<PAGE_BUF_WIDTH> page_buf_t;


extern "C"{
void pg_filter(ap_int<512> *pg_data_in, int blk_dat_offset, int page_cnt, int natts, int filter_cnt,
		ap_int<512> * tupdesc_in, ap_int<512> * filter_in,
		int *tuple_cnt_out, int *tuple_off_out);
}

#endif

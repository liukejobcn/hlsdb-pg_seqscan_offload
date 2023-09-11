#include "pg_filter.h"

void copy_buf(void *dst, void *src, int size)
{
#pragma HLS INLINE off
	memcpy(dst, src, size);
}

void copy_to_local_buf(
		page_buf_t *local_buf0,
		page_buf_t *local_buf1,
		page_buf_t *local_buf2,
		page_buf_t *local_buf3,
		page_buf_t *local_buf4,
		page_buf_t *local_buf5,
		page_buf_t *local_buf6,
		page_buf_t *local_buf7,
		int *page_tuple_cnt0,
		int *page_tuple_cnt1,
		int *page_tuple_cnt2,
		int *page_tuple_cnt3,
		int *page_tuple_cnt4,
		int *page_tuple_cnt5,
		int *page_tuple_cnt6,
		int *page_tuple_cnt7,
		ap_int<512> *pg_data_in,
		int index
		)
{
	u16 pd_lower;
	u16 tuple_cnt;
	ap_int<512> header;

	int i = 0;
	memcpy(&header, pg_data_in + 128*(PE_CNT*index + i), 64);
	pd_lower = header.range(13*8-1, 12*8);
	tuple_cnt = (pd_lower-24)>>2;
	*page_tuple_cnt0 = tuple_cnt;

	i++;
	memcpy(&header, pg_data_in + 128*(PE_CNT*index + i), 64);
	pd_lower = header.range(13*8-1, 12*8);
	tuple_cnt = (pd_lower-24)>>2;
	*page_tuple_cnt1 = tuple_cnt;

	i++;
	memcpy(&header, pg_data_in + 128*(PE_CNT*index + i), 64);
	pd_lower = header.range(13*8-1, 12*8);
	tuple_cnt = (pd_lower-24)>>2;
	*page_tuple_cnt2 = tuple_cnt;

	i++;
	memcpy(&header, pg_data_in + 128*(PE_CNT*index + i), 64);
	pd_lower = header.range(13*8-1, 12*8);
	tuple_cnt = (pd_lower-24)>>2;
	*page_tuple_cnt3 = tuple_cnt;

	i++;
	memcpy(&header, pg_data_in + 128*(PE_CNT*index + i), 64);
	pd_lower = header.range(13*8-1, 12*8);
	tuple_cnt = (pd_lower-24)>>2;
	*page_tuple_cnt4 = tuple_cnt;

	i++;
	memcpy(&header, pg_data_in + 128*(PE_CNT*index + i), 64);
	pd_lower = header.range(13*8-1, 12*8);
	tuple_cnt = (pd_lower-24)>>2;
	*page_tuple_cnt5 = tuple_cnt;

	i++;
	memcpy(&header, pg_data_in + 128*(PE_CNT*index + i), 64);
	pd_lower = header.range(13*8-1, 12*8);
	tuple_cnt = (pd_lower-24)>>2;
	*page_tuple_cnt6 = tuple_cnt;

	i++;
	memcpy(&header, pg_data_in + 128*(PE_CNT*index + i), 64);
	pd_lower = header.range(13*8-1, 12*8);
	tuple_cnt = (pd_lower-24)>>2;
	*page_tuple_cnt7 = tuple_cnt;


	i = 0;
	memcpy(local_buf0, pg_data_in + 128*(PE_CNT*index + i), PG_PAGE_SIZE);

	i++;
	memcpy(local_buf1, pg_data_in + 128*(PE_CNT*index + i), PG_PAGE_SIZE);

	i++;
	memcpy(local_buf2, pg_data_in + 128*(PE_CNT*index + i), PG_PAGE_SIZE);

	i++;
	memcpy(local_buf3, pg_data_in + 128*(PE_CNT*index + i), PG_PAGE_SIZE);

	i++;
	memcpy(local_buf4, pg_data_in + 128*(PE_CNT*index + i), PG_PAGE_SIZE);

	i++;
	memcpy(local_buf5, pg_data_in + 128*(PE_CNT*index + i), PG_PAGE_SIZE);

	i++;
	memcpy(local_buf6, pg_data_in + 128*(PE_CNT*index + i), PG_PAGE_SIZE);

	i++;
	memcpy(local_buf7, pg_data_in + 128*(PE_CNT*index + i), PG_PAGE_SIZE);


}
void duplicate_buf(
		page_buf_t *local_buf0,
		page_buf_t *local_buf1,
		page_buf_t *local_buf2,
		page_buf_t *local_buf3,
		page_buf_t *local_buf4,
		page_buf_t *local_buf5,
		page_buf_t *local_buf6,
		page_buf_t *local_buf7,
		page_buf_t *bufa0,
		page_buf_t *bufa1,
		page_buf_t *bufa2,
		page_buf_t *bufa3,
		page_buf_t *bufa4,
		page_buf_t *bufa5,
		page_buf_t *bufa6,
		page_buf_t *bufa7,
		page_buf_t *bufb0,
		page_buf_t *bufb1,
		page_buf_t *bufb2,
		page_buf_t *bufb3,
		page_buf_t *bufb4,
		page_buf_t *bufb5,
		page_buf_t *bufb6,
		page_buf_t *bufb7
		)
{
	copy_buf(bufa0, local_buf0, PG_PAGE_SIZE);
	copy_buf(bufa1, local_buf1, PG_PAGE_SIZE);
	copy_buf(bufa2, local_buf2, PG_PAGE_SIZE);
	copy_buf(bufa3, local_buf3, PG_PAGE_SIZE);
	copy_buf(bufa4, local_buf4, PG_PAGE_SIZE);
	copy_buf(bufa5, local_buf5, PG_PAGE_SIZE);
	copy_buf(bufa6, local_buf6, PG_PAGE_SIZE);
	copy_buf(bufa7, local_buf7, PG_PAGE_SIZE);

	copy_buf(bufb0, local_buf0, PG_PAGE_SIZE);
	copy_buf(bufb1, local_buf1, PG_PAGE_SIZE);
	copy_buf(bufb2, local_buf2, PG_PAGE_SIZE);
	copy_buf(bufb3, local_buf3, PG_PAGE_SIZE);
	copy_buf(bufb4, local_buf4, PG_PAGE_SIZE);
	copy_buf(bufb5, local_buf5, PG_PAGE_SIZE);
	copy_buf(bufb6, local_buf6, PG_PAGE_SIZE);
	copy_buf(bufb7, local_buf7, PG_PAGE_SIZE);
}

void parse_tuple_attr(int index, int natts, short *desc_len_in, short *desc_align_in,
		unsigned char page[8192], int tup_off, int tup_len,
		unsigned short *tup_off_array, unsigned short *tup_len_array,
		unsigned short *att_pos_array, unsigned short *att_len_array)
{
	int desc;
	short desc_len, desc_align;
	unsigned short pos_index;

	unsigned short pos_index_p, pos_index_n;
	unsigned short len_dat1byte, len_dat4byte;
	unsigned short len;
	unsigned short att_off, att_len;

	int tupdat_off;

	pos_index = page[tup_off + 22];
	iter_atts:for(int i=0; i<natts; i++){
#pragma HLS LOOP_TRIPCOUNT min=16 max=16
		desc_len = desc_len_in[i];
		desc_align = desc_align_in[i];

		pos_index_p = (pos_index + desc_align);
		desc_align = ~desc_align;
		pos_index_p = (pos_index_p) & desc_align;
		if(desc_len > 0){
			att_off = pos_index_p;
			att_len = (unsigned short)desc_len;
			pos_index =  pos_index_p + desc_len;
			*(att_pos_array + index*MAX_COL + i) = att_off;
			*(att_len_array + index*MAX_COL + i) = att_len;
		}else{
			unsigned char meta_len = 1;
			len_dat1byte = page[tup_off + pos_index];
			if(0 == len_dat1byte){
				pos_index = pos_index_p;
			}
			memcpy(&len_dat4byte, page + tup_off + pos_index , 2);
			if(len_dat4byte & 0x1){
				len_dat4byte = (len_dat4byte >> 1)&0x7f;
			}else{
				len_dat4byte = (len_dat4byte >> 2)&0xffff;
				meta_len = 4;
			}
			att_off = pos_index;
			att_len = len_dat4byte;
			pos_index = pos_index + len_dat4byte;
			*(att_pos_array + index*MAX_COL + i) = att_off + meta_len;
			*(att_len_array + index*MAX_COL + i) = att_len - meta_len;
		}

	}

	tup_off_array[index] = tup_off;
	tup_len_array[index] = tup_len;
}

void get_tuple_attr_pos(page_buf_t * buf, int blk_dat_offset, int page_id,
		int natts, short *desc_len_in, short *desc_align_in,
		int page_tuple_cnt,
		unsigned short *tup_off_array,unsigned short *tup_len_array,
		unsigned short *att_pos_array,unsigned short *att_len_array)
{
#pragma HLS INLINE off
	unsigned char * page = (unsigned char *)buf;
	pd_linp_t *item;
	unsigned short len, off;

	item = (pd_linp_t*)(page + 24);
	iter_tuple:for(int i=0; i<page_tuple_cnt; i++){
#pragma HLS LOOP_TRIPCOUNT min=44 max=44
		parse_tuple_attr(i, natts, desc_len_in, desc_align_in, page, item[i].lp_off, item[i].lp_len,
				tup_off_array, tup_len_array,
				att_pos_array, att_len_array);
	}
}


void parallel_get_tuple_attr_pos(
		int blk_dat_offset,
		int index,
		int natts,
		int page_tuple_cnt0,
		int page_tuple_cnt1,
		int page_tuple_cnt2,
		int page_tuple_cnt3,
		int page_tuple_cnt4,
		int page_tuple_cnt5,
		int page_tuple_cnt6,
		int page_tuple_cnt7,
		page_buf_t buf0[PAGE_BUF_CNT],
		page_buf_t buf1[PAGE_BUF_CNT],
		page_buf_t buf2[PAGE_BUF_CNT],
		page_buf_t buf3[PAGE_BUF_CNT],
		page_buf_t buf4[PAGE_BUF_CNT],
		page_buf_t buf5[PAGE_BUF_CNT],
		page_buf_t buf6[PAGE_BUF_CNT],
		page_buf_t buf7[PAGE_BUF_CNT],
		tupdesc_t *tupdesc0,
		tupdesc_t *tupdesc1,
		tupdesc_t *tupdesc2,
		tupdesc_t *tupdesc3,
		tupdesc_t *tupdesc4,
		tupdesc_t *tupdesc5,
		tupdesc_t *tupdesc6,
		tupdesc_t *tupdesc7,
		unsigned short tup_off_array0[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off_array1[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off_array2[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off_array3[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off_array4[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off_array5[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off_array6[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off_array7[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len_array0[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len_array1[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len_array2[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len_array3[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len_array4[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len_array5[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len_array6[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len_array7[MAX_TUPLE_PER_PAGE],
		unsigned short pos_array0[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array1[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array2[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array3[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array4[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array5[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array6[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array7[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array0[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array1[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array2[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array3[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array4[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array5[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array6[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array7[MAX_TUPLE_PER_PAGE*MAX_COL]
		)
{
	get_tuple_attr_pos(buf0, blk_dat_offset, index*PE_CNT+0,
			natts, tupdesc0->desc_len_in, tupdesc0->desc_align_in, page_tuple_cnt0,
			tup_off_array0, tup_len_array0,
			pos_array0, len_array0);
	
	get_tuple_attr_pos(buf1, blk_dat_offset, index*PE_CNT+1,
			natts, tupdesc1->desc_len_in, tupdesc1->desc_align_in, page_tuple_cnt1,
			tup_off_array1, tup_len_array1,
			pos_array1, len_array1);

	get_tuple_attr_pos(buf2, blk_dat_offset, index*PE_CNT+2,
			natts, tupdesc2->desc_len_in, tupdesc2->desc_align_in, page_tuple_cnt2,
			tup_off_array2, tup_len_array2,
			pos_array2, len_array2);

	get_tuple_attr_pos(buf3, blk_dat_offset, index*PE_CNT+3,
			natts, tupdesc3->desc_len_in, tupdesc3->desc_align_in, page_tuple_cnt3,
			tup_off_array3, tup_len_array3,
			pos_array3, len_array3);

	get_tuple_attr_pos(buf4, blk_dat_offset, index*PE_CNT+4,
			natts, tupdesc4->desc_len_in, tupdesc4->desc_align_in, page_tuple_cnt4,
			tup_off_array4, tup_len_array4,
			pos_array4, len_array4);

	get_tuple_attr_pos(buf5, blk_dat_offset, index*PE_CNT+5,
			natts, tupdesc5->desc_len_in, tupdesc5->desc_align_in, page_tuple_cnt5,
			tup_off_array5, tup_len_array5,
			pos_array5, len_array5);

	get_tuple_attr_pos(buf6, blk_dat_offset, index*PE_CNT+6,
			natts, tupdesc6->desc_len_in, tupdesc6->desc_align_in, page_tuple_cnt6,
			tup_off_array6, tup_len_array6,
			pos_array6, len_array6);

	get_tuple_attr_pos(buf7, blk_dat_offset, index*PE_CNT+7,
			natts, tupdesc7->desc_len_in, tupdesc7->desc_align_in, page_tuple_cnt7,
			tup_off_array7, tup_len_array7,
			pos_array7, len_array7);


}

void filter_tuple(page_buf_t * buf, int blk_dat_offset, int page_id, int natts,
		int filter_cnt, filter_t *filter,
		int page_tuple_cnt,
		unsigned short *tup_off_array,unsigned short *tup_len_array,
		unsigned short *att_pos_array,unsigned short *att_len_array,
		int hit_off[MAX_TUPLE_PER_PAGE],
		int *hit_count_out)
{
#pragma HLS INLINE off
	unsigned char * page = (unsigned char *)buf;
	pd_linp_t *item;
	unsigned short tuplen, tupoff;

	int hit_count = 0;

	double  float8_stack[64];
	int		int_stack[64];
	char	char_stack[255];
	u8		bool_stack[64];
	u8		float8_stack_top;
	u8		int_stack_top;
	u8		char_stack_top;
	u8		bool_stack_top;

#pragma HLS BIND_STORAGE variable=float8_stack type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=int_stack type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=bool_stack type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=char_stack type=ram_2p impl=uram

	double	*float8_ptr = (double *)buf;
	int		*int_ptr = (int*)buf;
	char	*char_ptr = (char*)buf;

	filter:for(int i=0; i<page_tuple_cnt; i++){
#pragma HLS LOOP_TRIPCOUNT min=44 max=44
		tupoff = tup_off_array[i];
		tuplen = tup_len_array[i];
		float8_stack_top = 0;
		int_stack_top = 0;
		char_stack_top = 0;
		bool_stack_top = 0;
		iter_clause:for(int j=0; j<filter_cnt; j++){
#pragma HLS LOOP_TRIPCOUNT min=6 max=6
			pg_clause_op_t *op = &filter->clause[j];
			u16 op_class = op->op_class;
			if(T_OpExpr == op->op_type){
				int arg0_tag = op->arg0_tag;
				int arg0_index = op->arg0_index;
				int arg1_tag = op->arg1_tag;
				int arg1_index = op->arg1_index;

				if(C_FLOAT8 == op_class){
					double arg0, arg1;
					if(T_Var == arg0_tag){
						unsigned short arg_off = *(att_pos_array + i*MAX_COL + arg0_index);
						arg_off += tupoff;
						arg_off = arg_off >> 3;
						arg0 = float8_ptr[arg_off];
					}else if(T_Const == arg0_tag){
						arg0 = filter->const_float8[arg0_index];
					}else{
						float8_stack_top -= 1;
						unsigned short index = float8_stack_top;
						arg0 = float8_stack[index];
					}

					if(T_Var == arg1_tag){
						unsigned short arg_off = *(att_pos_array + i*MAX_COL + arg1_index);
						arg_off += tupoff;
						arg_off = arg_off >> 3;
						arg1 = float8_ptr[arg_off];
					}else if(T_Const == arg1_tag){
						arg1 = filter->const_float8[arg1_index];
					}else{
						float8_stack_top -= 1;
						unsigned short index = float8_stack_top;
						arg1 = float8_stack[index];
					}

					u16 func_id = op->func_id;
					switch(func_id){
					//float8 compare
					case FLOAT8_EQ:
						FLOAT8_CMP(==);
						break;
					case FLOAT8_NE:
						FLOAT8_CMP(!=);
						break;
					case FLOAT8_GE:
						FLOAT8_CMP(>=);
						break;
					case FLOAT8_LE:
						FLOAT8_CMP(<=);
						break;
					case FLOAT8_GT:
						FLOAT8_CMP(>);
						break;
					case FLOAT8_LT:
						FLOAT8_CMP(<);
						break;
						//float8 math
					case FLOAT8PL:
						FLOAT8_MATH(+);
						break;
					case FLOAT8MI:
						FLOAT8_MATH(-);
						break;
					case FLOAT8MUL:
						FLOAT8_MATH(*);
						break;
						//                    case FLOAT8DIV:
						//                    	FLOAT8_MATH(/);
						//                    	break;
					default:
						break;
					}
				}else if(C_INT == op_class){
					int arg0, arg1;
					if(T_Var == arg0_tag){
						unsigned short arg_off = *(att_pos_array + i*MAX_COL + arg0_index);
						arg_off += tupoff;
						arg_off = arg_off >> 2;
						arg0 = int_ptr[arg_off];
					}else if(T_Const == arg0_tag){
						arg0 = filter->const_int[arg0_index];
					}else{
						int_stack_top -= 1;
						unsigned short index = int_stack_top;
						arg0 = int_stack[index];
					}

					if(T_Var == arg1_tag){
						unsigned short arg_off = *(att_pos_array + i*MAX_COL + arg1_index);
						arg_off += tupoff;
						arg_off = arg_off >> 2;
						arg1 = int_ptr[arg_off];
					}else if(T_Const == arg1_tag){
						arg1 = filter->const_int[arg1_index];
					}else{
						int_stack_top -= 1;
						unsigned short index = int_stack_top;
						arg1 = int_stack[index];
					}

					u16 func_id = op->func_id;
					switch(func_id){
					//int compare
					case DATE_EQ:
						INT_CMP(==);
						break;
					case DATE_NE:
						INT_CMP(!=);
						break;
					case DATE_GE:
						INT_CMP(>=);
						break;
					case DATE_LE:
						INT_CMP(<=);
						break;
					case DATE_GT:
						INT_CMP(>);
						break;
					case DATE_LT:
						INT_CMP(<);
						break;
					default:
						break;
					}
				}else{
					;
				}
			}else{ //bool expr
				int nargs = op->nargs;
				bool_stack_top -= nargs;
				int sum = 0;
				bool_loop:for(int i=0; i<nargs; i++){
#pragma HLS LOOP_TRIPCOUNT min=5 max=5
					sum += bool_stack[bool_stack_top + i];
				}
				bool bool_ret;
				if(OP_AND == op->func_id){
					if(nargs == sum){
						bool_ret = 1;
					}else{
						bool_ret = 0;
					}
				}else{
					if(0 == sum){
						bool_ret = 0;
					}else{
						bool_ret = 1;
					}

				}
				bool_stack[bool_stack_top] = bool_ret;
				bool_stack_top++;
			}
		}//for(int j=0; j<filter_cnt; j++)

		bool is_hit = bool_stack[0];
		if(is_hit){
			hit_off[hit_count] = (page_id<<16)|i;
			hit_count++;
		}
	}//filter:for(int i=0; i<page_tuple_cnt; i++)

	*hit_count_out = hit_count;
}


void parallel_filter_tuple(
		int blk_dat_offset,
		int index,
		int natts,
		int filter_cnt,
		page_buf_t buf0[PAGE_BUF_CNT],
		page_buf_t buf1[PAGE_BUF_CNT],
		page_buf_t buf2[PAGE_BUF_CNT],
		page_buf_t buf3[PAGE_BUF_CNT],
		page_buf_t buf4[PAGE_BUF_CNT],
		page_buf_t buf5[PAGE_BUF_CNT],
		page_buf_t buf6[PAGE_BUF_CNT],
		page_buf_t buf7[PAGE_BUF_CNT],
		filter_t *filter0,
		filter_t *filter1,
		filter_t *filter2,
		filter_t *filter3,
		filter_t *filter4,
		filter_t *filter5,
		filter_t *filter6,
		filter_t *filter7,
		int page_tuple_cnt0,
		int page_tuple_cnt1,
		int page_tuple_cnt2,
		int page_tuple_cnt3,
		int page_tuple_cnt4,
		int page_tuple_cnt5,
		int page_tuple_cnt6,
		int page_tuple_cnt7,
		unsigned short tup_off0[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off1[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off2[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off3[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off4[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off5[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off6[MAX_TUPLE_PER_PAGE],
		unsigned short tup_off7[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len0[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len1[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len2[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len3[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len4[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len5[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len6[MAX_TUPLE_PER_PAGE],
		unsigned short tup_len7[MAX_TUPLE_PER_PAGE],
		unsigned short pos_array0[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array1[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array2[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array3[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array4[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array5[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array6[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short pos_array7[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array0[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array1[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array2[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array3[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array4[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array5[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array6[MAX_TUPLE_PER_PAGE*MAX_COL],
		unsigned short len_array7[MAX_TUPLE_PER_PAGE*MAX_COL],
		int hit_off0[MAX_TUPLE_PER_PAGE],
		int hit_off1[MAX_TUPLE_PER_PAGE],
		int hit_off2[MAX_TUPLE_PER_PAGE],
		int hit_off3[MAX_TUPLE_PER_PAGE],
		int hit_off4[MAX_TUPLE_PER_PAGE],
		int hit_off5[MAX_TUPLE_PER_PAGE],
		int hit_off6[MAX_TUPLE_PER_PAGE],
		int hit_off7[MAX_TUPLE_PER_PAGE],
		int *hit_count_out0,
		int *hit_count_out1,
		int *hit_count_out2,
		int *hit_count_out3,
		int *hit_count_out4,
		int *hit_count_out5,
		int *hit_count_out6,
		int *hit_count_out7
	  )
{
	filter_tuple(buf0, blk_dat_offset, index*PE_CNT + 0, natts,
			filter_cnt, filter0, page_tuple_cnt0,
			tup_off0, tup_len0,
			pos_array0, len_array0,
			hit_off0, hit_count_out0);

	filter_tuple(buf1, blk_dat_offset, index*PE_CNT + 1, natts,
			filter_cnt, filter1, page_tuple_cnt1,
			tup_off1, tup_len1,
			pos_array1, len_array1,
			hit_off1, hit_count_out1);

	filter_tuple(buf2, blk_dat_offset, index*PE_CNT + 2, natts,
			filter_cnt, filter2, page_tuple_cnt2,
			tup_off2, tup_len2,
			pos_array2, len_array2,
			hit_off2, hit_count_out2);

	filter_tuple(buf3, blk_dat_offset, index*PE_CNT + 3, natts,
			filter_cnt, filter3, page_tuple_cnt3,
			tup_off3, tup_len3,
			pos_array3, len_array3,
			hit_off3, hit_count_out3);

	filter_tuple(buf4, blk_dat_offset, index*PE_CNT + 4, natts,
			filter_cnt, filter4, page_tuple_cnt4,
			tup_off4, tup_len4,
			pos_array4, len_array4,
			hit_off4, hit_count_out4);

	filter_tuple(buf5, blk_dat_offset, index*PE_CNT + 5, natts,
			filter_cnt, filter5, page_tuple_cnt5,
			tup_off5, tup_len5,
			pos_array5, len_array5,
			hit_off5, hit_count_out5);

	filter_tuple(buf6, blk_dat_offset, index*PE_CNT + 6, natts,
			filter_cnt, filter6, page_tuple_cnt6,
			tup_off6, tup_len6,
			pos_array6, len_array6,
			hit_off6, hit_count_out6);

	filter_tuple(buf7, blk_dat_offset, index*PE_CNT + 7, natts,
			filter_cnt, filter7, page_tuple_cnt7,
			tup_off7, tup_len7,
			pos_array7, len_array7,
			hit_off7, hit_count_out7);

}

void write_result(
	int *tuple_off_out,
	int *tuple_cnt,
	int hit_off0[MAX_TUPLE_PER_PAGE],
	int hit_off1[MAX_TUPLE_PER_PAGE],
	int hit_off2[MAX_TUPLE_PER_PAGE],
	int hit_off3[MAX_TUPLE_PER_PAGE],
	int hit_off4[MAX_TUPLE_PER_PAGE],
	int hit_off5[MAX_TUPLE_PER_PAGE],
	int hit_off6[MAX_TUPLE_PER_PAGE],
	int hit_off7[MAX_TUPLE_PER_PAGE],
	int hit_count0,
	int hit_count1,
	int hit_count2,
	int hit_count3,
	int hit_count4,
	int hit_count5,
	int hit_count6,
	int hit_count7
	)
{
	int index = *tuple_cnt;

	write_ret0:for(int i=0; i<hit_count0; i++){
#pragma HLS LOOP_TRIPCOUNT min=2 max=2
		tuple_off_out[index++] = hit_off0[i];
	}

	write_ret1:for(int i=0; i<hit_count1; i++){
#pragma HLS LOOP_TRIPCOUNT min=2 max=2
		tuple_off_out[index++] = hit_off1[i];
	}

	write_ret2:for(int i=0; i<hit_count2; i++){
#pragma HLS LOOP_TRIPCOUNT min=2 max=2
		tuple_off_out[index++] = hit_off2[i];
	}

	write_ret3:for(int i=0; i<hit_count3; i++){
#pragma HLS LOOP_TRIPCOUNT min=2 max=2
		tuple_off_out[index++] = hit_off3[i];
	}

	write_ret4:for(int i=0; i<hit_count4; i++){
#pragma HLS LOOP_TRIPCOUNT min=2 max=2
		tuple_off_out[index++] = hit_off4[i];
	}

	write_ret5:for(int i=0; i<hit_count5; i++){
#pragma HLS LOOP_TRIPCOUNT min=2 max=2
		tuple_off_out[index++] = hit_off5[i];
	}

	write_ret6:for(int i=0; i<hit_count6; i++){
#pragma HLS LOOP_TRIPCOUNT min=2 max=2
		tuple_off_out[index++] = hit_off6[i];
	}

	write_ret7:for(int i=0; i<hit_count7; i++){
#pragma HLS LOOP_TRIPCOUNT min=2 max=2
		tuple_off_out[index++] = hit_off7[i];
	}

	*tuple_cnt = index;
}

void copy_tupdesc_and_filter(
		tupdesc_t *tupdesc0,
	       	tupdesc_t *tupdesc1,
	       	tupdesc_t *tupdesc2,
	       	tupdesc_t *tupdesc3,
		tupdesc_t *tupdesc4,
	       	tupdesc_t *tupdesc5,
	       	tupdesc_t *tupdesc6,
	       	tupdesc_t *tupdesc7,
		filter_t *filter0,
	       	filter_t *filter1,
	       	filter_t *filter2,
	       	filter_t *filter3,
		filter_t *filter4,
	       	filter_t *filter5,
	       	filter_t *filter6,
	       	filter_t *filter7,
		ap_int<512> * tupdesc_in,
	       	ap_int<512> * filter_in
		)
{
	memcpy(tupdesc0, tupdesc_in, sizeof(tupdesc_t));
	memcpy(tupdesc1, tupdesc_in, sizeof(tupdesc_t));
	memcpy(tupdesc2, tupdesc_in, sizeof(tupdesc_t));
	memcpy(tupdesc3, tupdesc_in, sizeof(tupdesc_t));
	memcpy(tupdesc4, tupdesc_in, sizeof(tupdesc_t));
	memcpy(tupdesc5, tupdesc_in, sizeof(tupdesc_t));
	memcpy(tupdesc6, tupdesc_in, sizeof(tupdesc_t));
	memcpy(tupdesc7, tupdesc_in, sizeof(tupdesc_t));

	memcpy(filter0, filter_in, sizeof(filter_t));
	memcpy(filter1, filter_in, sizeof(filter_t));
	memcpy(filter2, filter_in, sizeof(filter_t));
	memcpy(filter3, filter_in, sizeof(filter_t));
	memcpy(filter4, filter_in, sizeof(filter_t));
	memcpy(filter5, filter_in, sizeof(filter_t));
	memcpy(filter6, filter_in, sizeof(filter_t));
	memcpy(filter7, filter_in, sizeof(filter_t));
}

void pg_filter(ap_int<512> *pg_data_in, int blk_dat_offset, int page_cnt, int natts, int filter_cnt,
		ap_int<512> * tupdesc_in, ap_int<512> * filter_in,
		int *tuple_cnt_out, int *tuple_off_out)
{
#pragma HLS interface m_axi port=pg_data_in offset=slave bundle=gmem0
#pragma HLS interface m_axi port=tupdesc_in offset=slave bundle=gmem0
#pragma HLS interface m_axi port=filter_in offset=slave bundle=gmem0
#pragma HLS interface m_axi port=tuple_cnt_out offset=slave bundle=gmem0
#pragma HLS interface m_axi port=tuple_off_out offset=slave bundle=gmem0

	tupdesc_t       tupdesc0;
        tupdesc_t       tupdesc1;
        tupdesc_t       tupdesc2;
        tupdesc_t       tupdesc3;
        tupdesc_t       tupdesc4;
        tupdesc_t       tupdesc5;
        tupdesc_t       tupdesc6;
        tupdesc_t       tupdesc7;

	filter_t        filter0;
        filter_t        filter1;
        filter_t        filter2;
        filter_t        filter3;
        filter_t        filter4;
        filter_t        filter5;
        filter_t        filter6;
        filter_t        filter7;
	int		tuple_cnt = 0;

	copy_tupdesc_and_filter(
			&tupdesc0,
		       	&tupdesc1,
		       	&tupdesc2,
		       	&tupdesc3,
			&tupdesc4,
		       	&tupdesc5,
		       	&tupdesc6,
		       	&tupdesc7,
			&filter0,
		       	&filter1,
		       	&filter2,
		       	&filter3,
			&filter4,
		       	&filter5,
		       	&filter6,
		       	&filter7,
			tupdesc_in,
		       	filter_in
			);


	int loop_cnt = page_cnt/PE_CNT;
	iter_pages:for(int i=0; i<loop_cnt; i++){
#pragma HLS LOOP_TRIPCOUNT min=100 max=100
#pragma HLS DATAFLOW
		page_buf_t local_buf0[PAGE_BUF_CNT];
		page_buf_t local_buf1[PAGE_BUF_CNT];
		page_buf_t local_buf2[PAGE_BUF_CNT];
		page_buf_t local_buf3[PAGE_BUF_CNT];
		page_buf_t local_buf4[PAGE_BUF_CNT];
		page_buf_t local_buf5[PAGE_BUF_CNT];
		page_buf_t local_buf6[PAGE_BUF_CNT];
		page_buf_t local_buf7[PAGE_BUF_CNT];
//#pragma HLS BIND_STORAGE variable=local_buf0 type=ram_2p impl=uram
//#pragma HLS BIND_STORAGE variable=local_buf1 type=ram_2p impl=uram
//#pragma HLS BIND_STORAGE variable=local_buf2 type=ram_2p impl=uram
//#pragma HLS BIND_STORAGE variable=local_buf3 type=ram_2p impl=uram
//#pragma HLS BIND_STORAGE variable=local_buf4 type=ram_2p impl=uram
//#pragma HLS BIND_STORAGE variable=local_buf5 type=ram_2p impl=uram
//#pragma HLS BIND_STORAGE variable=local_buf6 type=ram_2p impl=uram
//#pragma HLS BIND_STORAGE variable=local_buf7 type=ram_2p impl=uram
		int page_tuple_cnt0;
		int page_tuple_cnt1;
		int page_tuple_cnt2;
		int page_tuple_cnt3;
		int page_tuple_cnt4;
		int page_tuple_cnt5;
		int page_tuple_cnt6;
		int page_tuple_cnt7;
		copy_to_local_buf(
				local_buf0,
				local_buf1,
				local_buf2,
				local_buf3,
				local_buf4,
				local_buf5,
				local_buf6,
				local_buf7,
				&page_tuple_cnt0,
				&page_tuple_cnt1,
				&page_tuple_cnt2,
				&page_tuple_cnt3,
				&page_tuple_cnt4,
				&page_tuple_cnt5,
				&page_tuple_cnt6,
				&page_tuple_cnt7,
				pg_data_in,
				i
				);

		page_buf_t page_pos_buf0[PAGE_BUF_CNT];
		page_buf_t page_pos_buf1[PAGE_BUF_CNT];
		page_buf_t page_pos_buf2[PAGE_BUF_CNT];
		page_buf_t page_pos_buf3[PAGE_BUF_CNT];
		page_buf_t page_pos_buf4[PAGE_BUF_CNT];
		page_buf_t page_pos_buf5[PAGE_BUF_CNT];
		page_buf_t page_pos_buf6[PAGE_BUF_CNT];
		page_buf_t page_pos_buf7[PAGE_BUF_CNT];
#pragma HLS BIND_STORAGE variable=page_pos_buf0 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_pos_buf1 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_pos_buf2 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_pos_buf3 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_pos_buf4 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_pos_buf5 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_pos_buf6 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_pos_buf7 type=ram_2p impl=uram

		page_buf_t page_filter_buf0[PAGE_BUF_CNT];
		page_buf_t page_filter_buf1[PAGE_BUF_CNT];
		page_buf_t page_filter_buf2[PAGE_BUF_CNT];
		page_buf_t page_filter_buf3[PAGE_BUF_CNT];
		page_buf_t page_filter_buf4[PAGE_BUF_CNT];
		page_buf_t page_filter_buf5[PAGE_BUF_CNT];
		page_buf_t page_filter_buf6[PAGE_BUF_CNT];
		page_buf_t page_filter_buf7[PAGE_BUF_CNT];
#pragma HLS BIND_STORAGE variable=page_filter_buf0 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_filter_buf1 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_filter_buf2 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_filter_buf3 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_filter_buf4 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_filter_buf5 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_filter_buf6 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=page_filter_buf7 type=ram_2p impl=uram
//#pragma HLS STREAM variable=page_filter_buf0 depth=3 dim=1
//#pragma HLS STREAM variable=page_filter_buf1 depth=3 dim=1
//#pragma HLS STREAM variable=page_filter_buf2 depth=3 dim=1
//#pragma HLS STREAM variable=page_filter_buf3 depth=3 dim=1
//#pragma HLS STREAM variable=page_filter_buf4 depth=3 dim=1
//#pragma HLS STREAM variable=page_filter_buf5 depth=3 dim=1
//#pragma HLS STREAM variable=page_filter_buf6 depth=3 dim=1
//#pragma HLS STREAM variable=page_filter_buf7 depth=3 dim=1
		duplicate_buf(
				local_buf0,
				local_buf1,
				local_buf2,
				local_buf3,
				local_buf4,
				local_buf5,
				local_buf6,
				local_buf7,
				page_pos_buf0,
				page_pos_buf1,
				page_pos_buf2,
				page_pos_buf3,
				page_pos_buf4,
				page_pos_buf5,
				page_pos_buf6,
				page_pos_buf7,
				page_filter_buf0,
				page_filter_buf1,
				page_filter_buf2,
				page_filter_buf3,
				page_filter_buf4,
				page_filter_buf5,
				page_filter_buf6,
				page_filter_buf7
			     );

		unsigned short tup_off_array0[MAX_TUPLE_PER_PAGE];
		unsigned short tup_off_array1[MAX_TUPLE_PER_PAGE];
		unsigned short tup_off_array2[MAX_TUPLE_PER_PAGE];
		unsigned short tup_off_array3[MAX_TUPLE_PER_PAGE];
		unsigned short tup_off_array4[MAX_TUPLE_PER_PAGE];
		unsigned short tup_off_array5[MAX_TUPLE_PER_PAGE];
		unsigned short tup_off_array6[MAX_TUPLE_PER_PAGE];
		unsigned short tup_off_array7[MAX_TUPLE_PER_PAGE];
#pragma HLS BIND_STORAGE variable=tup_off_array0 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_off_array1 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_off_array2 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_off_array3 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_off_array4 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_off_array5 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_off_array6 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_off_array7 type=ram_2p impl=uram

		unsigned short tup_len_array0[MAX_TUPLE_PER_PAGE];
		unsigned short tup_len_array1[MAX_TUPLE_PER_PAGE];
		unsigned short tup_len_array2[MAX_TUPLE_PER_PAGE];
		unsigned short tup_len_array3[MAX_TUPLE_PER_PAGE];
		unsigned short tup_len_array4[MAX_TUPLE_PER_PAGE];
		unsigned short tup_len_array5[MAX_TUPLE_PER_PAGE];
		unsigned short tup_len_array6[MAX_TUPLE_PER_PAGE];
		unsigned short tup_len_array7[MAX_TUPLE_PER_PAGE];
#pragma HLS BIND_STORAGE variable=tup_len_array0 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_len_array1 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_len_array2 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_len_array3 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_len_array4 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_len_array5 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_len_array6 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=tup_len_array7 type=ram_2p impl=uram

		unsigned short pos_array0[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short pos_array1[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short pos_array2[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short pos_array3[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short pos_array4[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short pos_array5[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short pos_array6[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short pos_array7[MAX_TUPLE_PER_PAGE*MAX_COL];
#pragma HLS BIND_STORAGE variable=pos_array0 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=pos_array1 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=pos_array2 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=pos_array3 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=pos_array4 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=pos_array6 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=pos_array7 type=ram_2p impl=uram

		unsigned short len_array0[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short len_array1[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short len_array2[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short len_array3[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short len_array4[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short len_array5[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short len_array6[MAX_TUPLE_PER_PAGE*MAX_COL];
		unsigned short len_array7[MAX_TUPLE_PER_PAGE*MAX_COL];
#pragma HLS BIND_STORAGE variable=len_array0 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=len_array1 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=len_array2 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=len_array3 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=len_array4 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=len_array5 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=len_array6 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=len_array7 type=ram_2p impl=uram

		parallel_get_tuple_attr_pos(
				blk_dat_offset,
				i,
				natts,
				page_tuple_cnt0,
				page_tuple_cnt1,
				page_tuple_cnt2,
				page_tuple_cnt3,
				page_tuple_cnt4,
				page_tuple_cnt5,
				page_tuple_cnt6,
				page_tuple_cnt7,
				page_pos_buf0, 
				page_pos_buf1, 
				page_pos_buf2, 
				page_pos_buf3, 
				page_pos_buf4, 
				page_pos_buf5, 
				page_pos_buf6, 
				page_pos_buf7, 
				&tupdesc0,
				&tupdesc1,
				&tupdesc2,
				&tupdesc3,
				&tupdesc4,
				&tupdesc5,
				&tupdesc6,
				&tupdesc7,
				tup_off_array0,
				tup_off_array1,
				tup_off_array2,
				tup_off_array3,
				tup_off_array4,
				tup_off_array5,
				tup_off_array6,
				tup_off_array7,
				tup_len_array0,
				tup_len_array1,
				tup_len_array2,
				tup_len_array3,
				tup_len_array4,
				tup_len_array5,
				tup_len_array6,
				tup_len_array7,
				pos_array0,
				pos_array1,
				pos_array2,
				pos_array3,
				pos_array4,
				pos_array5,
				pos_array6,
				pos_array7,
				len_array0,
				len_array1,
				len_array2,
				len_array3,
				len_array4,
				len_array5,
				len_array6,
				len_array7
				);


		int hit_off0[MAX_TUPLE_PER_PAGE];
		int hit_off1[MAX_TUPLE_PER_PAGE];
		int hit_off2[MAX_TUPLE_PER_PAGE];
		int hit_off3[MAX_TUPLE_PER_PAGE];
		int hit_off4[MAX_TUPLE_PER_PAGE];
		int hit_off5[MAX_TUPLE_PER_PAGE];
		int hit_off6[MAX_TUPLE_PER_PAGE];
		int hit_off7[MAX_TUPLE_PER_PAGE];
#pragma HLS BIND_STORAGE variable=hit_off0 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=hit_off1 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=hit_off2 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=hit_off3 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=hit_off4 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=hit_off5 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=hit_off6 type=ram_2p impl=uram
#pragma HLS BIND_STORAGE variable=hit_off7 type=ram_2p impl=uram
		int hit_count_out0;
		int hit_count_out1;
		int hit_count_out2;
		int hit_count_out3;
		int hit_count_out4;
		int hit_count_out5;
		int hit_count_out6;
		int hit_count_out7;
		parallel_filter_tuple(
			blk_dat_offset,
			i,
			natts,
			filter_cnt,
			page_filter_buf0,
			page_filter_buf1,
			page_filter_buf2,
			page_filter_buf3,
			page_filter_buf4,
			page_filter_buf5,
			page_filter_buf6,
			page_filter_buf7,
			&filter0,
			&filter1,
			&filter2,
			&filter3,
			&filter4,
			&filter5,
			&filter6,
			&filter7,
			page_tuple_cnt0,
			page_tuple_cnt1,
			page_tuple_cnt2,
			page_tuple_cnt3,
			page_tuple_cnt4,
			page_tuple_cnt5,
			page_tuple_cnt6,
			page_tuple_cnt7,
			tup_off_array0,
			tup_off_array1,
			tup_off_array2,
			tup_off_array3,
			tup_off_array4,
			tup_off_array5,
			tup_off_array6,
			tup_off_array7,
			tup_len_array0,
			tup_len_array1,
			tup_len_array2,
			tup_len_array3,
			tup_len_array4,
			tup_len_array5,
			tup_len_array6,
			tup_len_array7,
			pos_array0,
			pos_array1,
			pos_array2,
			pos_array3,
			pos_array4,
			pos_array5,
			pos_array6,
			pos_array7,
			len_array0,
			len_array1,
			len_array2,
			len_array3,
			len_array4,
			len_array5,
			len_array6,
			len_array7,
			hit_off0,
			hit_off1,
			hit_off2,
			hit_off3,
			hit_off4,
			hit_off5,
			hit_off6,
			hit_off7,
			&hit_count_out0,
			&hit_count_out1,
			&hit_count_out2,
			&hit_count_out3,
			&hit_count_out4,
			&hit_count_out5,
			&hit_count_out6,
			&hit_count_out7
		);

		write_result(
			tuple_off_out,
			&tuple_cnt,
			hit_off0,
			hit_off1,
			hit_off2,
			hit_off3,
			hit_off4,
			hit_off5,
			hit_off6,
			hit_off7,
			hit_count_out0,
			hit_count_out1,
			hit_count_out2,
			hit_count_out3,
			hit_count_out4,
			hit_count_out5,
			hit_count_out6,
			hit_count_out7
			);

	}

	*tuple_cnt_out = tuple_cnt;
}

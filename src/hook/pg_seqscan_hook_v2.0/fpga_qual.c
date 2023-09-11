#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "hlog.h"
#include "fpga_qual.h"

typedef struct filter_state{
	int	max_attr_index;
	int	op_cnt;	
	int	const_float8_index;
	int	const_int_index;
	int	const_char_index;
}filter_state_t;

__debug_use static int stack_count = 0;

__debug_use static char *get_var_tag(int type)
{
        static char *src_array[] = {"tuple", "const", "stack"};
        if(T_Var == type){
                return src_array[0];
        }else if(T_Const == type){
                return src_array[1];
        }else if(T_OpExpr == type){
                return src_array[2];
        }else{
                return NULL;
        }
}

__debug_use static const FmgrBuiltin * fmgr_isbuiltin(Oid id)
{
        uint16          index;

        /* fast lookup only possible if original oid still assigned */
        if (id >= FirstBootstrapObjectId)
                return NULL;

        /*
         * Lookup function data. If there's a miss in that range it's likely a
         * nonexistant function, returning NULL here will trigger an ERROR later.
         */
        index = fmgr_builtin_oid_index[id];
        if (index == InvalidOidBuiltinMapping)
                return NULL;

        return &fmgr_builtins[index];
}

static DateADT change_timestamp_to_date(Timestamp timestamp)
{
        struct pg_tm tt, *tm = &tt;
        fsec_t          fsec;
        DateADT date;

        timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL);

        date = DirectFunctionCall3(make_date, tm->tm_year, tm->tm_mon, tm->tm_mday);
        return date;
}


static void show_filter(filter_t *filter, filter_state_t *filter_stat)
{
	__debug_use int op_cnt = filter_stat->op_cnt;
	__debug_use int max_attr_index =filter_stat->max_attr_index;

	__debug_use pg_clause_op_t *clause;

	DBG_INFO("op_cnt=%d, max_attr_index=%d\n", op_cnt, max_attr_index);

	for(int i=0; i<op_cnt; i++){
		clause = &filter->clause[i];
		DBG_INFO("op_type=%d, op_class=%d, funcid=%d, nargs=%d\n\t arg0_tag=%d, arg0_index=%d, arg0_len=%d\n\t arg1_tag=%d, arg1_index=%d, arg1_len=%d\n",
				clause->op_type, clause->op_class, clause->func_id, clause->nargs,
				clause->arg0_tag, clause->arg0_index, clause->arg0_len,
				clause->arg1_tag, clause->arg1_index, clause->arg1_len);
	}

}

static int func_id_rewrite(int func_id)
{
	switch(func_id){
		//date_xx_timestamp
		case  DATE_LT_TIMESTAMP: //date_lt_timestamp
			func_id = DATE_LT; //date_lt
			break;

		case  DATE_LE_TIMESTAMP: //date_le_timestamp
			func_id = DATE_LE; //date_le
			break;

		case  DATE_EQ_TIMESTAMP: //date_eq_timestamp
			func_id = DATE_EQ; //date_eq
			break;

		case  DATE_GT_TIMESTAMP: //date_gt_timestamp
			func_id = DATE_GT; //date_gt
			break;

		case  DATE_GE_TIMESTAMP: //date_ge_timestamp
			func_id = DATE_GE; //date_ge
			break;

		case  DATE_NE_TIMESTAMP: //date_ne_timestamp
			func_id = DATE_NE; //date_ne
			break;

		case  DATE_CMP_TIMESTAMP: //date_cmp_timestamp
			func_id = DATE_CMP; //date_cmp
			break;
						//timestamp_xx_date
		case  TIMESTAMP_LT_DATE: //timestamp_lt_date
			func_id = DATE_LT; //date_lt
			break;

		case  TIMESTAMP_LE_DATE: //timestamp_le_date
			func_id = DATE_LE; //date_le
			break;

		case  TIMESTAMP_EQ_DATE: //timestamp_eq_date
			func_id = DATE_EQ; //date_eq
			break;

		case  TIMESTAMP_GT_DATE: //timestamp_gt_date
			func_id = DATE_GT; //date_gt
			break;

		case  TIMESTAMP_GE_DATE: //timestamp_ge_date
			func_id = DATE_GE; //date_ge
			break;

		case  TIMESTAMP_NE_DATE: //timestamp_ne_date
			func_id = DATE_NE; //date_ne
			break;

		case  TIMESTAMP_CMP_DATE: //timestamp_cmp_date
			func_id = DATE_CMP; //date_cmp
			break;

		default:
			break;
	}
	return func_id;
}

static int get_var_size_by_type(int type)
{
        int size = -1;
        switch(type){
                case INT4OID:
                        size = 4;
                        break;
                case FLOAT8OID:
                        size = 8;
                        break;
                case DATEOID:
                        size = 4;
                        break;
                default:
                        size = -1;
        }

        return size;
}

static int get_op_class_by_funcid(int funcid)
{
	int ret = C_INT;
        switch(funcid){
		//int_logic
		case INT4MUL:
		case INT4DIV:
		case INT4PL:
		case INT4MI:
                //date_logic
                case DATE_EQ:
                case DATE_LT:
                case DATE_LE:
                case DATE_GT:
                case DATE_GE:
                case DATE_NE:
		//date timestamp logic
		case DATE_LT_TIMESTAMP:
		case DATE_LE_TIMESTAMP:
		case DATE_EQ_TIMESTAMP:
		case DATE_GT_TIMESTAMP:
		case DATE_GE_TIMESTAMP:
		case DATE_NE_TIMESTAMP:
		case DATE_CMP_TIMESTAMP:
		case TIMESTAMP_LT_DATE:
		case TIMESTAMP_LE_DATE:
		case TIMESTAMP_EQ_DATE:
		case TIMESTAMP_GT_DATE:
		case TIMESTAMP_GE_DATE:
		case TIMESTAMP_NE_DATE:
		case TIMESTAMP_CMP_DATE:
			ret = C_INT;
                        break;
                //float8_logic
                case FLOAT8_EQ:
                case FLOAT8_NE:
                case FLOAT8_LT:
                case FLOAT8_LE:
                case FLOAT8_GT:
                case FLOAT8_GE:
                //float8_logic
                case FLOAT8MUL:
                case FLOAT8DIV:
                case FLOAT8PL:
                case FLOAT8MI:
			ret = C_FLOAT8;
                        break;

                default:
                        break;
        }
	return ret;
}

static void copy_constval(char *buf, Const *dconst)
{
        if(dconst->constbyval){
                memcpy(buf, (char *)&dconst->constvalue, dconst->constlen);
        }else{
                memcpy(buf, (char *)dconst->constvalue, dconst->constlen);
        }
}

static int get_func_arg_info(int funcid, u16 *arg_index, u16 *arg_len)
{
        switch(funcid){
		//int_logic
		case INT4MUL:
		case INT4DIV:
		case INT4PL:
		case INT4MI:
                //date_logic
                case DATE_EQ:
                case DATE_LT:
                case DATE_LE:
                case DATE_GT:
                case DATE_GE:
                case DATE_NE:
		case DATE_LT_TIMESTAMP:
		case DATE_LE_TIMESTAMP:
		case DATE_EQ_TIMESTAMP:
		case DATE_GT_TIMESTAMP:
		case DATE_GE_TIMESTAMP:
		case DATE_NE_TIMESTAMP:
		case DATE_CMP_TIMESTAMP:
		case TIMESTAMP_LT_DATE:
		case TIMESTAMP_LE_DATE:
		case TIMESTAMP_EQ_DATE:
		case TIMESTAMP_GT_DATE:
		case TIMESTAMP_GE_DATE:
		case TIMESTAMP_NE_DATE:
		case TIMESTAMP_CMP_DATE:
			*arg_index = 0;
			*arg_len = 4;
			break;
                //float8_logic
                case FLOAT8_EQ:
                case FLOAT8_NE:
                case FLOAT8_LT:
                case FLOAT8_LE:
                case FLOAT8_GT:
                case FLOAT8_GE:
                //float8_logic
                case FLOAT8MUL:
                case FLOAT8DIV:
                case FLOAT8PL:
                case FLOAT8MI:
                        *arg_index = 0;
                        *arg_len = 8;
                        break;

                default:
                        break;
        }
        return 0;
}


static int set_oparg(Node *arg, u8 *arg_tag, u16 *arg_index, u16* arg_len, filter_t *filter, filter_state_t *filter_stat)
{
	int op_index = filter_stat->op_cnt;
	pg_clause_op_t *clause = &filter->clause[op_index];
	int op_class;
	int tag;

	op_class = clause->op_class; 

	tag = nodeTag(arg);
        if(T_Var == tag){
                Var *var = (Var*)arg;
		*arg_index = var->varattno - 1;
		*arg_len = get_var_size_by_type(var->vartype);
		if(*arg_index > filter_stat->max_attr_index){
			filter_stat->max_attr_index = *arg_index;
		}
        }else if(T_Const == tag){
	        Const *dconst = (Const*)arg;
		int arg_type;
		if(C_INT == op_class){
			*arg_index = filter_stat->const_int_index++;
			arg_type = dconst->consttype;
			*arg_len = 4; 
			if(TIMESTAMPOID == arg_type){
				DateADT date;
				Timestamp timestamp;
				copy_constval((char *)&timestamp, dconst);
				date = change_timestamp_to_date(timestamp);
				memcpy(&filter->const_int[*arg_index], &date, sizeof(int));
			}else{
				copy_constval((char *)&filter->const_int[*arg_index], dconst);
			}
		}
		if(C_FLOAT8 == op_class){
			*arg_index = filter_stat->const_float8_index++;
			*arg_len = dconst->constlen; 
			copy_constval((char *)&filter->const_float8[*arg_index], dconst);
		}
	}else{
		get_func_arg_info(clause->func_id, arg_index, arg_len);
	}

	*arg_tag = tag;
	return 0;
}

static void assign_opexpr(Node *expr, filter_t *filter, filter_state_t *filter_stat)
{
        OpExpr* opexpr = (OpExpr*) expr;
        Node*   arg0;
        Node*   arg1;
	int op_class;
	int op_index = filter_stat->op_cnt;
	pg_clause_op_t *clause = &filter->clause[op_index];

        arg0 = linitial(opexpr->args);
        arg1 = lsecond(opexpr->args);

	op_class = get_op_class_by_funcid(opexpr->opfuncid);

	clause->op_type = T_OpExpr;
	clause->op_class = op_class;
	clause->func_id = func_id_rewrite(opexpr->opfuncid);
	clause->nargs = 2;

	set_oparg(arg0, &clause->arg0_tag, &clause->arg0_index, &clause->arg0_len, filter, filter_stat);
	set_oparg(arg1, &clause->arg1_tag, &clause->arg1_index, &clause->arg1_len, filter, filter_stat);

	filter_stat->op_cnt++;

        return ;
}


static void assign_boolexpr(Node *expr, filter_t *filter, filter_state_t *filter_stat)
{
	BoolExpr* boolexpr = (BoolExpr*) expr;
	int     nargs = list_length(boolexpr->args);
	int op_index = filter_stat->op_cnt;
	pg_clause_op_t *clause = &filter->clause[op_index];

	clause->op_type = T_BoolExpr;
	clause->nargs = nargs;
	if(AND_EXPR == boolexpr->boolop){
		clause->func_id = AND_EXPR;
        }else if(OR_EXPR == boolexpr->boolop){
		clause->func_id = OR_EXPR;
        }else{
		clause->func_id = NOT_EXPR;
        }

	filter_stat->op_cnt++;

	return ;
}

static void assign_and_boolexpr(int nargs, filter_t *filter, filter_state_t *filter_stat)
{
	int op_index = filter_stat->op_cnt;
	pg_clause_op_t *clause = &filter->clause[op_index];

	clause->op_type = T_BoolExpr;
	clause->nargs = nargs;
	clause->func_id = AND_EXPR;

	filter_stat->op_cnt++;
}

static void walk_expr_for_fpga(Node *expr, filter_t *filter, filter_state_t *filter_stat)
{
	if(T_OpExpr == nodeTag(expr)){
		OpExpr* opexpr = (OpExpr*) expr;
		Node*   arg0;
		Node*   arg1;
		int arg0_tag, arg1_tag;

		arg0 = linitial(opexpr->args);
		arg1 = lsecond(opexpr->args);

		arg0_tag = nodeTag(arg0);
		arg1_tag = nodeTag(arg1);

		//leaf node: VV
		if((arg0_tag==T_Var) &&  (arg1_tag==T_Var)){
			assign_opexpr(expr, filter, filter_stat);
			stack_count++;
			DBG_INFO("leaf node VV, func=%s, opexpr->opno=%d, opexpr->opfuncid=%d, arg0_tag=%d[%s], arg1_tag=%d[%s], (stack_count=%d)\n", 
					fmgr_isbuiltin(opexpr->opfuncid)->funcName, opexpr->opno, opexpr->opfuncid, arg0_tag, get_var_tag(arg0_tag), arg1_tag, get_var_tag(arg1_tag), stack_count);
			return ;
		}

		//leaf node: VC
		if((arg0_tag==T_Var) &&  (arg1_tag==T_Const)){
			assign_opexpr(expr, filter, filter_stat);
			stack_count++;
			DBG_INFO("leaf node VC, func=%s, opexpr->opno=%d, opexpr->opfuncid=%d, arg0_tag=%d[%s], arg1_tag=%d[%s], (stack_count=%d)\n", 
					fmgr_isbuiltin(opexpr->opfuncid)->funcName, opexpr->opno, opexpr->opfuncid, arg0_tag, get_var_tag(arg0_tag), arg1_tag, get_var_tag(arg1_tag), stack_count);
			return ;
		}

		//leaf node: CV
		if((arg0_tag==T_Const) &&  (arg1_tag==T_Var)){
			assign_opexpr(expr, filter, filter_stat);
			stack_count++;
			DBG_INFO("leaf node CV, func=%s, opexpr->opno=%d, opexpr->opfuncid=%d, arg0_tag=%d[%s], arg1_tag=%d[%s], (stack_count=%d)\n", 
					fmgr_isbuiltin(opexpr->opfuncid)->funcName, opexpr->opno, opexpr->opfuncid, arg0_tag, get_var_tag(arg0_tag), arg1_tag, get_var_tag(arg1_tag), stack_count);
			return ;
		}

		//leaf node: CC
		if((arg0_tag==T_Const) &&  (arg1_tag==T_Const)){
			assign_opexpr(expr, filter, filter_stat);
			stack_count++;
			DBG_INFO("leaf node CC, func=%s, opexpr->opno=%d, opexpr->opfuncid=%d, arg0_tag=%d[%s], arg1_tag=%d[%s], (stack_count=%d)\n", 
					fmgr_isbuiltin(opexpr->opfuncid)->funcName, opexpr->opno, opexpr->opfuncid, arg0_tag, get_var_tag(arg0_tag), arg1_tag, get_var_tag(arg1_tag), stack_count);
			return ;
		}


		//oo node: OO
		if( (arg0_tag==T_OpExpr) && (arg1_tag==T_OpExpr) ){
			walk_expr_for_fpga(arg0, filter, filter_stat);
			walk_expr_for_fpga(arg1, filter, filter_stat);
			assign_opexpr(expr, filter, filter_stat);
			stack_count--;
			DBG_INFO("middle node OO, func=%s, opexpr->opno=%d, opexpr->opfuncid=%d, arg0_tag=%d[%s], arg1_tag=%d[%s], (stack_count=%d)\n", 
					fmgr_isbuiltin(opexpr->opfuncid)->funcName, opexpr->opno, opexpr->opfuncid, arg0_tag, get_var_tag(arg0_tag), arg1_tag, get_var_tag(arg1_tag), stack_count);
			return ;
		}

		//calc node:CO, VO, OC, OV
		if(arg0_tag == T_Const){
			walk_expr_for_fpga(arg1, filter, filter_stat);
			assign_opexpr(expr, filter, filter_stat);
			DBG_INFO("middle node CO, func=%s, opexpr->opno=%d, opexpr->opfuncid=%d, arg0_tag=%d[%s], arg1_tag=%d[%s], (stack_count=%d)\n", 
					fmgr_isbuiltin(opexpr->opfuncid)->funcName, opexpr->opno, opexpr->opfuncid, arg0_tag, get_var_tag(arg0_tag), arg1_tag, get_var_tag(arg1_tag), stack_count);
		}else if(arg0_tag == T_Var){
			walk_expr_for_fpga(arg1, filter, filter_stat);
			assign_opexpr(expr, filter, filter_stat);
			DBG_INFO("middle node VO, func=%s, opexpr->opno=%d, opexpr->opfuncid=%d, arg0_tag=%d[%s], arg1_tag=%d[%s], (stack_count=%d)\n", 
					fmgr_isbuiltin(opexpr->opfuncid)->funcName, opexpr->opno, opexpr->opfuncid, arg0_tag, get_var_tag(arg0_tag), arg1_tag, get_var_tag(arg1_tag), stack_count);
		}else if(arg1_tag == T_Const){
			walk_expr_for_fpga(arg0, filter, filter_stat);
			assign_opexpr(expr, filter, filter_stat);
			DBG_INFO("middle node OC, func=%s, opexpr->opno=%d, opexpr->opfuncid=%d, arg0_tag=%d[%s], arg1_tag=%d[%s], (stack_count=%d)\n", 
					fmgr_isbuiltin(opexpr->opfuncid)->funcName, opexpr->opno, opexpr->opfuncid, arg0_tag, get_var_tag(arg0_tag), arg1_tag, get_var_tag(arg1_tag), stack_count);
		}else if(arg1_tag == T_Var){
			walk_expr_for_fpga(arg0, filter, filter_stat);
			assign_opexpr(expr, filter, filter_stat);
			DBG_INFO("middle node OV, func=%s, opexpr->opno=%d, opexpr->opfuncid=%d, arg0_tag=%d[%s], arg1_tag=%d[%s], (stack_count=%d)\n", 
					fmgr_isbuiltin(opexpr->opfuncid)->funcName, opexpr->opno, opexpr->opfuncid, arg0_tag, get_var_tag(arg0_tag), arg1_tag, get_var_tag(arg1_tag), stack_count);
		}else{
			DBG_INFO("[error] impossible happen, T_OpExpr nargs not T_Var or T_Const: arg0_tag=%d[%s], arg1_tag=%d[%s]\n", 
					arg0_tag, get_var_tag(arg0_tag), arg1_tag, get_var_tag(arg1_tag));
		}
	}else if(T_BoolExpr == nodeTag(expr)){
		BoolExpr   *boolexpr = (BoolExpr *) expr;
		int	nargs = list_length(boolexpr->args);
		ListCell*       lc;
		Node*   arg;
		int	type;
		foreach(lc, boolexpr->args){
			arg = lfirst(lc);
			type = nodeTag(arg);
			if(T_OpExpr == type){
				walk_expr_for_fpga(arg, filter, filter_stat);
			}else if(T_BoolExpr == type){
				walk_expr_for_fpga(arg, filter, filter_stat);
			}else{
				DBG_INFO("unknow type %d\n", type);
				return ;
			}
		}
		if(AND_EXPR == boolexpr->boolop){
			stack_count -= nargs;
			stack_count++;
			assign_boolexpr(expr,filter,  filter_stat);
			DBG_INFO("T_BoolExpr[AND]: type = %d, pop nargs = %d, stack_count=%d\n", boolexpr->boolop, nargs, stack_count);
		}else if(OR_EXPR == boolexpr->boolop){
			stack_count -= nargs;
			stack_count++;
			assign_boolexpr(expr,filter,  filter_stat);
			DBG_INFO("T_BoolExpr[OR]: type = %d, pop nargs = %d, stack_count=%d\n", boolexpr->boolop, nargs, stack_count);
		}else{
			stack_count -= nargs;
			stack_count++;
			assign_boolexpr(expr,filter,  filter_stat);
			DBG_INFO("T_BoolExpr[NOT]: type = %d, pop nargs = %d, stack_count=%d\n", boolexpr->boolop, nargs, stack_count);
		}

	}else{
		DBG_INFO("unknow node type %d\n", nodeTag(expr));
	}

	return ;
}

int create_fpga_filter(RelOptInfo* baserel, filter_t *filter, int *max_attr_index, int *op_cnt)
{
	ListCell*       lc;
	int                 nargs = 0;

	filter_state_t filter_stat;
	memset(&filter_stat, 0, sizeof(filter_state_t));

	stack_count = 0;
	foreach (lc, baserel->baserestrictinfo) {
		RestrictInfo* rinfo = (RestrictInfo*) lfirst (lc);
		if (!IsA (rinfo, RestrictInfo)) {
			continue;
		}
		walk_expr_for_fpga((Node*)rinfo->clause, filter, &filter_stat);
	}

	nargs = list_length(baserel->baserestrictinfo);

	assign_and_boolexpr(nargs, filter, &filter_stat);

	*max_attr_index = filter_stat.max_attr_index;
	*op_cnt = filter_stat.op_cnt;

	show_filter(filter, &filter_stat);
	return 0;
}


static void check_func_id(int oid, int *is_support)
{
        if(func_id_support_hash[oid] == 0){
                DBG_INFO("func is not support, oid=%d, name=%s\n", oid, fmgr_isbuiltin(oid)->funcName);
                *is_support = 0;
        }
}

void fpga_ops_check(Node* expr, int *is_support)
{
	
	if( T_OpExpr == nodeTag(expr) ){
		OpExpr* opexpr = (OpExpr*) expr;
                Node*   arg0;
                Node*   arg1;
                int arg0_tag, arg1_tag;

		if (list_length(opexpr->args) != 2) {
			DBG_INFO("list_length(opexpr->args) != 2 return %d\n", list_length(opexpr->args));
			*is_support = 0;
			return ;
		}

                arg0 = linitial(opexpr->args);
                arg1 = lsecond(opexpr->args);
                arg0_tag = nodeTag(arg0);
                arg1_tag = nodeTag(arg1);

                //leaf node: VV
                if((arg0_tag==T_Var) &&  (arg1_tag==T_Var)){
			check_func_id(opexpr->opfuncid, is_support);
                        return ;
                }

                //leaf node: VC
                if((arg0_tag==T_Var) &&  (arg1_tag==T_Const)){
			check_func_id(opexpr->opfuncid, is_support);
                        return ;
                }

                //leaf node: CV
                if((arg0_tag==T_Const) &&  (arg1_tag==T_Var)){
			check_func_id(opexpr->opfuncid, is_support);
                        return ;
                }

                //leaf node: CC
                if((arg0_tag==T_Const) &&  (arg1_tag==T_Const)){
			check_func_id(opexpr->opfuncid, is_support);
                        return ;
                }

		//oo node: OO
		if( (arg0_tag==T_OpExpr) && (arg1_tag==T_OpExpr) ){
                        fpga_ops_check(arg0, is_support);
                        fpga_ops_check(arg1, is_support);
			check_func_id(opexpr->opfuncid, is_support);
                        return ;
		}

		//calc node:CO, VO, OC, OV
		if(arg0_tag == T_Const){
                        fpga_ops_check(arg1, is_support);
			check_func_id(opexpr->opfuncid, is_support);
                }else if(arg0_tag == T_Var){
                        fpga_ops_check(arg1, is_support);
			check_func_id(opexpr->opfuncid, is_support);
                }else if(arg1_tag == T_Const){
                        fpga_ops_check(arg0, is_support);
			check_func_id(opexpr->opfuncid, is_support);
                }else if(arg1_tag == T_Var){
                        fpga_ops_check(arg0, is_support);
			check_func_id(opexpr->opfuncid, is_support);
                }else{
			DBG_INFO("impossible  happen, tag=%d, arg0_tag=%d, arg1_tag=%d\n", nodeTag(expr), arg0_tag, arg1_tag);
			*is_support = 0;
                }

	}else if( T_BoolExpr == nodeTag(expr) ){
                BoolExpr   *boolexpr = (BoolExpr *) expr;
                ListCell*       lc;
                Node*   arg;
                int     type;
                foreach(lc, boolexpr->args){
                        arg = lfirst(lc);
                        type = nodeTag(arg);
                        if(T_OpExpr == type){
				fpga_ops_check(arg, is_support);
                        }else if(T_BoolExpr == type){
				fpga_ops_check(arg, is_support);
                        }else{
                                DBG_INFO("unknow type %d\n", type);
				*is_support = 0;
                                return ;
                        }
                }
	}else{
		DBG_INFO("unknow expr %d\n", nodeTag(expr));
		*is_support = 0;
		return ;
	}
}

int create_qual_for_fpga(qual_t *qual, char *filepath, int file_cnt, int max_attr_index, int op_cnt)
{
	qual->natts = max_attr_index + 1;
	qual->filter_cnt = op_cnt;
	qual->file_cnt = file_cnt;
	strncpy(qual->filepath, filepath, sizeof(qual->filepath));
	return 0;
}

int create_tupdesc_for_fpga(struct tupleDesc *pg_tupdesc, tupdesc_t *fpga_tupdesc)
{
        FormData_pg_attribute *attr;

        short attalign;
        for(int i=0; i<pg_tupdesc->natts; i++){
                attr = TupleDescAttr(pg_tupdesc, i);
                fpga_tupdesc->desc_len_in[i] = attr->attlen;

                switch(attr->attalign){
                        case 'i':
                                attalign = 3;
                                break;
                        case 'c':
                                attalign = 0;
                                break;
                        case 'd':
                                attalign = 7;
                                break;
                        default:
                                attalign = 1;
                                break;
                }
                fpga_tupdesc->desc_align_in[i] = attalign;

        }

        return 0;
}


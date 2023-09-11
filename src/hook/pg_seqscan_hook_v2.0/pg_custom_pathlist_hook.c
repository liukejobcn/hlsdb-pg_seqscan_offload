#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <linux/un.h>

#include "common.h"
#include "pg_custom_pathlist_hook.h"
#include "hlog.h"
#include "pg_filter_common.h"
#include "fpga_qual.h"
#include "query.h"
#include "share_mem.h"
#include "shmem_lib.h"
#include "msg_client.h"


#define FPGA_SERVER_PATH        "/tmp/fpga_seqscan_server"

static void *g_share_mem = NULL;

static Plan*
PlanPGCustomScanPath (PlannerInfo* root,
					RelOptInfo* rel,
					CustomPath* best_path,
					List* tlist,
					List* clauses,
					List* custom_plans);

static Node*
CreatePGCustomScanState (CustomScan* custom_scan);

static void BeginPGCustomScan (CustomScanState* node, EState* estate, int eflags);
static void ReScanPGCustomScan (CustomScanState* node);
static TupleTableSlot* ExecPGCustomScan (CustomScanState* node);
static void EndPGCustomScan (CustomScanState* node);


static CustomPathMethods	PGCustomScan_path_methods = {
	"PGCustomScan",			   /* CustomName */
	PlanPGCustomScanPath,		 /* PlanCustomPath */
#if PG_VERSION_NUM < 90600
	NULL,					   /* TextOutCustomPath */
#endif
};

static CustomScanMethods	PGCustomScan_scan_methods = {
	"PGCustomScan",			   /* CustomName */
	CreatePGCustomScanState,	  /* CreateCustomScanState */
#if PG_VERSION_NUM < 90600
	NULL,					   /* TextOutCustomScan */
#endif
};

static CustomExecMethods	PGCustomScan_exec_methods = {
	"PGCustomScan",			   /* CustomName */
	BeginPGCustomScan,			/* BeginCustomScan */
	ExecPGCustomScan,			 /* ExecCustomScan */
	EndPGCustomScan,			  /* EndCustomScan */
	ReScanPGCustomScan,		   /* ReScanCustomScan */
	NULL,					   /* MarkPosCustomScan */
	NULL,					   /* RestrPosCustomScan */
#if PG_VERSION_NUM >= 90600
	NULL,					   /* EstimateDSMCustomScan */
	NULL,					   /* InitializeDSMCustomScan */
	NULL,					   /* InitializeWorkerCustomScan */
#endif
	NULL,		  		/* ExplainCustomScan */
};


static Plan*
PlanPGCustomScanPath (PlannerInfo* root,
					RelOptInfo* rel,
					CustomPath* best_path,
					List* tlist,
					List* clauses,
					List* custom_plans)
{
	List*		   PGCustom_quals = best_path->custom_private;

	CustomScan*		cscan = (CustomScan *)newNode(sizeof(FpgaCustomScan), T_CustomScan);
        FpgaCustomScan*		fpga_cscan = (FpgaCustomScan*) cscan;


	DBG_INFO("trace\n");
	cscan->flags = best_path->flags;
	cscan->methods = &PGCustomScan_scan_methods;

	cscan->scan.scanrelid = rel->relid;
	cscan->scan.plan.targetlist = tlist;
	//cscan->scan.plan.qual = extract_actual_clauses (clauses, false);
	cscan->scan.plan.qual = NULL;
	cscan->custom_exprs = PGCustom_quals;

	fpga_cscan->rel = rel;

	return &cscan->scan.plan;
}

static Node*
CreatePGCustomScanState (CustomScan* custom_scan)
{
	FpgaCustomScan* 	fpga_cscan = (FpgaCustomScan*) custom_scan;
        PGCustomScanState* 	pg_css = (PGCustomScanState*) palloc0 (sizeof (PGCustomScanState));


	DBG_INFO("trace\n");

	memset(pg_css, 0 , sizeof(PGCustomScanState));
	NodeSetTag (pg_css, T_CustomScanState);
	pg_css->css.flags = custom_scan->flags;
	pg_css->css.methods = &PGCustomScan_exec_methods;
	pg_css->baserel = fpga_cscan->rel;

	create_fpga_filter(pg_css->baserel, &pg_css->filter, &pg_css->max_attr_index, &pg_css->filter_cnt);

	return (Node*)&pg_css->css;
}

static void BeginPGCustomScan (CustomScanState* node, EState* estate, int eflags)
{
        PGCustomScanState  	*pg_css = (PGCustomScanState *) node;
	Relation        	rel = node->ss.ss_currentRelation;
	struct tupleDesc        *tupdesc = RelationGetDescr(rel);
	RelFileNode             *rd_node = &rel->rd_node;

	DBG_INFO("trace pg_css=%p\n", pg_css);

	create_tupdesc_for_fpga(tupdesc, &pg_css->tupdesc);

	pg_css->blk_cnt = RelationGetNumberOfBlocksInFork(rel, MAIN_FORKNUM);

        pg_css->file_cnt = pg_css->blk_cnt / ((BlockNumber) RELSEG_SIZE);

	if( pg_css->blk_cnt % ((BlockNumber) RELSEG_SIZE) ){
		pg_css->file_cnt++;
	}

	snprintf(pg_css->filepath, sizeof(pg_css->filepath), "%s/base/%d/%d", get_current_dir_name(), rd_node->dbNode, rd_node->relNode);

	create_qual_for_fpga(&pg_css->qual, pg_css->filepath, pg_css->file_cnt, pg_css->max_attr_index, pg_css->filter_cnt);

	//to improve performance avoid page fault, so we use global variable here
	pg_css->share_mem = g_share_mem;

	pg_css->respond_cnt = 0;

	pg_css->blk_tuple_cnt = 0;

	pg_css->total_tuple_cnt = 0;

	pg_css->request_cnt = 0;

}

static void ReScanPGCustomScan (CustomScanState* node)
{
	DBG_INFO("trace\n");
}


static TupleTableSlot *
AccessPGCustomScan(CustomScanState *node)
{
        PGCustomScanState  	*pg_css = (PGCustomScanState *) node;
        TupleTableSlot 		*slot = node->ss.ss_ScanTupleSlot;
        HeapTuple               tuple;
	int 			blk_index;
	char			*page;
	int			tup_pos;
	int 			page_id;
	int			hit_tuple_index;
	pd_linp_t		*item;

	//DBG_INFO("trace, pg_css=%p\n", pg_css);

	if(0 == pg_css->request_cnt){
		pg_css->conn = connect_to_server(FPGA_SERVER_PATH);
		pg_css->request_cnt = send_query_requset(pg_css->conn, &pg_css->tupdesc, &pg_css->filter, &pg_css->qual, BLK_CNT); 
		DBG_INFO("filepath=%s, file_cnt=%d, requset_cnt=%d, pg_css=%p\n", pg_css->filepath, pg_css->file_cnt, pg_css->request_cnt, pg_css);
	}

	if( (pg_css->request_cnt == pg_css->respond_cnt) && (0 == pg_css->blk_tuple_cnt) ){
		DBG_INFO("Scan end\n");
		close_connect(pg_css->conn);
		return NULL;
	}

	if( (0 == pg_css->respond_cnt) || (0 == pg_css->blk_tuple_cnt) ){
		blk_index = recv_query_result(pg_css->conn);

		pg_css->blk_index = blk_index;

		pg_css->mem_blk = &pg_css->share_mem->blk[blk_index];

                pthread_mutex_unlock(&pg_css->mem_blk->blk_mutex);

                pg_css->blk_tuple_cnt =  pg_css->mem_blk->tup_cnt;

                pg_css->total_tuple_cnt += pg_css->blk_tuple_cnt;

		pg_css->respond_cnt++;

		pg_css->tup_index = 0;
	}

	tup_pos = pg_css->mem_blk->tup_pos[pg_css->tup_index];

	page_id = (tup_pos>>16) & 0xffff;

	hit_tuple_index = (tup_pos&0xffff);

	page = pg_css->mem_blk->src[page_id].dat;

	item = (pd_linp_t*)(page + 24);

	item += hit_tuple_index;

	pg_css->c_tuple.t_data = (HeapTupleHeader)(page + item->lp_off);

        pg_css->c_tuple.t_len  = item->lp_len;

	tuple =  &pg_css->c_tuple;

        ExecStoreTuple(tuple, slot, InvalidBuffer, false);

	//DBG_INFO("blk_index=%d, blk_tuple_cnt=%d, top_pos=%d\n", pg_css->blk_index, pg_css->blk_tuple_cnt, tup_pos);

	//print_slot(slot);

	pg_css->blk_tuple_cnt--;
	
	pg_css->tup_index++;

        return slot;
}

static bool
RecheckPGCustomScan(CustomScanState *node, TupleTableSlot *slot)
{
        return true;
}

static TupleTableSlot *
ExecPGCustomScan(CustomScanState *node)
{
        return ExecScan(&node->ss,
                                        (ExecScanAccessMtd) AccessPGCustomScan,
                                        (ExecScanRecheckMtd) RecheckPGCustomScan);
}


static void EndPGCustomScan (CustomScanState* node)
{
        PGCustomScanState  *pg_css = (PGCustomScanState *) node;
	DBG_INFO("trace\n");

	//munmap(pg_css->share_mem, sizeof(share_mem_data_t));

	close_connect(pg_css->conn);

	DBG_INFO("trace end\n");
}

void PGSetCustomRelPathlistHook (PlannerInfo* root, RelOptInfo* baserel,
				   Index rtindex, RangeTblEntry* rte)
{

	char            relkind;
	int		baserestrictinfo_cnt;
        ListCell*       lc;
        CustomPath* cpath;
        Relids    required_outer;

        DBG_INFO("set custom hook\n");

        if (rte->rtekind != RTE_RELATION) {
                DBG_INFO("return rte->rtekind != RTE_RELATION, %d, %d\n", rte->rtekind, RTE_RELATION);
                return;
        }

        relkind = get_rel_relkind (rte->relid);

        if (relkind != RELKIND_RELATION &&
                relkind != RELKIND_MATVIEW &&
                relkind != RELKIND_TOASTVALUE) {
                DBG_INFO("return rte->rtekind != RTE_RELATION, %d, %d\n", rte->rtekind, RTE_RELATION);
                return;
        }


	baserestrictinfo_cnt = list_length(baserel->baserestrictinfo);
	if( 0 == baserestrictinfo_cnt){
		DBG_INFO("baserestrictinfo_cnt=0 , query not support\n");
		return ;
	}

	foreach (lc, baserel->baserestrictinfo) {
                RestrictInfo* rinfo = (RestrictInfo*) lfirst (lc);
                int     is_support;

                if (!IsA (rinfo, RestrictInfo)) {
                    continue;
                }
                is_support = 1;
                fpga_ops_check((Node*)rinfo->clause, &is_support);
                if(0 == is_support){
                        DBG_INFO("query not support\n");
                        return ;
                }
        }

	required_outer = baserel->lateral_relids;

	cpath = (CustomPath*) palloc0 (sizeof (CustomPath));
	cpath->path.type = T_CustomPath;
	cpath->path.pathtype = T_CustomScan;
	cpath->path.parent = baserel;
#if PG_VERSION_NUM >= 90600
	cpath->path.pathtarget = baserel->reltarget;
#endif
	cpath->path.param_info = get_baserel_parampathinfo (root, baserel, required_outer);

	cpath->flags = CUSTOMPATH_SUPPORT_BACKWARD_SCAN;
	cpath->custom_private = NULL;
	cpath->methods = &PGCustomScan_path_methods;

	DBG_INFO("add_path for rel, relid=%d, reltablespace=%d\n", baserel->relid, baserel->reltablespace);

	if(!g_share_mem){
		DBG_INFO("open_share_memory\n");
		g_share_mem = open_share_memory(SHARE_MEM_NAME, sizeof(share_mem_data_t));
	}

	add_path (baserel, &cpath->path);
}


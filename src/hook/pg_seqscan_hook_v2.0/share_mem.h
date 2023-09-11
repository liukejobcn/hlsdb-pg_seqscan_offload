#ifndef __SHARE_MEM_H__
#define __SHARE_MEM_H__

#define PG_PAGE_SIZE    8192
#define SYS_PAGE_SIZE   4096
#define KERNEL_CNT      8
#define SPLIT_CNT       8
#define BLK_CNT		(KERNEL_CNT*SPLIT_CNT)
#define CL_SRC_MEM_SIZE 1024*1024*1024
#define PAGE_TUPLE      315

#define CL_RET_MEM_SIZE CL_SRC_MEM_SIZE

#define BLK_SRC_DATA_SIZE       (CL_SRC_MEM_SIZE/(BLK_CNT))
#define BLK_SRC_PAGE_CNT        (BLK_SRC_DATA_SIZE/PG_PAGE_SIZE)
#define BLK_TUPLE_POS_SIZE              (PAGE_TUPLE*BLK_SRC_PAGE_CNT*sizeof(int))
#define BLK_TUPLE_POS_SYS_PAGE_CNT      ((BLK_TUPLE_POS_SIZE+SYS_PAGE_SIZE)/SYS_PAGE_SIZE)
#define SHARE_MEM_NAME	"pg_share_mem"

typedef struct pg_page{
        char dat[PG_PAGE_SIZE];
}pg_page_t;

typedef struct sys_page{
        char dat[SYS_PAGE_SIZE];
}sys_page_t;

typedef struct mem_blk{
        pg_page_t src[BLK_SRC_PAGE_CNT];
        union{
                tupdesc_t tupdesc;
                sys_page_t tupdesc_dat;
        };
        union{
                filter_t filter;
                sys_page_t filter_dat;
        };
        union{
                int     tup_cnt;
                sys_page_t tup_cnt_dat;
        };
        union{
                int     tup_pos[PAGE_TUPLE];
                sys_page_t      tup_pos_dat[BLK_TUPLE_POS_SYS_PAGE_CNT];
        };
	union{
		pthread_mutexattr_t mutex_attr;
		pthread_mutex_t blk_mutex;
                sys_page_t ctl_dat;
	};
}mem_blk_t;

typedef struct share_mem_data{
        mem_blk_t blk[BLK_CNT];
}share_mem_data_t;

#endif

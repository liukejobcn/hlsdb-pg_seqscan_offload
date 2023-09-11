#ifndef __FPGA_SEQSCAN_WORKER_H__
#define __FPGA_SEQSCAN_WORKER_H__
#include "fpga_seqscan.h"
class pg_file
{
public:
        int fd;
        int filesize;
        unsigned char *data;
        char name[128];
};

class req_iov
{
public:
        int index;
        int fd;
        int off;
        unsigned char *dat;
        int len;
	int len_padding;
};

class fpga_seqscan_worker : public fpga_seqscan
{
public:
	fpga_seqscan_worker();	
	virtual ~fpga_seqscan_worker();	
	//thread
	int start_worker(int worker_cnt);
	int wait_worker();
	static void worker_func(fpga_seqscan_worker *data);
	static void read_event_callback(cl_event event1, cl_int cmd_status, void *data);

	//query
	int recv_query_requset(tupdesc_t *tupdesc, filter_t *filter, qual_t *qual);
	int fill_query_info(tupdesc_t *tupdesc, filter_t *filter, int natts, int filter_cnt);
	int get_file_request_cnt(char *filepath, int buf_cnt);
	int get_query_request_cnt(qual_t *qual, int buf_cnt);
	int iter_files_to_requset(qual_t *qual);

	//request
	int split_file_to_request(char *filepath);
        bool is_empty();
        int fetch_req_iov(req_iov &iov);
        void notify_req_done();
        void wait_req_done();
        void wait_sub_req_done();
	virtual int notify_hook_process(sub_buf_t *sub_buf);
public:
	//thread
	bool running;
	int worker_cnt;
	thread **threads;

	//query
	tupdesc_t tupdesc;
	filter_t filter;
	qual_t qual;

	//request
	std::deque<req_iov> req_queue;
        std::deque<pg_file> fd_queue;
        std::mutex queue_mutex;
        std::mutex counter_mutex;
        int pg_page_size;
        sem_t req_sem;
        sem_t req_done_sem;
        sem_t sub_req_done_sem;
	int	cu_cnt;
        int     total_req_cnt;
        int     total_respond_cnt;
	int	sub_req_cnt;
	int	sub_respond_cnt;

//legacy only for debug
public:
	int show_sub_buf_tup_cnt(void);

};

#endif



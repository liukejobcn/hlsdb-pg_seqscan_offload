#ifndef __FPGA_SEQSCAN__H__
#define __FPGA_SEQSCAN__H__
#include "fpga_card.h"

typedef struct sub_buf{
	void *hook_data;
	void *ptr_sub_buf;
	mem_blk_t	*ptr_mem_blk;
	pthread_mutex_t *blk_mutex;
	int sub_buf_size;
	int fd;
	int buf_index;
	int kernel_index;
	int page_cnt;
	int natts;
	int filter_cnt;
	int is_active;
	cl::size_type src_offset;
	cl::size_type src_len;
	cl::size_type tupdesc_offset;
	cl::size_type tupdesc_len;
	cl::size_type filter_offset;
	cl::size_type filter_len;
	cl::size_type tup_cnt_offset;
	cl::size_type tup_cnt_len;
	cl::size_type tup_pos_offset;
	cl::size_type tup_pos_len;
	pg_page_t *ptr_src;
	tupdesc_t *ptr_tupdesc;
	filter_t *ptr_filter;
	int *ptr_tup_cnt;
	int *ptr_tup_pos;
	cl::Buffer *src;
	cl::Buffer *tupdesc;
	cl::Buffer *filter;
	cl::Buffer *tup_cnt;
	cl::Buffer *tup_pos;
	std::mutex buf_mutex;
	cl::Event       write_event;
        cl::Event       kernel_event;
        cl::Event       read_event;
        std::vector<cl::Event> kernel_waitList;
        std::vector<cl::Event> read_waitList;
}sub_buf_t;

class fpga_seqscan : public fpga_card
{
public:
	fpga_seqscan();
	~fpga_seqscan();
	int init(const char *xclbin, const char *kernel_name, void *data);
	int sub_buf_reset();
public:
	int kernel_cnt;
	int split_cnt;

	char share_mem_filename[128];
	int  share_mem_fd;
	unsigned long  share_mem_size;
	share_mem_data_t *ptr_share_mem;

	int sub_buf_cnt;
	int sub_buf_size;
	int sub_buf_src_size;
	int sub_buf_ret_size;

	int tupdesc_len;
	int filter_len;
	int tup_cnt_len;
	int tup_pos_len;

	sub_buf_t *sub_buf_array;

};


#endif

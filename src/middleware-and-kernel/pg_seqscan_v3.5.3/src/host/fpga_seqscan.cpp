#include "fpga_seqscan.h"

fpga_seqscan::fpga_seqscan()
{
	sub_buf_cnt = BLK_CNT; 
	kernel_cnt = KERNEL_CNT;
	split_cnt = SPLIT_CNT;

	sub_buf_size = sizeof(mem_blk_t);
	sub_buf_src_size = BLK_SRC_DATA_SIZE;
	sub_buf_ret_size = sub_buf_size - sub_buf_src_size;
	share_mem_size = sizeof(share_mem_data_t);

	tupdesc_len = 4096;
	filter_len = 4096;
	tup_cnt_len = 4096;
}

fpga_seqscan::~fpga_seqscan()
{
	for(int i=0; i<sub_buf_cnt; i++){
		sub_buf_t *sub_buf = &sub_buf_array[i];
		delete sub_buf->src;
		delete sub_buf->tupdesc;
		delete sub_buf->filter;
		delete sub_buf->tup_cnt;
		delete sub_buf->tup_pos;
	}
	delete []sub_buf_array;

	munmap(ptr_share_mem, share_mem_size);
	close(share_mem_fd);
	shm_unlink(SHARE_MEM_NAME);
}

int fpga_seqscan::sub_buf_reset()
{
	for(int i=0; i<sub_buf_cnt; i++){
		sub_buf_array[i].is_active = 0;
	}
	return 0;
}

int fpga_seqscan::init(const char *xclbin, const char *kernel_name, void *data)
{
	int ret;
	cl_int err;


	ret = fpga_card::load_xclbin(xclbin, kernel_name, kernel_cnt);
	if(ret){
		return ret;
	}

	//init the sub_buf pool
	share_mem_fd =shm_open(SHARE_MEM_NAME, O_CREAT|O_TRUNC|O_RDWR, 0777);
	ret = ftruncate(share_mem_fd, share_mem_size);
	if(ret < 0){
		DBG_ERR("ftruncate failed, ret=%d, errno=%d(%s\n)", ret, errno, strerror(errno));
		return ret;
	}
	ptr_share_mem = (share_mem_data_t *)mmap(NULL, share_mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, share_mem_fd, 0);	
	memset(ptr_share_mem, 0, share_mem_size);

	sub_buf_array = new sub_buf_t[sub_buf_cnt];

	//Page(8192),Item(4),Tupleheader(22), 8192/(22+4)=315
	tup_pos_len = 315*(sub_buf_src_size/8192)*sizeof(int);

	for(int i=0; i<sub_buf_cnt; i++){
		mem_blk_t *ptr_mem_blk = &ptr_share_mem->blk[i]; 
		sub_buf_array[i].buf_index = i;
		sub_buf_array[i].kernel_index = i%kernel_cnt;
		sub_buf_array[i].ptr_mem_blk = ptr_mem_blk;
		sub_buf_array[i].sub_buf_size = sub_buf_size;

		sub_buf_array[i].src_len = sub_buf_src_size;
		sub_buf_array[i].ptr_src = &ptr_mem_blk->src[0];

		sub_buf_array[i].tupdesc_len = tupdesc_len;
		sub_buf_array[i].ptr_tupdesc = &ptr_mem_blk->tupdesc;

		sub_buf_array[i].filter_len = filter_len;
		sub_buf_array[i].ptr_filter = &ptr_mem_blk->filter;

		sub_buf_array[i].tup_cnt_len = tup_cnt_len;
		sub_buf_array[i].ptr_tup_cnt = &ptr_mem_blk->tup_cnt; 

		sub_buf_array[i].tup_pos_len = tup_pos_len;
		sub_buf_array[i].ptr_tup_pos = &ptr_mem_blk->tup_pos[0]; 

		pthread_mutexattr_init(&ptr_mem_blk->mutex_attr);
		pthread_mutexattr_setpshared(&ptr_mem_blk->mutex_attr,PTHREAD_PROCESS_SHARED);
		pthread_mutex_init(&ptr_mem_blk->blk_mutex, &ptr_mem_blk->mutex_attr);
		sub_buf_array[i].blk_mutex= &ptr_mem_blk->blk_mutex;

		sub_buf_array[i].src = new cl::Buffer(*context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, sub_buf_array[i].src_len, sub_buf_array[i].ptr_src, &err);
		sub_buf_array[i].tupdesc = new cl::Buffer(*context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, sub_buf_array[i].tupdesc_len,sub_buf_array[i].ptr_tupdesc, &err);
		sub_buf_array[i].filter = new cl::Buffer(*context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, sub_buf_array[i].filter_len, sub_buf_array[i].ptr_filter, &err);

		sub_buf_array[i].tup_cnt = new cl::Buffer(*context, CL_MEM_WRITE_ONLY|CL_MEM_USE_HOST_PTR, sub_buf_array[i].tup_cnt_len, sub_buf_array[i].ptr_tup_cnt, &err);
		sub_buf_array[i].tup_pos = new cl::Buffer(*context, CL_MEM_WRITE_ONLY|CL_MEM_USE_HOST_PTR, sub_buf_array[i].tup_pos_len, sub_buf_array[i].ptr_tup_pos, &err);
		
		sub_buf_array[i].hook_data = data;

		sub_buf_array[i].is_active = 0;
	}


	return 0;
}

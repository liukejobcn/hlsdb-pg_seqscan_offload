#include "fpga_seqscan_worker.h"

fpga_seqscan_worker::fpga_seqscan_worker()
{
	running = false;

	total_req_cnt = 0;
        pg_page_size = 8192;
	cu_cnt = 8;
        sem_init(&req_sem, 0, 0);
        sem_init(&req_done_sem, 0, 0);
        sem_init(&sub_req_done_sem, 0, 0);
}

fpga_seqscan_worker::~fpga_seqscan_worker()
{
	for(int i=0; i<worker_cnt; i++){
		delete threads[i];
	}
	delete threads;

	for(unsigned int i=0; i<fd_queue.size(); i++){
		pg_file &pg_file = fd_queue.at(i);
                munmap(pg_file.data, pg_file.filesize);
                close(pg_file.fd);
        }
        fd_queue.clear();
}

int fpga_seqscan_worker::get_file_request_cnt(char *filepath, int buf_cnt)
{
	int req_cnt;
        int filesize;
        int file_blk_cnt;
        struct stat statbuf;
        int blk_group_cnt;
        int blk_group_mod;

        stat(filepath, &statbuf);
        filesize = statbuf.st_size;
        file_blk_cnt = filesize/8192;

        blk_group_cnt = file_blk_cnt/cu_cnt;
        blk_group_mod = file_blk_cnt%cu_cnt;
        if(blk_group_mod){
                blk_group_cnt++;
        }

        req_cnt = (blk_group_cnt < buf_cnt) ? blk_group_cnt : buf_cnt;

        return req_cnt;
}

int fpga_seqscan_worker::get_query_request_cnt(qual_t *qual, int buf_cnt)
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

int fpga_seqscan_worker::fill_query_info(tupdesc_t *tupdesc, filter_t *filter, int natts, int filter_cnt)
{
	for(int i=0; i<sub_buf_cnt; i++){
		sub_buf_t *sub_buf = &sub_buf_array[i];
		memcpy(sub_buf->ptr_tupdesc, tupdesc, sizeof(tupdesc_t));
		memcpy(sub_buf->ptr_filter, filter, sizeof(filter_t));
		sub_buf->natts = natts;
		sub_buf->filter_cnt = filter_cnt;
	}

	return 0;
}

int fpga_seqscan_worker::iter_files_to_requset(qual_t *qual)
{
	char filepath[512];
	snprintf(filepath, sizeof(filepath), "%s", qual->filepath);
	split_file_to_request(filepath);

	for(int i=1; i<qual->file_cnt; i++){
		snprintf(filepath, sizeof(filepath), "%s.%d", qual->filepath, i);
		split_file_to_request(filepath);
	}

	return 0;
}

int fpga_seqscan_worker::recv_query_requset(tupdesc_t *tupdesc, filter_t *filter, qual_t *qual)
{
	sub_buf_reset();

	memcpy(&this->tupdesc, tupdesc, sizeof(tupdesc_t));
	memcpy(&this->filter, filter, sizeof(filter_t));
	memcpy(&this->qual, qual, sizeof(qual_t));

	fill_query_info(tupdesc, filter, qual->natts, qual->filter_cnt);

	total_req_cnt = get_query_request_cnt(qual, sub_buf_cnt);
	total_respond_cnt = 0;

	iter_files_to_requset(qual);

	return 0;
}

void fpga_seqscan_worker::read_event_callback(cl_event event1, cl_int cmd_status, void *data)
{
	sub_buf_t *sub_buf = (sub_buf_t *)data;
	fpga_seqscan_worker *worker = (fpga_seqscan_worker *)sub_buf->hook_data;

	worker->notify_hook_process(sub_buf);

	worker->notify_req_done();

	//this will be done in pg hook
	//pthread_mutex_unlock(sub_buf->blk_mutex);
	
	return ;
}

int fpga_seqscan_worker::notify_hook_process(sub_buf_t *sub_buf)
{
	return 0;
}

void fpga_seqscan_worker::worker_func(fpga_seqscan_worker *worker)
{
	DBG_INFO("start run \n");
	while(worker->running){
		sem_wait(&worker->req_sem);
		if(worker->is_empty()){
			continue;
		}
		req_iov iov;
		worker->fetch_req_iov(iov);
		//DBG_INFO("index=%d, off=%d, len=%d, dat=%p\n", iov.index, iov.off, iov.len, iov.dat);

		sub_buf_t *sub_buf = &worker->sub_buf_array[iov.index];

		pthread_mutex_lock(sub_buf->blk_mutex);

		if(!sub_buf->is_active){
			sub_buf->is_active = 1;
		}else{
			sub_buf->read_event.wait();
		}

		memcpy(sub_buf->ptr_src, iov.dat, iov.len);
		if(iov.len != iov.len_padding){
			memset((char *)sub_buf->ptr_src + iov.len, 0, iov.len_padding-iov.len); 
		}

		sub_buf->page_cnt = iov.len_padding/8192;

		worker->kernel_mutex.lock();
		cl::Kernel **kernels = worker->kernels;
		int k = sub_buf->kernel_index;
		int narg = 0;
		kernels[k]->setArg(narg++, *sub_buf->src);
                kernels[k]->setArg(narg++, 0);
                kernels[k]->setArg(narg++, sub_buf->page_cnt);
                kernels[k]->setArg(narg++, sub_buf->natts);
                kernels[k]->setArg(narg++, sub_buf->filter_cnt);
                kernels[k]->setArg(narg++, *sub_buf->tupdesc);
                kernels[k]->setArg(narg++, *sub_buf->filter);
                kernels[k]->setArg(narg++, *sub_buf->tup_cnt);
                kernels[k]->setArg(narg++, *sub_buf->tup_pos);

                worker->q->enqueueMigrateMemObjects({*sub_buf->src, *sub_buf->tupdesc, *sub_buf->filter}, 0, NULL, &sub_buf->write_event);

		sub_buf->kernel_waitList.clear();
                sub_buf->kernel_waitList.push_back(sub_buf->write_event);
                worker->q->enqueueTask(*kernels[k], &sub_buf->kernel_waitList, &sub_buf->kernel_event);

		sub_buf->read_waitList.clear();
                sub_buf->read_waitList.push_back(sub_buf->kernel_event);
                worker->q->enqueueMigrateMemObjects({*sub_buf->tup_cnt, *sub_buf->tup_pos}, CL_MIGRATE_MEM_OBJECT_HOST, &sub_buf->read_waitList, &sub_buf->read_event);

		sub_buf->read_event.setCallback(CL_COMPLETE, read_event_callback, (void *)sub_buf);

		worker->kernel_mutex.unlock();

	}
	DBG_INFO("Thread over\n");
}


int fpga_seqscan_worker::start_worker(int worker_cnt)
{
	running = true;

	this->worker_cnt = worker_cnt;

	threads = new thread *[worker_cnt];

	for(int i=0; i<worker_cnt; i++){
		threads[i] = new thread(fpga_seqscan_worker::worker_func, this);
	}

	return 0;
}

int fpga_seqscan_worker::wait_worker()
{
	running = false;
	for(int i=0; i<worker_cnt; i++){
		sem_post(&req_sem);
	}
	for(int i=0; i<worker_cnt; i++){
		threads[i]->join();
	}
	return 0;
}

int fpga_seqscan_worker::split_file_to_request(char *filepath)
{
	int fd;
	struct stat statbuf;
	int filesize;
	int file_blk_cnt;
	int blk_group_cnt;
	int blk_group_mod;

	int sub_blk_cnt;
	int sub_blk_mod;
	int sub_blk_size;

	fd = open(filepath, O_RDWR, 0666);
	if(fd < 0){
		DBG_ERR("Failed to open file %s, fd=%d, error=%d(%s\n)", filepath, fd, errno, strerror(errno));
		return -1;
	}
	fstat(fd, &statbuf);
	filesize =  statbuf.st_size;

	pg_file pg_file;
	pg_file.fd = fd;
	pg_file.filesize = filesize;
	pg_file.data = (unsigned char *)mmap(NULL, filesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	
	strncpy(pg_file.name, filepath, sizeof(pg_file.name));

	fd_queue.push_back(pg_file);

	file_blk_cnt = filesize/pg_page_size;	

	blk_group_cnt = file_blk_cnt/cu_cnt;
	blk_group_mod = file_blk_cnt%cu_cnt;
	if(blk_group_mod){
		blk_group_cnt++;
	}

	sub_respond_cnt = 0;
	sub_req_cnt = get_file_request_cnt(filepath, sub_buf_cnt);

	if(blk_group_cnt >= sub_buf_cnt){
		sub_blk_cnt = blk_group_cnt/sub_buf_cnt;
		sub_blk_mod = blk_group_cnt%sub_buf_cnt;

		sub_blk_size = sub_blk_cnt*pg_page_size*cu_cnt;
		for(int i=0; i<(sub_buf_cnt-1); i++){
			req_iov iov;	
			iov.index = i;
			iov.fd = fd;
			iov.off = i*sub_blk_size; 
			iov.dat = pg_file.data + i*sub_blk_size; 
			iov.len_padding = sub_blk_size;
			iov.len = sub_blk_size;

			req_queue.push_back(iov);
			sem_post(&req_sem);
		}

		int offset = (sub_buf_cnt-1)*sub_blk_size;
		sub_blk_size = (sub_blk_cnt+sub_blk_mod)*pg_page_size*cu_cnt;
		req_iov iov;	
		iov.index = sub_buf_cnt-1;
		iov.fd = fd;
		iov.off = offset ;
		iov.dat = pg_file.data + offset; 
		iov.len_padding = sub_blk_size;

		if(blk_group_mod){
			iov.len = sub_blk_size - pg_page_size*(cu_cnt - blk_group_mod);
		}else{
			iov.len = sub_blk_size;
		}

		req_queue.push_back(iov);
		sem_post(&req_sem);
	}else{
		sub_blk_size = pg_page_size*cu_cnt;
		for(int i=0; i<(blk_group_cnt-1); i++){
			req_iov iov;	
			iov.index = i;
			iov.fd = fd;
			iov.off = i*sub_blk_size; 
			iov.dat = pg_file.data + i*sub_blk_size; 
			iov.len = sub_blk_size;
			iov.len_padding = sub_blk_size;

			req_queue.push_back(iov);
			sem_post(&req_sem);
		}

		int offset = (blk_group_cnt-1)*sub_blk_size;
		req_iov iov;	
		iov.index = blk_group_cnt-1;
		iov.fd = fd;
		iov.off = offset ;
		iov.dat = pg_file.data + offset; 
		iov.len_padding = sub_blk_size;

		if(blk_group_mod){
			iov.len = blk_group_mod*pg_page_size;
		}else{
			iov.len = sub_blk_size;
		}

		req_queue.push_back(iov);
		sem_post(&req_sem);
	}

	wait_sub_req_done();

	return 0;
}


void fpga_seqscan_worker::notify_req_done()
{
	counter_mutex.lock();

	sub_respond_cnt++;
	if(sub_req_cnt == sub_respond_cnt){
		sem_post(&sub_req_done_sem);
	}

	total_respond_cnt++;
	if(total_req_cnt == total_respond_cnt){
		sem_post(&req_done_sem);
	}

	counter_mutex.unlock();
}

void fpga_seqscan_worker::wait_sub_req_done()
{
	sem_wait(&sub_req_done_sem);
}

void fpga_seqscan_worker::wait_req_done()
{
	sem_wait(&req_done_sem);
}

int fpga_seqscan_worker::fetch_req_iov(req_iov &iov)
{
	queue_mutex.lock();
	iov = req_queue.at(0);
	req_queue.pop_front();
	queue_mutex.unlock();

	return 0;
}

bool fpga_seqscan_worker::is_empty()
{
	bool empty;
	queue_mutex.lock();
	if(0 == req_queue.size()){
		empty = true;
	}else{
		empty = false;
	}
	queue_mutex.unlock();

	return empty;
}

int fpga_seqscan_worker::show_sub_buf_tup_cnt(void)
{
	int total_cnt = 0;
	int tup_cnt;
	for(int i=0; i<sub_buf_cnt; i++){
		sub_buf_t *sub_buf = &sub_buf_array[i];
		tup_cnt = *(int *)sub_buf->ptr_tup_cnt;
		total_cnt += tup_cnt;
	}

	DBG_INFO("total_cnt=%d\n", total_cnt);
	return 0;
}

#ifndef __FPGA_SERVER_H__
#define __FPGA_SERVER_H__
#include "fpga_seqscan_worker.h"

class fpga_server : public fpga_seqscan_worker
{
public:
	fpga_server();
	~fpga_server();
	int open_server(const char *name, void *data=NULL);
	int start_server(char *xclbin, const char *kernel="pg_filter", int worker_cnt=4);
	int close_server();
	int msg_func(int conn);
	int notify_hook_process(sub_buf_t *sub_buf);
	int send_respond_msg(int conn, sub_buf_t *sub_buf);
public:
	int sock;
	mutex socket_mutex;
        struct sockaddr_un address;
        char sock_name[256];
	bool running;
	void *data;
	int conn;

public:

};


#endif

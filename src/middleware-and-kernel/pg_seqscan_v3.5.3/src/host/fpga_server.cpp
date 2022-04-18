#include "fpga_server.h"
#include "debug.h"
#include "fpga_common.h"

fpga_server::fpga_server()
{
	return ;
}

fpga_server::~fpga_server()
{
	return ;
}

int fpga_server::open_server(const char *name, void *data)
{
	int ret;
	sock = socket(PF_UNIX, SOCK_STREAM, 0);
        if(-1 == sock){
		return -1;
        }

        address.sun_family = AF_UNIX;
        snprintf(address.sun_path, UNIX_PATH_MAX, "%s", name);
        ret = unlink(name);
        if (ret != 0 && ret != -ENOENT && ret != -EPERM){
		return -2;
        }

        ret = bind(sock, (struct sockaddr *) &address, sizeof(address));
        if (ret != 0){
		return -3;
        }

        strncpy(sock_name, name, strlen(name)+1);

	this->data = data;

	return 0;
}

int fpga_server::start_server(char *xclbin, const char *kernel, int worker_cnt)
{
	int ret;
	struct sockaddr_un address;
        socklen_t addrlen;

	init(xclbin, kernel, this);

	start_worker(worker_cnt);

        ret = listen(sock, 128);
        if (ret != 0){
                return -1;
        }

	INFO("server start running\n");
	running = true;
        while(running) {
                conn = accept(sock, (struct sockaddr *) &address, &addrlen);
                if (conn == -1){
                        DBG_ERR("accept failed errno=%d(%s)\n", errno, strerror(errno));
                        return -2;
                }
                while(1){
			ret = msg_func(conn);
                        if(ret){
                                break;
                        }
                }
        }

	return 0;
}


int fpga_server::close_server()
{
	return 0;
}

int fpga_server::msg_func(int conn)
{
	int ret;
        struct msghdr msgh;
        struct iovec iov[3];
	tupdesc_t tupdesc;
        filter_t filter;
        qual_t qual;

        iov[0].iov_base = &tupdesc;
        iov[0].iov_len = sizeof(tupdesc_t);

        iov[1].iov_base = &filter;
        iov[1].iov_len = sizeof(filter_t);

        iov[2].iov_base = &qual;
        iov[2].iov_len = sizeof(qual_t);

	memset(&msgh, 0, sizeof(msgh));
	msgh.msg_name = NULL;
	msgh.msg_namelen = 0;
	msgh.msg_iov = iov;
	msgh.msg_iovlen= 3;
	msgh.msg_control= NULL;
	msgh.msg_controllen= 0;

	ret = recvmsg(conn, &msgh, 0);
	if(ret < 0){
		DBG_ERR("recvmsg failed errno=%d(%s)", errno, strerror(errno));
		return ret;
	}

	recv_query_requset(&tupdesc, &filter, &qual);

	wait_req_done();

	return 1;
}

int fpga_server::notify_hook_process(sub_buf_t *sub_buf)
{
	send_respond_msg(conn, sub_buf);
	return 0;
}

int fpga_server::send_respond_msg(int conn, sub_buf_t *sub_buf)
{
	int ret;
        struct msghdr msgh;
        struct iovec iov[1];
	int buf_index = sub_buf->buf_index;
	DBG_INFO("sub_buf[%d] is done, tup_cnt=%d\n", sub_buf->buf_index, sub_buf->ptr_mem_blk->tup_cnt);

        iov[0].iov_base = &buf_index;
        iov[0].iov_len = sizeof(int);

	msgh.msg_name = NULL;
	msgh.msg_namelen = 0;
	msgh.msg_iov = iov;
	msgh.msg_iovlen= 1;
	msgh.msg_control= NULL;
	msgh.msg_controllen= 0;

	socket_mutex.lock();
	ret = sendmsg(conn, &msgh, 0);
	socket_mutex.unlock();
	if(ret < 0){
		DBG_ERR("sendmsg failed errno=%d(%s)", errno, strerror(errno));
		return ret;
	}

	return 0;
}

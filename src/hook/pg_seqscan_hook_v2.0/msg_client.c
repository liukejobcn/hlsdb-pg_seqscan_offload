#include "msg_client.h"
#include "query.h"

int connect_to_server(char *name) 
{
        int conn, ret;
        struct sockaddr_un address;

        conn = socket(PF_UNIX, SOCK_STREAM, 0);
        if (conn == -1){
                DBG_INFO("socket failed\n");
                return -1;
        }
        memset(&address, 0, sizeof(address));
        address.sun_family = AF_UNIX;
        snprintf(address.sun_path, UNIX_PATH_MAX, "%s", name);
        ret = connect(conn, (struct sockaddr *)&address, sizeof(struct sockaddr_un));
        if (ret != 0){
                DBG_INFO("connect failed\n");
        }
        return conn;
}

int close_connect(int conn)
{
	return close(conn);
}

int send_query_requset(int conn, tupdesc_t *tupdesc, filter_t *filter, qual_t *qual, int buf_cnt)
{
	struct iovec iov[3];
        struct msghdr msgh;
        int ret;
	int request_cnt;

	request_cnt = get_query_request_cnt(qual, buf_cnt);

        iov[0].iov_base = tupdesc;
        iov[0].iov_len = sizeof(tupdesc_t);

        iov[1].iov_base = filter;
        iov[1].iov_len = sizeof(filter_t);

        iov[2].iov_base = qual;
        iov[2].iov_len = sizeof(qual_t);

	memset(&msgh, 0, sizeof(msgh));
	msgh.msg_name = NULL;
        msgh.msg_namelen = 0;
        msgh.msg_iov = iov;
        msgh.msg_iovlen = 3;
	msgh.msg_control = NULL;
	msgh.msg_controllen = 0;

        ret = sendmsg(conn, &msgh, 0);
        if (ret < 0){
		DBG_INFO("sendmsg failed errno=%d(%s)", errno, strerror(errno));
		return -1;
	}

	return request_cnt;
}

int recv_query_result(int conn)
{
        int ret;
        struct msghdr msgh;
        struct iovec iov[1];
        int buf_index;

        iov[0].iov_base = &buf_index;
        iov[0].iov_len = sizeof(int);

        msgh.msg_name = NULL;
        msgh.msg_namelen = 0;
        msgh.msg_iov = iov;
        msgh.msg_iovlen= 1;
        msgh.msg_control= NULL;
        msgh.msg_controllen= 0;

        ret = recvmsg(conn, &msgh, 0);
        if (ret < 0){
		DBG_INFO("recvmmsg failed errno=%d(%s)", errno, strerror(errno));
                return ret;
        }


	return buf_index;
}


int send_q6_query_requset(int conn, char *filepath, int file_cnt, int buf_cnt)
{
	struct iovec iov[3];
        struct msghdr msgh;
        int ret;
	int request_cnt;

	tupdesc_t tupdesc;
	filter_t filter;
	qual_t qual;

	fill_tupdesc(&tupdesc);	
	fill_filter_q6(&filter);
	fill_qual(&qual, filepath, file_cnt);

	request_cnt = get_query_request_cnt(&qual, buf_cnt);

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
        msgh.msg_iovlen = 3;
	msgh.msg_control = NULL;
	msgh.msg_controllen = 0;

        ret = sendmsg(conn, &msgh, 0);
        if (ret < 0){
		DBG_INFO("sendmsg failed errno=%d(%s)", errno, strerror(errno));
		return -1;
	}

	return request_cnt;
}

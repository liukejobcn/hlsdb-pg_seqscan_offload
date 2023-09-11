#ifndef __MSG_CLIENT_H__
#define __MSG_CLIENT_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <semaphore.h>
#include <signal.h>

#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <pthread.h>

#include "pg_filter_common.h"
#include "share_mem.h"
#include "query.h"
#include "hlog.h"

int connect_to_server(char *name);
int close_connect(int conn);
int send_query_requset(int conn, tupdesc_t *tupdesc, filter_t *filter, qual_t *qual, int buf_cnt);
int recv_query_result(int conn);

__debug_use int send_q6_query_requset(int conn, char *filepath, int file_cnt, int cu_cnt);

#endif


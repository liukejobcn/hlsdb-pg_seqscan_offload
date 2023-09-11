#ifndef __FPGA_COMMMON_H___
#define __FPGA_COMMMON_H__

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#include <CL/cl2.hpp>
#include <stdlib.h>
#include <fstream>
#include <queue>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <iostream>
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
#include <thread>
#include "pg_filter_common.h"
#include "share_mem.h"
#include "debug.h"

using namespace std;


#endif

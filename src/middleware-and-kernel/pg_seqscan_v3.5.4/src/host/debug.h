#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define FPGA_DEBUG

//#define DBG_INFO(fmt, args...) printf("[%s:%s:%d]: " fmt " \n", __FILE__, __func__, __LINE__, ##args)
#ifdef FPGA_DEBUG
#define DBG_INFO(fmt, args...) do { \
                struct timeval tv;\
                gettimeofday(&tv, NULL);\
                printf("{%d-%ld}(%ld.%ld)[%s:%s:%d]: " fmt " \n", getpid(), syscall(SYS_gettid), tv.tv_sec, tv.tv_usec,  __FILE__, __func__, __LINE__, ##args);\
        }while(0);
#else
#define DBG_INFO(fmt, args...)
#endif

#define INFO(fmt, args...) do { \
                struct timeval tv;\
                gettimeofday(&tv, NULL);\
                printf("{%d-%ld}(%ld.%ld): " fmt " \n", getpid(), syscall(SYS_gettid), tv.tv_sec, tv.tv_usec, ##args);\
        }while(0);

#define DBG_ERR(fmt, args...) do { \
                struct timeval tv;\
                gettimeofday(&tv, NULL);\
                printf("{%d-%ld}(%ld.%ld)[%s:%s:%d]: " fmt " \n", getpid(), syscall(SYS_gettid), tv.tv_sec, tv.tv_usec,  __FILE__, __func__, __LINE__, ##args);\
        }while(0);

void __inline__ show_time_diff(const char *str, struct timeval &start, struct timeval &end)
{
#ifdef FPGA_DEBUG
        long long elapsed;
        long long elapsed_s, elapsed_ms, elapsed_us;
        elapsed = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
        elapsed_s = elapsed/1000000;
        elapsed_ms = (elapsed%1000000)/1000;
        elapsed_us = (elapsed%1000);
        DBG_INFO("%s: Time elapsed:%lld ns. (%lld.%lld.%lld) sec\n", str, elapsed, elapsed_s, elapsed_ms, elapsed_us);
#endif
}

#endif

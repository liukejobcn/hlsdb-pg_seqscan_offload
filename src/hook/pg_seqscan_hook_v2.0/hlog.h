#ifndef __LOG_H__
#define __LOG_H__

#define PG_HOOK_DEBUG
#ifdef PG_HOOK_DEBUG
#define DBG_INFO(fmt, args...) hlog("[%s:%d]: "fmt"", __func__, __LINE__, ##args);
#else
#define DBG_INFO(fmt, args...)
#endif

#define __debug_use __attribute__((unused))

void hlog (const char* fmt, ...) __attribute__ ((format (gnu_printf, 1, 2)));
void hlog_init(void);
void dump_hex(void * data, int len, char type);
int dump_bin_to_file(char *file_name, char *pdata, int len);
#endif

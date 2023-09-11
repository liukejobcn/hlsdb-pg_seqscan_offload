#include "hlog.h"
#include <stdio.h>
#include <stdarg.h>

static const char *hook_debug_log = "/tmp/hlog.txt";
static FILE *hlogFile = NULL;

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;

int dump_bin_to_file(char *file_name, char *pdata, int len)
{
	FILE *fp;
	int remain = len;
	int offset = 0;
	int ret;
	fp = fopen(file_name,"w+");
	while(remain){
		ret = fwrite(pdata + offset, 1, remain, fp);
		if(ret <=0){
			return -1;
		}
		remain -= ret;
		offset += ret;
	}
	fclose(fp);
	return 0;

}

void dump_hex(void * pdata, int len, char type)
{
	int i; 
	char *data = pdata;
	if('c' == type){
		for(i=0; i<len; i++){
			hlog("0x%02x, ", *(u8*)(data+i));
			if( ((i+1)%16) == 0){
				hlog("\n");
			}
		}
	}

	if('s' == type){
		for(i=0; i<len; i+=2){
			hlog("0x%04x, ", *(u16*)(data+i));
			if( ((i+2)%16) == 0){
				hlog("\n");
			}
		}
	}

	if('i' == type){
		for(i=0; i<len; i+=4){
			hlog("0x%08x, ", *(u32*)(data+i));
			if(0 == i){
				continue;
			}

			if( ((i+4)%16) == 0){
				hlog("\n");
			}
		}
	}


}

void hlog_init(void)
{
        if (!hlogFile) {
                hlogFile = fopen (hook_debug_log, "w");
        }
}

void hlog (const char* fmt, ...)
{
        if (hlogFile) {
                va_list args;
                va_start (args, fmt);
                vfprintf (hlogFile, fmt, args);
                va_end (args);
                fflush (hlogFile);
        }
}


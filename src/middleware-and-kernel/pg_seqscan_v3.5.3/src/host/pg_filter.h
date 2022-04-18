#ifndef __PG_FILTER_H__
#define __PG_FILTER_H__

#include <ap_int.h>
#include <hls_stream.h>
#include <string.h>
#include "pg_filter_common.h"

#define FLOAT8_CMP(cmp)		bool_stack[bool_stack_top] = (arg0 cmp arg1) ? 1 : 0; bool_stack_top++;
#define FLOAT8_MATH(math)	float8_stack[float8_stack_top] = (arg0 math arg1) ; float8_stack_top++;

#define INT_CMP(cmp)		bool_stack[bool_stack_top] = (arg0 cmp arg1) ? 1 : 0; bool_stack_top++;


#endif

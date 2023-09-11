#ifndef __FUNC_ID_H__
#define __FUNC_ID_H__
#define T_Var 104
#define T_Const 105
#define T_OpExpr	113
#define T_BoolExpr	117
#define OP_AND	0
#define OP_OR	1

#define C_INT 		0
#define C_FLOAT8	1

/*float func
{ 293, "float8eq", 2, true, false, float8eq },
{ 294, "float8ne", 2, true, false, float8ne },
{ 295, "float8lt", 2, true, false, float8lt },
{ 296, "float8le", 2, true, false, float8le },
{ 297, "float8gt", 2, true, false, float8gt },
{ 298, "float8ge", 2, true, false, float8ge },
*/

#define FLOAT8_EQ	293
#define FLOAT8_NE	294
#define FLOAT8_LT	295
#define FLOAT8_LE	296
#define FLOAT8_GT	297
#define FLOAT8_GE	298

/* date func
  { 1086, "date_eq", 2, true, false, date_eq },
  { 1087, "date_lt", 2, true, false, date_lt },
  { 1088, "date_le", 2, true, false, date_le },
  { 1089, "date_gt", 2, true, false, date_gt },
  { 1090, "date_ge", 2, true, false, date_ge },
  { 1091, "date_ne", 2, true, false, date_ne },
  { 1092, "date_cmp", 2, true, false, date_cmp },
*/

#define DATE_EQ		1086
#define DATE_LT		1087
#define DATE_LE		1088
#define DATE_GT		1089
#define DATE_GE		1090
#define DATE_NE		1091
#define DATE_CMP	1092

#define FLOAT8MUL	216
#define FLOAT8DIV	217
#define FLOAT8PL	218
#define FLOAT8MI	219

/*
  { 216, "float8mul", 2, true, false, float8mul },
  { 217, "float8div", 2, true, false, float8div },
  { 218, "float8pl", 2, true, false, float8pl },
  { 219, "float8mi", 2, true, false, float8mi },
 */

/*
{ 141, "int4mul", 2, true, false, int4mul },
{ 154, "int4div", 2, true, false, int4div },
{ 177, "int4pl", 2, true, false, int4pl },
{ 181, "int4mi", 2, true, false, int4mi },
*/

#define INT4MUL		141
#define INT4DIV		154
#define INT4PL		177
#define INT4MI		181


#endif

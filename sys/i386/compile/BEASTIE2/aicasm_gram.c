#include <stdlib.h>
#ifndef lint
#ifdef __unused
__unused
#endif
static char const 
yyrcsid[] = "$FreeBSD: src/usr.bin/yacc/skeleton.c,v 1.37 2003/02/12 18:03:55 davidc Exp $";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
#if defined(__cplusplus) || __STDC__
static int yygrowstack(void);
#else
static int yygrowstack();
#endif
#define YYPREFIX "yy"
#line 2 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
/*-
 * Parser for the Aic7xxx SCSI Host adapter sequencer assembler.
 *
 * Copyright (c) 1997, 1998, 2000 Justin T. Gibbs.
 * Copyright (c) 2001, 2002 Adaptec Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    substantially similar to the "NO WARRANTY" disclaimer below
 *    ("Disclaimer") and any redistribution must be conditioned upon
 *    including a substantially similar Disclaimer requirement for further
 *    binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 * $Id: aicasm_gram.y,v 1.25 2006/12/29 13:08:46 yar Exp $
 *
 * $FreeBSD$
 */

#include <sys/types.h>

#include <inttypes.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#ifdef __linux__
#include "../queue.h"
#else
#include <sys/queue.h>
#endif

#include "aicasm.h"
#include "aicasm_symbol.h"
#include "aicasm_insformat.h"

int yylineno;
char *yyfilename;
char stock_prefix[] = "aic_";
char *prefix = stock_prefix;
char *patch_arg_list;
char *versions;
static char errbuf[255];
static char regex_pattern[255];
static symbol_t *cur_symbol;
static symbol_t *field_symbol;
static symbol_t *scb_or_sram_symbol;
static symtype cur_symtype;
static symbol_ref_t accumulator;
static symbol_ref_t mode_ptr;
static symbol_ref_t allones;
static symbol_ref_t allzeros;
static symbol_ref_t none;
static symbol_ref_t sindex;
static int instruction_ptr;
static int num_srams;
static int sram_or_scb_offset;
static int download_constant_count;
static int in_critical_section;
static u_int enum_increment;
static u_int enum_next_value;

static void process_field(int field_type, symbol_t *sym, int mask);
static void initialize_symbol(symbol_t *symbol);
static void add_macro_arg(const char *argtext, int position);
static void add_macro_body(const char *bodytext);
static void process_register(symbol_t **p_symbol);
static void format_1_instr(int opcode, symbol_ref_t *dest,
			   expression_t *immed, symbol_ref_t *src, int ret);
static void format_2_instr(int opcode, symbol_ref_t *dest,
			   expression_t *places, symbol_ref_t *src, int ret);
static void format_3_instr(int opcode, symbol_ref_t *src,
			   expression_t *immed, symbol_ref_t *address);
static void test_readable_symbol(symbol_t *symbol);
static void test_writable_symbol(symbol_t *symbol);
static void type_check(symbol_t *symbol, expression_t *expression, int and_op);
static void make_expression(expression_t *immed, int value);
static void add_conditional(symbol_t *symbol);
static void add_version(const char *verstring);
static int  is_download_const(expression_t *immed);

#define SRAM_SYMNAME "SRAM_BASE"
#define SCB_SYMNAME "SCB_BASE"
#line 114 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
typedef union {
	u_int		value;
	char		*str;
	symbol_t	*sym;
	symbol_ref_t	sym_ref;
	expression_t	expression;
} YYSTYPE;
#line 143 "aicasm_gram.c"
#define YYERRCODE 256
#define T_REGISTER 257
#define T_CONST 258
#define T_EXPORT 259
#define T_DOWNLOAD 260
#define T_SCB 261
#define T_SRAM 262
#define T_ALIAS 263
#define T_SIZE 264
#define T_EXPR_LSHIFT 265
#define T_EXPR_RSHIFT 266
#define T_ADDRESS 267
#define T_ACCESS_MODE 268
#define T_MODES 269
#define T_DEFINE 270
#define T_SET_SRC_MODE 271
#define T_SET_DST_MODE 272
#define T_MODE 273
#define T_BEGIN_CS 274
#define T_END_CS 275
#define T_FIELD 276
#define T_ENUM 277
#define T_MASK 278
#define T_NUMBER 279
#define T_PATH 280
#define T_STRING 281
#define T_ARG 282
#define T_MACROBODY 283
#define T_CEXPR 284
#define T_EOF 285
#define T_INCLUDE 286
#define T_VERSION 287
#define T_PREFIX 288
#define T_PATCH_ARG_LIST 289
#define T_SHR 290
#define T_SHL 291
#define T_ROR 292
#define T_ROL 293
#define T_MVI 294
#define T_MOV 295
#define T_CLR 296
#define T_BMOV 297
#define T_JMP 298
#define T_JC 299
#define T_JNC 300
#define T_JE 301
#define T_JNE 302
#define T_JNZ 303
#define T_JZ 304
#define T_CALL 305
#define T_ADD 306
#define T_ADC 307
#define T_INC 308
#define T_DEC 309
#define T_STC 310
#define T_CLC 311
#define T_CMP 312
#define T_NOT 313
#define T_XOR 314
#define T_TEST 315
#define T_AND 316
#define T_OR 317
#define T_RET 318
#define T_NOP 319
#define T_ACCUM 320
#define T_ALLONES 321
#define T_ALLZEROS 322
#define T_NONE 323
#define T_SINDEX 324
#define T_MODE_PTR 325
#define T_A 326
#define T_SYMBOL 327
#define T_NL 328
#define T_IF 329
#define T_ELSE 330
#define T_ELSE_IF 331
#define T_ENDIF 332
#define UMINUS 333
const short yylhs[] = {                                        -1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   19,   19,   20,   21,   22,   36,   23,   38,
   35,   37,   37,   39,   39,   39,   39,   39,   39,   39,
   39,   39,   39,   39,   39,   39,   39,   40,   41,   42,
   43,   17,   17,   16,   16,   54,   44,   56,   44,   44,
   57,   45,   58,   45,   55,   55,   59,   59,   46,   47,
   48,   49,   50,   51,   52,   53,    6,    6,    6,    6,
    6,    6,    6,    6,    6,    6,    6,    6,    6,   24,
   24,   60,   25,   25,   18,   18,   18,   61,   62,   64,
   26,   65,   66,   67,   27,   63,   63,   63,   63,   68,
   68,    1,    1,    1,    1,    3,    7,    8,    8,    4,
    5,    5,   10,   10,   29,   30,   31,   32,    9,    9,
   28,    2,    2,    2,    2,    2,    2,   33,   33,   33,
   33,   11,   11,   11,   11,   34,   34,   34,   34,   34,
   34,   34,   34,   34,   34,   34,   34,   34,   34,   34,
   12,   12,   12,   12,   34,   13,   13,   13,   13,   14,
   14,   15,   15,   34,   34,   34,   34,   34,   34,
};
const short yylen[] = {                                         2,
    1,    2,    1,    2,    1,    2,    1,    2,    1,    2,
    1,    2,    1,    2,    1,    2,    1,    2,    1,    2,
    1,    2,    1,    2,    1,    2,    1,    2,    1,    2,
    1,    2,    4,    4,    3,    3,    3,    0,    3,    0,
    5,    1,    2,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    2,    2,    2,
    2,    1,    3,    1,    1,    0,    5,    0,    7,    3,
    0,    5,    0,    7,    1,    3,    1,    2,    3,    2,
    1,    1,    1,    1,    1,    1,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    2,    2,    1,    1,    3,
    3,    2,    2,    5,    0,    1,    3,    0,    0,    0,
    9,    0,    0,    0,    9,    0,    1,    1,    2,    1,
    2,    1,    4,    4,    1,    1,    1,    1,    1,    1,
    0,    2,    0,    1,    3,    3,    2,    2,    0,    1,
    3,    1,    3,    3,    1,    3,    3,    3,    4,    2,
    1,    1,    1,    1,    1,    7,    7,    5,    5,    3,
    7,    3,    4,    8,    6,    6,    5,    4,    3,    2,
    1,    1,    1,    1,    7,    1,    1,    1,    1,    1,
    1,    1,    1,    3,    7,    7,    7,    5,    5,
};
const short yydefred[] = {                                      0,
   38,    0,  140,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  172,  171,  174,  173,    0,    0,
    0,    0,  176,  177,  178,  179,  154,  155,    0,    0,
    0,    0,    0,    0,  153,    0,  152,    0,    0,    0,
    0,    0,  151,    0,    0,    0,    0,    0,    1,    3,
    5,    7,    9,   11,   13,   15,   17,   19,   21,   23,
   25,   27,   29,   31,    0,    0,    0,  112,  108,  102,
    0,    0,  137,  138,    0,    0,    0,    0,    0,   98,
  125,    0,    0,    0,    0,  126,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  134,    0,    0,
    0,    0,  130,    0,    0,    0,    0,  170,    0,    0,
    0,  150,    2,    4,    6,    8,   10,   12,   14,   16,
   18,   20,   22,   24,   26,   28,   30,   32,    0,    0,
    0,    0,    0,    0,  103,    0,    0,   39,  101,   99,
    0,    0,    0,  135,  136,    0,    0,   37,   35,   36,
    0,   97,   96,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  162,    0,  160,    0,    0,    0,    0,  169,
  148,    0,  141,    0,    0,    0,    0,    0,    0,  184,
  106,    0,   40,    0,  113,  109,   33,   34,    0,    0,
   95,    0,    0,    0,    0,    0,    0,    0,   91,   92,
    0,    0,    0,  168,    0,  132,    0,    0,  163,    0,
  129,    0,    0,    0,    0,    0,    0,    0,  149,    0,
    0,  143,  144,  146,  147,    0,    0,    0,   58,    0,
    0,  124,  123,    0,  189,    0,  188,    0,  158,  159,
    0,  182,  183,    0,  167,  181,  180,    0,    0,    0,
    0,    0,  107,  104,    0,    0,    0,    0,    0,    0,
    0,   81,   83,   84,   85,   86,   82,    0,   42,   44,
   45,   46,   47,   48,   49,   50,   51,   52,   53,   54,
   55,   56,   57,  114,  110,  166,  165,    0,    0,    0,
    0,    0,    0,    0,    0,   80,   59,   60,   64,   65,
   62,    0,    0,    0,    0,    0,    0,   41,   43,    0,
    0,    0,  161,  187,  186,  185,  157,  156,  175,    0,
    0,    0,    0,    0,    0,  120,    0,    0,    0,    0,
  164,   63,    0,    0,    0,   75,    0,    0,    0,  115,
  121,  111,    0,    0,   67,    0,    0,   72,    0,   76,
    0,   69,   74,
};
const short yydgoto[] = {                                      44,
   86,  134,   87,   93,  170,  222,   89,  223,   45,  100,
   46,   47,   48,  258,  254,  311,  312,  192,   49,   50,
   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,
   61,   62,   63,   64,  336,   66,  278,  238,  279,  280,
  281,  282,  283,  284,  285,  286,  287,  288,  289,  290,
  291,  292,  293,  314,  345,  343,  316,  347,  346,   65,
  143,  241,  338,  321,  142,  240,  320,  339,
};
const short yysindex[] = {                                    316,
    0, -306,    0,  -80,  -75, -262, -181, -172,   55,   69,
  -11,   76,   80,   81,    0,    0,    0,    0,  127, -271,
 -271, -271,    0,    0,    0,    0,    0,    0, -271, -271,
 -230, -266, -271, -271,    0, -271,    0, -271,   71, -171,
 -135, -119,    0,  316, -177, -271, -271,  -46,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -38, -175,  156,    0,    0,    0,
   94,   95,    0,    0, -123, -122, -126, -120, -116,    0,
    0,   72,  130,  130,  130,    0,  118,   19, -154,   72,
    0,  124, -154, -171,  133,  134,  134,    0, -171,  112,
 -271,  114,    0,  136,  134,  140,  141,    0,  135,   63,
  -95,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  137,  149,
  153,   37,   84,  144,    0,  -76,   85,    0,    0,    0,
   19,  -58,  -58,    0,    0,  150,  179,    0,    0,    0,
 -244,    0,    0,  -35,  130,  130,  130,  130,  130,  130,
  130,  130,  130,  -46, -271,  -46,  155, -271, -271, -171,
 -171,  157,    0,  171,    0,  129, -171,  129,  129,    0,
    0,   96,    0,  129,  130,  -62,  -61,  -59,  -56,    0,
    0,   -3,    0,  -55,    0,    0,    0,    0,  128,  139,
    0, -171,   58,   58,   93,  145,   16,   16,    0,    0,
  166, -171,  168,    0,  182,    0,  169,  170,    0,  129,
    0,   19, -229,  174, -212,   19, -154,  134,    0,  134,
  -25,    0,    0,    0,    0,  -48,  -47,  272,    0,  -29,
  -29,    0,    0,  178,    0,  180,    0,  130,    0,    0,
 -171,    0,    0,  -46,    0,    0,    0,  -46,  -46, -171,
 -171, -171,    0,    0,  -89,  -37,  -27, -242,  -83,  -73,
  -70,    0,    0,    0,    0,    0,    0,  695,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -171,  192,  221,
  224,  227,  268,  276,  289,    0,    0,    0,    0,    0,
    0,  310,  130,  233,  130,  234,  130,    0,    0, -237,
 -237,  301,    0,    0,    0,    0,    0,    0,    0, -242,
   19,   86,   19,   86,   19,    0, -175,  282, -175,  290,
    0,    0,  291,  130,  -39,    0,  294,  -19, -175,    0,
    0,    0,   86,   19,    0,   86,   86,    0,  -13,    0,
  -10,    0,    0,
};
const short yyrindex[] = {                                     91,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  368,  368,    0,    0,    0,    0,    0,    0,    0,  368,
    0,    0,    0,   91,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  205,    0,    0,    0,    0,    0,  202,    0,  132,
  167,    0,    0,  368,    0,  -44,  -44,    0,  368,    0,
    0,    0,    0,    0,  -44,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  369,   95,    0,    0,   43,    0,    0,    0,    0,
  949,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  368,
  368,    0,    0,    0,    0,    0,  368,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  368,  401,  475,  875,  801,    1,   75,    0,    0,
    0,  368,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  122,    0,    0,    0,  163,    0,  -44,    0,  -44,
  -44,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  368,    0,    0,    0,    0,    0,    0,    0,    0,  368,
  368,  368,    0,    0,    0,    0,    0,    0,  306,  311,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  368,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  549,    0,    0,    0,    0,    0,    0,    0,  308,
  308,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  632,    0,  313,    0,  718,    0,  318,    0,  319,    0,
    0,    0,    0,   -8,    0,    0,    0,    0,  321,    0,
    0,    0,    0,   -4,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
};
const short yygindex[] = {                                      0,
   -9, -150,   47,   14,  -26,  336, -146, -125,    0,  716,
    0,    0,  -63,    0,    0,  108,    0,    0,  403,  404,
  405,  407,  408,  414,  415,  420,  425,  429,  430,  432,
  433,  434,  435,  436,   73,    0,    0,    0,  204,  -32,
 -118,    0, -188,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -283,    0,    0,    0,  131,    0,
    0,    0,  164,    0,    0,    0,    0,  147,
};
#define YYTABLESIZE 1279
const short yytable[] = {                                     133,
   89,  136,  159,  112,  356,  201,  162,  160,  202,  161,
   91,  163,  159,  211,  131,  213,  162,  160,  169,  161,
   67,  163,   76,  103,  356,  164,  103,  101,  107,  166,
  356,  268,  227,  356,  199,   77,  309,  237,   89,   78,
  236,   89,   68,   89,   89,   89,  104,   69,   75,  106,
  348,   98,  225,  228,   81,   90,  159,  162,  230,   89,
  162,  160,  163,  161,   70,  163,   92,   94,   95,  359,
  171,  252,  253,  361,   90,   96,   97,   99,  177,  186,
  105,  187,  200,  105,  310,  355,  105,   98,  158,  137,
  256,  257,  130,  131,  251,   81,   90,   71,  158,  162,
  160,  298,  161,  300,  163,  358,   72,  301,  302,  195,
  196,  362,   90,   73,  363,   90,   77,   90,   90,   90,
   78,  294,  295,   89,   89,   89,  188,   74,  189,  108,
  159,  337,  337,   90,  162,  160,   77,  161,  138,  163,
   78,   79,  158,   23,   24,   25,   98,  174,  110,  129,
   26,  137,  144,  145,  148,  103,  146,  147,  103,  103,
  149,  155,  151,  259,  150,  128,   85,  165,   85,   85,
  173,   83,  175,   83,   83,  122,  168,  169,  212,  176,
  128,  215,  216,  178,  179,  181,  162,  160,  182,  161,
  122,  163,  184,  180,  183,   85,  185,   90,   90,   90,
   83,  260,  190,  261,  262,  191,  128,  193,  194,  111,
  126,  197,  198,  214,  220,  219,  232,  233,  229,  234,
  242,  128,  235,  239,  245,  248,  247,  249,  250,  156,
  157,  243,  255,  263,  266,  264,  296,  306,  297,  156,
  157,  307,   99,  313,  135,  308,   99,   99,  122,   99,
  323,   99,   84,  315,   84,   84,  317,   89,   89,   89,
  127,   89,   89,   89,   89,   89,   89,   89,   89,   89,
   89,   89,   89,  131,   89,   89,   89,   89,   89,  324,
  132,   84,  325,  156,  157,  326,   89,   89,   89,   89,
   89,   89,   89,   89,   89,   89,   89,   89,   89,   89,
   89,   89,   89,   89,   89,   89,   89,   89,   89,   89,
   89,   89,   89,   89,   89,   89,   89,   89,   89,   89,
   89,   89,   89,   89,   89,   89,  327,   89,   99,   89,
   89,   90,   90,   90,  328,   90,   90,   90,   90,   90,
   90,   90,   90,   90,   90,   90,   90,  329,   90,   90,
   90,   90,   90,  330,   88,  332,  334,  156,  157,  341,
   90,   90,   90,   90,   90,   90,   90,   90,   90,   90,
   90,   90,   90,   90,   90,   90,   90,   90,   90,   90,
   90,   90,   90,   90,   90,   90,   90,   90,   90,   90,
   90,   90,   90,   90,   90,   90,   90,   90,   90,   90,
   93,   90,  141,   90,   90,   80,  350,   80,   80,  156,
  157,  351,  344,  353,  352,  139,  357,  139,  152,  153,
  154,  351,  128,  128,  128,  128,  133,  142,   66,  122,
  122,  122,  116,   71,   80,   73,  122,  342,   93,  128,
   43,   93,  117,  118,   93,  119,  113,  114,  115,  122,
  116,  117,   81,   82,  221,  140,  140,  118,  119,   93,
  127,  127,  127,  120,  130,  130,  130,  127,  121,   99,
   99,  130,  122,  123,   94,  124,  125,  126,  127,  128,
  128,  319,  140,  349,  340,    0,  360,    0,    0,    0,
   88,  203,  204,  205,  206,  207,  208,  209,  210,  127,
  127,  127,   99,   99,   99,    0,  127,    0,    0,   99,
    0,    0,   94,    0,  226,   94,    0,    0,   94,  127,
  231,    0,    0,   93,   93,   93,    0,    0,    0,    0,
    0,    0,    0,   94,  265,  266,    0,    0,  194,  267,
  268,    0,    0,    0,    0,    0,    0,  269,  270,  271,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    1,    2,    3,    0,    4,    5,    0,    0,
    0,    0,    0,   88,    0,    6,    7,    8,    0,    9,
   10,  272,  273,  274,  275,  276,  277,   94,   94,   94,
    0,   11,   12,   13,   14,   15,   16,   17,   18,   19,
   20,   21,   22,   23,   24,   25,    0,    0,    0,    0,
   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,
   36,   37,   38,   39,   40,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   41,   42,    0,    0,  331,    0,
  333,    0,  335,    0,    0,    0,    0,   93,   93,   93,
    0,   93,   93,   93,   93,   93,   93,   93,   93,   93,
   93,   93,   93,   61,   93,   93,   93,   93,   93,  354,
    0,    0,    0,    0,    0,    0,   93,   93,   93,   93,
   93,   93,   93,   93,   93,   93,   93,   93,   93,   93,
   93,   93,   93,   93,   93,   93,   93,   93,   93,   93,
   93,   93,   93,   93,   93,   93,   93,   93,   93,   93,
   93,   93,   93,   93,   93,   93,    0,   93,    0,   93,
   93,   94,   94,   94,    0,   94,   94,   94,   94,   94,
   94,   94,   94,   94,   94,   94,   94,  102,   94,   94,
   94,   94,   94,    0,   68,  109,   70,    0,    0,    0,
   94,   94,   94,   94,   94,   94,   94,   94,   94,   94,
   94,   94,   94,   94,   94,   94,   94,   94,   94,   94,
   94,   94,   94,   94,   94,   94,   94,   94,   94,   94,
   94,   94,   94,   94,   94,   94,   94,   94,   94,   94,
   88,   94,    0,   94,   94,    0,    0,    0,    0,  167,
    0,   61,   61,    0,  172,   61,   61,   61,    0,  318,
    0,    0,    0,    0,   61,   61,   61,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   88,    0,
    0,   88,   79,    0,   88,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   88,
    0,    0,    0,    0,    0,    0,    0,    0,   61,   61,
   61,   61,   61,   61,   87,   61,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  217,  218,    0,    0,    0,
    0,    0,  224,    0,   70,   70,    0,    0,   70,   70,
   70,    0,    0,    0,    0,    0,    0,   70,   70,   70,
    0,    0,    0,    0,    0,   87,    0,  244,   87,    0,
    0,    0,    0,   88,   88,   88,    0,  246,    0,    0,
    0,    0,    0,   87,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  100,    0,
    0,   70,   70,   70,   70,   70,   70,  265,  266,    0,
    0,  194,  267,  268,    0,    0,  299,    0,    0,    0,
  269,  270,  271,    0,    0,  303,  304,  305,    0,    0,
   79,   79,    0,    0,   79,   79,   79,    0,    0,    0,
    0,    0,    0,   79,   79,   79,    0,   87,   87,   87,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  322,  272,  273,  274,  275,  276,  277,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   79,   79,   79,
   79,   79,   79,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   88,   88,   88,
    0,   88,   88,   88,   88,    0,    0,   88,   88,   88,
   88,   88,   88,  100,   88,   88,   88,   88,   88,    0,
    0,    0,    0,    0,    0,    0,   88,   88,   88,   88,
   88,   88,   88,   88,   88,   88,   88,   88,   88,   88,
   88,   88,   88,   88,   88,   88,   88,   88,   88,   88,
   88,   88,   88,   88,   88,   88,   88,   88,   88,   88,
   88,   88,   88,   88,   88,   88,    0,   88,    0,   88,
   88,   87,   87,   87,    0,   87,   87,   87,   87,    0,
    0,   87,   87,   87,   87,   87,   87,    0,   87,   87,
   87,   87,   87,    0,    0,    0,    0,    0,    0,    0,
   87,   87,   87,   87,   87,   87,   87,   87,   87,   87,
   87,   87,   87,   87,   87,   87,   87,   87,   87,   87,
   87,   87,   87,   87,   87,   87,   87,   87,   87,   87,
   87,   87,   87,   87,   87,   87,   87,   87,   87,   87,
    0,   87,    0,   87,   87,  100,  100,  100,    0,  100,
  100,    0,    0,    0,    0,    0,    0,    0,  100,  100,
  100,    0,  100,  100,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  100,  100,  100,  100,  100,  100,
  100,  100,  100,  100,  100,  100,  100,  100,  100,    0,
    0,    0,    0,  100,  100,  100,  100,  100,  100,  100,
  100,  100,  100,  100,  100,  100,  100,  100,    0,    0,
    0,    0,    0,    0,    0,  100,    0,  100,  100,
};
const short yycheck[] = {                                      46,
    0,   40,   38,  123,   44,   41,   42,   43,  155,   45,
   20,   47,   38,  164,   59,  166,   42,   43,   44,   45,
  327,   47,   34,   33,   44,   89,   36,  294,   38,   93,
   44,  269,  179,   44,  279,   44,  279,   41,   38,   44,
   44,   41,  123,   43,   44,   45,   33,  123,   60,   36,
  334,  318,  178,  179,  326,  327,   38,   42,  184,   59,
   42,   43,   47,   45,  327,   47,   20,   21,   22,  353,
   97,  301,  302,  357,    0,   29,   30,   31,  105,   43,
   34,   45,  327,   41,  327,  125,   44,  318,  124,  327,
  303,  304,   46,   47,  220,  326,  327,  279,  124,   42,
   43,  248,   45,  254,   47,  125,  279,  258,  259,  142,
  143,  125,   38,   59,  125,   41,  125,   43,   44,   45,
  125,  240,  241,  123,  124,  125,   43,   59,   45,   59,
   38,  320,  321,   59,   42,   43,   61,   45,   66,   47,
   61,   61,  124,  298,  299,  300,  318,  101,  284,  327,
  305,  327,   59,   59,  281,  165,  280,  280,  168,  169,
  281,   44,   91,  227,  281,   44,   40,   44,   40,   40,
   59,   45,   59,   45,   45,   44,   44,   44,  165,   44,
   59,  168,  169,   44,   44,  123,   42,   43,  284,   45,
   59,   47,   44,   59,   58,   40,   44,  123,  124,  125,
   45,  228,   59,  230,  231,  282,   44,  123,  267,  329,
   44,   62,   34,   59,   44,   59,  279,  279,  123,  279,
   93,   59,  279,  279,   59,   44,   59,   59,   59,  265,
  266,   93,   59,  282,  264,  283,   59,  327,   59,  265,
  266,  279,   38,  327,  283,  273,   42,   43,   44,   45,
   59,   47,  126,  327,  126,  126,  327,  257,  258,  259,
   59,  261,  262,  263,  264,  265,  266,  267,  268,  269,
  270,  271,  272,  318,  274,  275,  276,  277,  278,   59,
  327,  126,   59,  265,  266,   59,  286,  287,  288,  289,
  290,  291,  292,  293,  294,  295,  296,  297,  298,  299,
  300,  301,  302,  303,  304,  305,  306,  307,  308,  309,
  310,  311,  312,  313,  314,  315,  316,  317,  318,  319,
  320,  321,  322,  323,  324,  325,   59,  327,  124,  329,
  330,  257,  258,  259,   59,  261,  262,  263,  264,  265,
  266,  267,  268,  269,  270,  271,  272,   59,  274,  275,
  276,  277,  278,   44,   19,  123,  123,  265,  266,   59,
  286,  287,  288,  289,  290,  291,  292,  293,  294,  295,
  296,  297,  298,  299,  300,  301,  302,  303,  304,  305,
  306,  307,  308,  309,  310,  311,  312,  313,  314,  315,
  316,  317,  318,  319,  320,  321,  322,  323,  324,  325,
    0,  327,   67,  329,  330,  279,  125,  279,  279,  265,
  266,  339,  327,  123,  125,  260,  123,  327,   83,   84,
   85,  349,  301,  302,  303,  304,   59,   59,  123,  298,
  299,  300,  125,  123,  279,  123,  305,  330,   38,  318,
  125,   41,  125,  125,   44,  125,   44,   44,   44,  318,
   44,   44,  326,  327,  326,  327,  327,   44,   44,   59,
  298,  299,  300,   44,  298,  299,  300,  305,   44,  265,
  266,  305,   44,   44,    0,   44,   44,   44,   44,   44,
  318,  278,  327,  337,  321,   -1,  356,   -1,   -1,   -1,
  155,  156,  157,  158,  159,  160,  161,  162,  163,  298,
  299,  300,  298,  299,  300,   -1,  305,   -1,   -1,  305,
   -1,   -1,   38,   -1,  179,   41,   -1,   -1,   44,  318,
  185,   -1,   -1,  123,  124,  125,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   59,  263,  264,   -1,   -1,  267,  268,
  269,   -1,   -1,   -1,   -1,   -1,   -1,  276,  277,  278,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  257,  258,  259,   -1,  261,  262,   -1,   -1,
   -1,   -1,   -1,  248,   -1,  270,  271,  272,   -1,  274,
  275,  320,  321,  322,  323,  324,  325,  123,  124,  125,
   -1,  286,  287,  288,  289,  290,  291,  292,  293,  294,
  295,  296,  297,  298,  299,  300,   -1,   -1,   -1,   -1,
  305,  306,  307,  308,  309,  310,  311,  312,  313,  314,
  315,  316,  317,  318,  319,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  329,  330,   -1,   -1,  313,   -1,
  315,   -1,  317,   -1,   -1,   -1,   -1,  257,  258,  259,
   -1,  261,  262,  263,  264,  265,  266,  267,  268,  269,
  270,  271,  272,  125,  274,  275,  276,  277,  278,  344,
   -1,   -1,   -1,   -1,   -1,   -1,  286,  287,  288,  289,
  290,  291,  292,  293,  294,  295,  296,  297,  298,  299,
  300,  301,  302,  303,  304,  305,  306,  307,  308,  309,
  310,  311,  312,  313,  314,  315,  316,  317,  318,  319,
  320,  321,  322,  323,  324,  325,   -1,  327,   -1,  329,
  330,  257,  258,  259,   -1,  261,  262,  263,  264,  265,
  266,  267,  268,  269,  270,  271,  272,   32,  274,  275,
  276,  277,  278,   -1,  123,   40,  125,   -1,   -1,   -1,
  286,  287,  288,  289,  290,  291,  292,  293,  294,  295,
  296,  297,  298,  299,  300,  301,  302,  303,  304,  305,
  306,  307,  308,  309,  310,  311,  312,  313,  314,  315,
  316,  317,  318,  319,  320,  321,  322,  323,  324,  325,
    0,  327,   -1,  329,  330,   -1,   -1,   -1,   -1,   94,
   -1,  263,  264,   -1,   99,  267,  268,  269,   -1,  125,
   -1,   -1,   -1,   -1,  276,  277,  278,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   38,   -1,
   -1,   41,  125,   -1,   44,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   59,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  320,  321,
  322,  323,  324,  325,    0,  327,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  170,  171,   -1,   -1,   -1,
   -1,   -1,  177,   -1,  263,  264,   -1,   -1,  267,  268,
  269,   -1,   -1,   -1,   -1,   -1,   -1,  276,  277,  278,
   -1,   -1,   -1,   -1,   -1,   41,   -1,  202,   44,   -1,
   -1,   -1,   -1,  123,  124,  125,   -1,  212,   -1,   -1,
   -1,   -1,   -1,   59,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,   -1,
   -1,  320,  321,  322,  323,  324,  325,  263,  264,   -1,
   -1,  267,  268,  269,   -1,   -1,  251,   -1,   -1,   -1,
  276,  277,  278,   -1,   -1,  260,  261,  262,   -1,   -1,
  263,  264,   -1,   -1,  267,  268,  269,   -1,   -1,   -1,
   -1,   -1,   -1,  276,  277,  278,   -1,  123,  124,  125,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  298,  320,  321,  322,  323,  324,  325,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  320,  321,  322,
  323,  324,  325,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,  259,
   -1,  261,  262,  263,  264,   -1,   -1,  267,  268,  269,
  270,  271,  272,  125,  274,  275,  276,  277,  278,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  286,  287,  288,  289,
  290,  291,  292,  293,  294,  295,  296,  297,  298,  299,
  300,  301,  302,  303,  304,  305,  306,  307,  308,  309,
  310,  311,  312,  313,  314,  315,  316,  317,  318,  319,
  320,  321,  322,  323,  324,  325,   -1,  327,   -1,  329,
  330,  257,  258,  259,   -1,  261,  262,  263,  264,   -1,
   -1,  267,  268,  269,  270,  271,  272,   -1,  274,  275,
  276,  277,  278,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  286,  287,  288,  289,  290,  291,  292,  293,  294,  295,
  296,  297,  298,  299,  300,  301,  302,  303,  304,  305,
  306,  307,  308,  309,  310,  311,  312,  313,  314,  315,
  316,  317,  318,  319,  320,  321,  322,  323,  324,  325,
   -1,  327,   -1,  329,  330,  257,  258,  259,   -1,  261,
  262,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  270,  271,
  272,   -1,  274,  275,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  286,  287,  288,  289,  290,  291,
  292,  293,  294,  295,  296,  297,  298,  299,  300,   -1,
   -1,   -1,   -1,  305,  306,  307,  308,  309,  310,  311,
  312,  313,  314,  315,  316,  317,  318,  319,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  327,   -1,  329,  330,
};
#define YYFINAL 44
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 333
#if YYDEBUG
const char * const yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'\"'",0,0,0,"'&'",0,"'('","')'","'*'","'+'","','","'-'","'.'","'/'",0,0,0,0,0,
0,0,0,0,0,"':'","';'","'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,"'{'","'|'","'}'","'~'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"T_REGISTER","T_CONST",
"T_EXPORT","T_DOWNLOAD","T_SCB","T_SRAM","T_ALIAS","T_SIZE","T_EXPR_LSHIFT",
"T_EXPR_RSHIFT","T_ADDRESS","T_ACCESS_MODE","T_MODES","T_DEFINE",
"T_SET_SRC_MODE","T_SET_DST_MODE","T_MODE","T_BEGIN_CS","T_END_CS","T_FIELD",
"T_ENUM","T_MASK","T_NUMBER","T_PATH","T_STRING","T_ARG","T_MACROBODY",
"T_CEXPR","T_EOF","T_INCLUDE","T_VERSION","T_PREFIX","T_PATCH_ARG_LIST","T_SHR",
"T_SHL","T_ROR","T_ROL","T_MVI","T_MOV","T_CLR","T_BMOV","T_JMP","T_JC","T_JNC",
"T_JE","T_JNE","T_JNZ","T_JZ","T_CALL","T_ADD","T_ADC","T_INC","T_DEC","T_STC",
"T_CLC","T_CMP","T_NOT","T_XOR","T_TEST","T_AND","T_OR","T_RET","T_NOP",
"T_ACCUM","T_ALLONES","T_ALLZEROS","T_NONE","T_SINDEX","T_MODE_PTR","T_A",
"T_SYMBOL","T_NL","T_IF","T_ELSE","T_ELSE_IF","T_ENDIF","UMINUS",
};
const char * const yyrule[] = {
"$accept : program",
"program : include",
"program : program include",
"program : prefix",
"program : program prefix",
"program : patch_arg_list",
"program : program patch_arg_list",
"program : version",
"program : program version",
"program : register",
"program : program register",
"program : constant",
"program : program constant",
"program : macrodefn",
"program : program macrodefn",
"program : scratch_ram",
"program : program scratch_ram",
"program : scb",
"program : program scb",
"program : label",
"program : program label",
"program : set_src_mode",
"program : program set_src_mode",
"program : set_dst_mode",
"program : program set_dst_mode",
"program : critical_section_start",
"program : program critical_section_start",
"program : critical_section_end",
"program : program critical_section_end",
"program : conditional",
"program : program conditional",
"program : code",
"program : program code",
"include : T_INCLUDE '<' T_PATH '>'",
"include : T_INCLUDE '\"' T_PATH '\"'",
"prefix : T_PREFIX '=' T_STRING",
"patch_arg_list : T_PATCH_ARG_LIST '=' T_STRING",
"version : T_VERSION '=' T_STRING",
"$$1 :",
"register : T_REGISTER $$1 reg_definition",
"$$2 :",
"reg_definition : T_SYMBOL '{' $$2 reg_attribute_list '}'",
"reg_attribute_list : reg_attribute",
"reg_attribute_list : reg_attribute_list reg_attribute",
"reg_attribute : reg_address",
"reg_attribute : size",
"reg_attribute : access_mode",
"reg_attribute : modes",
"reg_attribute : field_defn",
"reg_attribute : enum_defn",
"reg_attribute : mask_defn",
"reg_attribute : alias",
"reg_attribute : accumulator",
"reg_attribute : mode_pointer",
"reg_attribute : allones",
"reg_attribute : allzeros",
"reg_attribute : none",
"reg_attribute : sindex",
"reg_address : T_ADDRESS T_NUMBER",
"size : T_SIZE T_NUMBER",
"access_mode : T_ACCESS_MODE T_MODE",
"modes : T_MODES mode_list",
"mode_list : mode_value",
"mode_list : mode_list ',' mode_value",
"mode_value : T_NUMBER",
"mode_value : T_SYMBOL",
"$$3 :",
"field_defn : T_FIELD $$3 '{' enum_entry_list '}'",
"$$4 :",
"field_defn : T_FIELD T_SYMBOL expression $$4 '{' enum_entry_list '}'",
"field_defn : T_FIELD T_SYMBOL expression",
"$$5 :",
"enum_defn : T_ENUM $$5 '{' enum_entry_list '}'",
"$$6 :",
"enum_defn : T_ENUM T_SYMBOL expression $$6 '{' enum_entry_list '}'",
"enum_entry_list : enum_entry",
"enum_entry_list : enum_entry_list ',' enum_entry",
"enum_entry : T_SYMBOL",
"enum_entry : T_SYMBOL expression",
"mask_defn : T_MASK T_SYMBOL expression",
"alias : T_ALIAS T_SYMBOL",
"accumulator : T_ACCUM",
"mode_pointer : T_MODE_PTR",
"allones : T_ALLONES",
"allzeros : T_ALLZEROS",
"none : T_NONE",
"sindex : T_SINDEX",
"expression : expression '|' expression",
"expression : expression '&' expression",
"expression : expression '+' expression",
"expression : expression '-' expression",
"expression : expression '*' expression",
"expression : expression '/' expression",
"expression : expression T_EXPR_LSHIFT expression",
"expression : expression T_EXPR_RSHIFT expression",
"expression : '(' expression ')'",
"expression : '~' expression",
"expression : '-' expression",
"expression : T_NUMBER",
"expression : T_SYMBOL",
"constant : T_CONST T_SYMBOL expression",
"constant : T_CONST T_SYMBOL T_DOWNLOAD",
"macrodefn_prologue : T_DEFINE T_SYMBOL",
"macrodefn : macrodefn_prologue T_MACROBODY",
"macrodefn : macrodefn_prologue '(' macro_arglist ')' T_MACROBODY",
"macro_arglist :",
"macro_arglist : T_ARG",
"macro_arglist : macro_arglist ',' T_ARG",
"$$7 :",
"$$8 :",
"$$9 :",
"scratch_ram : T_SRAM '{' $$7 reg_address $$8 size $$9 scb_or_sram_attributes '}'",
"$$10 :",
"$$11 :",
"$$12 :",
"scb : T_SCB '{' $$10 reg_address $$11 size $$12 scb_or_sram_attributes '}'",
"scb_or_sram_attributes :",
"scb_or_sram_attributes : modes",
"scb_or_sram_attributes : scb_or_sram_reg_list",
"scb_or_sram_attributes : modes scb_or_sram_reg_list",
"scb_or_sram_reg_list : reg_definition",
"scb_or_sram_reg_list : scb_or_sram_reg_list reg_definition",
"reg_symbol : T_SYMBOL",
"reg_symbol : T_SYMBOL '[' T_SYMBOL ']'",
"reg_symbol : T_SYMBOL '[' T_NUMBER ']'",
"reg_symbol : T_A",
"destination : reg_symbol",
"immediate : expression",
"immediate_or_a : expression",
"immediate_or_a : T_A",
"source : reg_symbol",
"opt_source :",
"opt_source : ',' source",
"ret :",
"ret : T_RET",
"set_src_mode : T_SET_SRC_MODE T_NUMBER ';'",
"set_dst_mode : T_SET_DST_MODE T_NUMBER ';'",
"critical_section_start : T_BEGIN_CS ';'",
"critical_section_end : T_END_CS ';'",
"export :",
"export : T_EXPORT",
"label : export T_SYMBOL ':'",
"address : T_SYMBOL",
"address : T_SYMBOL '+' T_NUMBER",
"address : T_SYMBOL '-' T_NUMBER",
"address : '.'",
"address : '.' '+' T_NUMBER",
"address : '.' '-' T_NUMBER",
"conditional : T_IF T_CEXPR '{'",
"conditional : T_ELSE T_IF T_CEXPR '{'",
"conditional : T_ELSE '{'",
"conditional : '}'",
"f1_opcode : T_AND",
"f1_opcode : T_XOR",
"f1_opcode : T_ADD",
"f1_opcode : T_ADC",
"code : f1_opcode destination ',' immediate_or_a opt_source ret ';'",
"code : T_OR reg_symbol ',' immediate_or_a opt_source ret ';'",
"code : T_INC destination opt_source ret ';'",
"code : T_DEC destination opt_source ret ';'",
"code : T_CLC ret ';'",
"code : T_CLC T_MVI destination ',' immediate_or_a ret ';'",
"code : T_STC ret ';'",
"code : T_STC destination ret ';'",
"code : T_BMOV destination ',' source ',' immediate ret ';'",
"code : T_MOV destination ',' source ret ';'",
"code : T_MVI destination ',' immediate ret ';'",
"code : T_NOT destination opt_source ret ';'",
"code : T_CLR destination ret ';'",
"code : T_NOP ret ';'",
"code : T_RET ';'",
"f2_opcode : T_SHL",
"f2_opcode : T_SHR",
"f2_opcode : T_ROL",
"f2_opcode : T_ROR",
"code : f2_opcode destination ',' expression opt_source ret ';'",
"jmp_jc_jnc_call : T_JMP",
"jmp_jc_jnc_call : T_JC",
"jmp_jc_jnc_call : T_JNC",
"jmp_jc_jnc_call : T_CALL",
"jz_jnz : T_JZ",
"jz_jnz : T_JNZ",
"je_jne : T_JE",
"je_jne : T_JNE",
"code : jmp_jc_jnc_call address ';'",
"code : T_OR reg_symbol ',' immediate jmp_jc_jnc_call address ';'",
"code : T_TEST source ',' immediate_or_a jz_jnz address ';'",
"code : T_CMP source ',' immediate_or_a je_jne address ';'",
"code : T_MOV source jmp_jc_jnc_call address ';'",
"code : T_MVI immediate jmp_jc_jnc_call address ';'",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;
#line 1380 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"

static void
process_field(int field_type, symbol_t *sym, int value)
{
	/*
	 * Add the current register to its
	 * symbol list, if it already exists,
	 * warn if we are setting it to a
	 * different value, or in the bit to
	 * the "allowed bits" of this register.
	 */
	if (sym->type == UNINITIALIZED) {
		sym->type = field_type;
		initialize_symbol(sym);
		sym->info.finfo->value = value;
		if (field_type != ENUM_ENTRY) {
			if (field_type != MASK && value == 0) {
				stop("Empty Field, or Enum", EX_DATAERR);
				/* NOTREACHED */
			}
			sym->info.finfo->value = value;
			sym->info.finfo->mask = value;
		} else if (field_symbol != NULL) {
			sym->info.finfo->mask = field_symbol->info.finfo->value;
		} else {
			sym->info.finfo->mask = 0xFF;
		}
	} else if (sym->type != field_type) {
		stop("Field definition mirrors a definition of the same "
		     " name, but a different type", EX_DATAERR);
		/* NOTREACHED */
	} else if (value != sym->info.finfo->value) {
		stop("Field redefined with a conflicting value", EX_DATAERR);
		/* NOTREACHED */
	}
	/* Fail if this symbol is already listed */
	if (symlist_search(&(sym->info.finfo->symrefs),
			   cur_symbol->name) != NULL) {
		stop("Field defined multiple times for register", EX_DATAERR);
		/* NOTREACHED */
	}
	symlist_add(&(sym->info.finfo->symrefs), cur_symbol,
		    SYMLIST_INSERT_HEAD);
	cur_symbol->info.rinfo->valid_bitmask |= sym->info.finfo->mask;
	cur_symbol->info.rinfo->typecheck_masks = TRUE;
	symlist_add(&(cur_symbol->info.rinfo->fields), sym, SYMLIST_SORT);
}

static void
initialize_symbol(symbol_t *symbol)
{
	switch (symbol->type) {
	case UNINITIALIZED:
		stop("Call to initialize_symbol with type field unset",
		     EX_SOFTWARE);
		/* NOTREACHED */
		break;
	case REGISTER:
	case SRAMLOC:
	case SCBLOC:
		symbol->info.rinfo =
		    (struct reg_info *)malloc(sizeof(struct reg_info));
		if (symbol->info.rinfo == NULL) {
			stop("Can't create register info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.rinfo, 0,
		       sizeof(struct reg_info));
		SLIST_INIT(&(symbol->info.rinfo->fields));
		/*
		 * Default to allowing access in all register modes
		 * or to the mode specified by the SCB or SRAM space
		 * we are in.
		 */
		if (scb_or_sram_symbol != NULL)
			symbol->info.rinfo->modes =
			    scb_or_sram_symbol->info.rinfo->modes;
		else
			symbol->info.rinfo->modes = ~0;
		break;
	case ALIAS:
		symbol->info.ainfo =
		    (struct alias_info *)malloc(sizeof(struct alias_info));
		if (symbol->info.ainfo == NULL) {
			stop("Can't create alias info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.ainfo, 0,
		       sizeof(struct alias_info));
		break;
	case MASK:
	case FIELD:
	case ENUM:
	case ENUM_ENTRY:
		symbol->info.finfo =
		    (struct field_info *)malloc(sizeof(struct field_info));
		if (symbol->info.finfo == NULL) {
			stop("Can't create field info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.finfo, 0, sizeof(struct field_info));
		SLIST_INIT(&(symbol->info.finfo->symrefs));
		break;
	case CONST:
	case DOWNLOAD_CONST:
		symbol->info.cinfo =
		    (struct const_info *)malloc(sizeof(struct const_info));
		if (symbol->info.cinfo == NULL) {
			stop("Can't create alias info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.cinfo, 0,
		       sizeof(struct const_info));
		break;
	case LABEL:
		symbol->info.linfo =
		    (struct label_info *)malloc(sizeof(struct label_info));
		if (symbol->info.linfo == NULL) {
			stop("Can't create label info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.linfo, 0,
		       sizeof(struct label_info));
		break;
	case CONDITIONAL:
		symbol->info.condinfo =
		    (struct cond_info *)malloc(sizeof(struct cond_info));
		if (symbol->info.condinfo == NULL) {
			stop("Can't create conditional info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.condinfo, 0,
		       sizeof(struct cond_info));
		break;
	case MACRO:
		symbol->info.macroinfo = 
		    (struct macro_info *)malloc(sizeof(struct macro_info));
		if (symbol->info.macroinfo == NULL) {
			stop("Can't create macro info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.macroinfo, 0,
		       sizeof(struct macro_info));
		STAILQ_INIT(&symbol->info.macroinfo->args);
		break;
	default:
		stop("Call to initialize_symbol with invalid symbol type",
		     EX_SOFTWARE);
		/* NOTREACHED */
		break;
	}
}

static void
add_macro_arg(const char *argtext, int argnum)
{
	struct macro_arg *marg;
	int i;
	int retval;
		

	if (cur_symbol == NULL || cur_symbol->type != MACRO) {
		stop("Invalid current symbol for adding macro arg",
		     EX_SOFTWARE);
		/* NOTREACHED */
	}

	marg = (struct macro_arg *)malloc(sizeof(*marg));
	if (marg == NULL) {
		stop("Can't create macro_arg structure", EX_SOFTWARE);
		/* NOTREACHED */
	}
	marg->replacement_text = NULL;
	retval = snprintf(regex_pattern, sizeof(regex_pattern),
			  "[^-/A-Za-z0-9_](%s)([^-/A-Za-z0-9_]|$)",
			  argtext);
	if (retval >= sizeof(regex_pattern)) {
		stop("Regex text buffer too small for arg",
		     EX_SOFTWARE);
		/* NOTREACHED */
	}
	retval = regcomp(&marg->arg_regex, regex_pattern, REG_EXTENDED);
	if (retval != 0) {
		stop("Regex compilation failed", EX_SOFTWARE);
		/* NOTREACHED */
	}
	STAILQ_INSERT_TAIL(&cur_symbol->info.macroinfo->args, marg, links);
}

static void
add_macro_body(const char *bodytext)
{
	if (cur_symbol == NULL || cur_symbol->type != MACRO) {
		stop("Invalid current symbol for adding macro arg",
		     EX_SOFTWARE);
		/* NOTREACHED */
	}
	cur_symbol->info.macroinfo->body = strdup(bodytext);
	if (cur_symbol->info.macroinfo->body == NULL) {
		stop("Can't duplicate macro body text", EX_SOFTWARE);
		/* NOTREACHED */
	}
}

static void
process_register(symbol_t **p_symbol)
{
	symbol_t *symbol = *p_symbol;

	if (symbol->type == UNINITIALIZED) {
		snprintf(errbuf, sizeof(errbuf), "Undefined register %s",
			 symbol->name);
		stop(errbuf, EX_DATAERR);
		/* NOTREACHED */
	} else if (symbol->type == ALIAS) {
		*p_symbol = symbol->info.ainfo->parent;
	} else if ((symbol->type != REGISTER)
		&& (symbol->type != SCBLOC)
		&& (symbol->type != SRAMLOC)) {
		snprintf(errbuf, sizeof(errbuf),
			 "Specified symbol %s is not a register",
			 symbol->name);
		stop(errbuf, EX_DATAERR);
	}
}

static void
format_1_instr(int opcode, symbol_ref_t *dest, expression_t *immed,
	       symbol_ref_t *src, int ret)
{
	struct instruction *instr;
	struct ins_format1 *f1_instr;

	if (src->symbol == NULL)
		src = dest;

	/* Test register permissions */
	test_writable_symbol(dest->symbol);
	test_readable_symbol(src->symbol);

	/* Ensure that immediate makes sense for this destination */
	type_check(dest->symbol, immed, opcode);

	/* Allocate sequencer space for the instruction and fill it out */
	instr = seq_alloc();
	f1_instr = &instr->format.format1;
	f1_instr->ret = ret ? 1 : 0;
	f1_instr->opcode = opcode;
	f1_instr->destination = dest->symbol->info.rinfo->address
			      + dest->offset;
	f1_instr->source = src->symbol->info.rinfo->address
			 + src->offset;
	f1_instr->immediate = immed->value;

	if (is_download_const(immed))
		f1_instr->parity = 1;
	else if (dest->symbol == mode_ptr.symbol) {
		u_int src_value;
		u_int dst_value;

		/*
		 * Attempt to update mode information if
		 * we are operating on the mode register.
		 */
		if (src->symbol == allones.symbol)
			src_value = 0xFF;
		else if (src->symbol == allzeros.symbol)
			src_value = 0;
		else if (src->symbol == mode_ptr.symbol)
			src_value = (dst_mode << 4) | src_mode;
		else
			goto cant_update;

		switch (opcode) {
		case AIC_OP_AND:
			dst_value = src_value & immed->value;
			break;
		case AIC_OP_XOR:
			dst_value = src_value ^ immed->value;
			break;
		case AIC_OP_ADD:
			dst_value = (src_value + immed->value) & 0xFF;
			break;
		case AIC_OP_OR:
			dst_value = src_value | immed->value;
			break;
		case AIC_OP_BMOV:
			dst_value = src_value;
			break;
		default:
			goto cant_update;
		}
		src_mode = dst_value & 0xF;
		dst_mode = (dst_value >> 4) & 0xF;
	}

cant_update:
	symlist_free(&immed->referenced_syms);
	instruction_ptr++;
}

static void
format_2_instr(int opcode, symbol_ref_t *dest, expression_t *places,
	       symbol_ref_t *src, int ret)
{
	struct instruction *instr;
	struct ins_format2 *f2_instr;
	uint8_t shift_control;

	if (src->symbol == NULL)
		src = dest;

	/* Test register permissions */
	test_writable_symbol(dest->symbol);
	test_readable_symbol(src->symbol);

	/* Allocate sequencer space for the instruction and fill it out */
	instr = seq_alloc();
	f2_instr = &instr->format.format2;
	f2_instr->ret = ret ? 1 : 0;
	f2_instr->opcode = AIC_OP_ROL;
	f2_instr->destination = dest->symbol->info.rinfo->address
			      + dest->offset;
	f2_instr->source = src->symbol->info.rinfo->address
			 + src->offset;
	if (places->value > 8 || places->value <= 0) {
		stop("illegal shift value", EX_DATAERR);
		/* NOTREACHED */
	}
	switch (opcode) {
	case AIC_OP_SHL:
		if (places->value == 8)
			shift_control = 0xf0;
		else
			shift_control = (places->value << 4) | places->value;
		break;
	case AIC_OP_SHR:
		if (places->value == 8) {
			shift_control = 0xf8;
		} else {
			shift_control = (places->value << 4)
				      | (8 - places->value)
				      | 0x08;
		}
		break;
	case AIC_OP_ROL:
		shift_control = places->value & 0x7;
		break;
	case AIC_OP_ROR:
		shift_control = (8 - places->value) | 0x08;
		break;
	default:
		shift_control = 0; /* Quiet Compiler */
		stop("Invalid shift operation specified", EX_SOFTWARE);
		/* NOTREACHED */
		break;
	};
	f2_instr->shift_control = shift_control;
	symlist_free(&places->referenced_syms);
	instruction_ptr++;
}

static void
format_3_instr(int opcode, symbol_ref_t *src,
	       expression_t *immed, symbol_ref_t *address)
{
	struct instruction *instr;
	struct ins_format3 *f3_instr;
	int addr;

	/* Test register permissions */
	test_readable_symbol(src->symbol);

	/* Ensure that immediate makes sense for this source */
	type_check(src->symbol, immed, opcode);

	/* Allocate sequencer space for the instruction and fill it out */
	instr = seq_alloc();
	f3_instr = &instr->format.format3;
	if (address->symbol == NULL) {
		/* 'dot' referrence.  Use the current instruction pointer */
		addr = instruction_ptr + address->offset;
	} else if (address->symbol->type == UNINITIALIZED) {
		/* forward reference */
		addr = address->offset;
		instr->patch_label = address->symbol;
	} else
		addr = address->symbol->info.linfo->address + address->offset;
	f3_instr->opcode = opcode;
	f3_instr->address = addr;
	f3_instr->source = src->symbol->info.rinfo->address
			 + src->offset;
	f3_instr->immediate = immed->value;

	if (is_download_const(immed))
		f3_instr->parity = 1;

	symlist_free(&immed->referenced_syms);
	instruction_ptr++;
}

static void
test_readable_symbol(symbol_t *symbol)
{
	
	if ((symbol->info.rinfo->modes & (0x1 << src_mode)) == 0) {
		snprintf(errbuf, sizeof(errbuf),
			"Register %s unavailable in source reg mode %d",
			symbol->name, src_mode);
		stop(errbuf, EX_DATAERR);
	}

	if (symbol->info.rinfo->mode == WO) {
		stop("Write Only register specified as source",
		     EX_DATAERR);
		/* NOTREACHED */
	}
}

static void
test_writable_symbol(symbol_t *symbol)
{
	
	if ((symbol->info.rinfo->modes & (0x1 << dst_mode)) == 0) {
		snprintf(errbuf, sizeof(errbuf),
			"Register %s unavailable in destination reg mode %d",
			symbol->name, dst_mode);
		stop(errbuf, EX_DATAERR);
	}

	if (symbol->info.rinfo->mode == RO) {
		stop("Read Only register specified as destination",
		     EX_DATAERR);
		/* NOTREACHED */
	}
}

static void
type_check(symbol_t *symbol, expression_t *expression, int opcode)
{
	symbol_node_t *node;
	int and_op;

	and_op = FALSE;
	if (opcode == AIC_OP_AND || opcode == AIC_OP_JNZ || AIC_OP_JZ)
		and_op = TRUE;

	/*
	 * Make sure that we aren't attempting to write something
	 * that hasn't been defined.  If this is an and operation,
	 * this is a mask, so "undefined" bits are okay.
	 */
	if (and_op == FALSE
	 && (expression->value & ~symbol->info.rinfo->valid_bitmask) != 0) {
		snprintf(errbuf, sizeof(errbuf),
			 "Invalid bit(s) 0x%x in immediate written to %s",
			 expression->value & ~symbol->info.rinfo->valid_bitmask,
			 symbol->name);
		stop(errbuf, EX_DATAERR);
		/* NOTREACHED */
	}

	/*
	 * Now make sure that all of the symbols referenced by the
	 * expression are defined for this register.
	 */
	if (symbol->info.rinfo->typecheck_masks != FALSE) {
		for(node = expression->referenced_syms.slh_first;
		    node != NULL;
		    node = node->links.sle_next) {
			if ((node->symbol->type == MASK
			  || node->symbol->type == FIELD
			  || node->symbol->type == ENUM
			  || node->symbol->type == ENUM_ENTRY)
			 && symlist_search(&node->symbol->info.finfo->symrefs,
					   symbol->name) == NULL) {
				snprintf(errbuf, sizeof(errbuf),
					 "Invalid field or mask %s "
					 "for register %s",
					 node->symbol->name, symbol->name);
				stop(errbuf, EX_DATAERR);
				/* NOTREACHED */
			}
		}
	}
}

static void
make_expression(expression_t *immed, int value)
{
	SLIST_INIT(&immed->referenced_syms);
	immed->value = value & 0xff;
}

static void
add_conditional(symbol_t *symbol)
{
	static int numfuncs;

	if (numfuncs == 0) {
		/* add a special conditional, "0" */
		symbol_t *false_func;

		false_func = symtable_get("0");
		if (false_func->type != UNINITIALIZED) {
			stop("Conditional expression '0' "
			     "conflicts with a symbol", EX_DATAERR);
			/* NOTREACHED */
		}
		false_func->type = CONDITIONAL;
		initialize_symbol(false_func);
		false_func->info.condinfo->func_num = numfuncs++;
		symlist_add(&patch_functions, false_func, SYMLIST_INSERT_HEAD);
	}

	/* This condition has occurred before */
	if (symbol->type == CONDITIONAL)
		return;

	if (symbol->type != UNINITIALIZED) {
		stop("Conditional expression conflicts with a symbol",
		     EX_DATAERR);
		/* NOTREACHED */
	}

	symbol->type = CONDITIONAL;
	initialize_symbol(symbol);
	symbol->info.condinfo->func_num = numfuncs++;
	symlist_add(&patch_functions, symbol, SYMLIST_INSERT_HEAD);
}

static void
add_version(const char *verstring)
{
	const char prefix[] = " * ";
	int newlen;
	int oldlen;

	newlen = strlen(verstring) + strlen(prefix);
	oldlen = 0;
	if (versions != NULL)
		oldlen = strlen(versions);
	versions = realloc(versions, newlen + oldlen + 2);
	if (versions == NULL)
		stop("Can't allocate version string", EX_SOFTWARE);
	strcpy(&versions[oldlen], prefix);
	strcpy(&versions[oldlen + strlen(prefix)], verstring);
	versions[newlen + oldlen] = '\n';
	versions[newlen + oldlen + 1] = '\0';
}

void
yyerror(const char *string)
{
	stop(string, EX_DATAERR);
}

static int
is_download_const(expression_t *immed)
{
	if ((immed->referenced_syms.slh_first != NULL)
	 && (immed->referenced_syms.slh_first->symbol->type == DOWNLOAD_CONST))
		return (TRUE);

	return (FALSE);
}
#line 1473 "aicasm_gram.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack()
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        return -1;
    yyss = newss;
    yyssp = newss + i;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        return -1;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#ifndef YYPARSE_PARAM
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG void
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif	/* ANSI-C/C++ */
#else	/* YYPARSE_PARAM */
#ifndef YYPARSE_PARAM_TYPE
#define YYPARSE_PARAM_TYPE void *
#endif
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG YYPARSE_PARAM_TYPE YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL YYPARSE_PARAM_TYPE YYPARSE_PARAM;
#endif	/* ANSI-C/C++ */
#endif	/* ! YYPARSE_PARAM */

int
yyparse (YYPARSE_PARAM_ARG)
    YYPARSE_PARAM_DECL
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate])) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(lint) || defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 33:
#line 260 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		include_file(yyvsp[-1].str, BRACKETED_INCLUDE);
	}
break;
case 34:
#line 264 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		include_file(yyvsp[-1].str, QUOTED_INCLUDE);
	}
break;
case 35:
#line 271 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (prefix != stock_prefix)
			stop("Prefix multiply defined",
			     EX_DATAERR);
		prefix = strdup(yyvsp[0].str);
		if (prefix == NULL)
			stop("Unable to record prefix", EX_SOFTWARE);
	}
break;
case 36:
#line 283 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (patch_arg_list != NULL)
			stop("Patch argument list multiply defined",
			     EX_DATAERR);
		patch_arg_list = strdup(yyvsp[0].str);
		if (patch_arg_list == NULL)
			stop("Unable to record patch arg list", EX_SOFTWARE);
	}
break;
case 37:
#line 295 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ add_version(yyvsp[0].str); }
break;
case 38:
#line 299 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ cur_symtype = REGISTER; }
break;
case 40:
#line 304 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			if (yyvsp[-1].sym->type != UNINITIALIZED) {
				stop("Register multiply defined", EX_DATAERR);
				/* NOTREACHED */
			}
			cur_symbol = yyvsp[-1].sym; 
			cur_symbol->type = cur_symtype;
			initialize_symbol(cur_symbol);
		}
break;
case 41:
#line 315 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{                    
			/*
			 * Default to allowing everything in for registers
			 * with no bit or mask definitions.
			 */
			if (cur_symbol->info.rinfo->valid_bitmask == 0)
				cur_symbol->info.rinfo->valid_bitmask = 0xFF;

			if (cur_symbol->info.rinfo->size == 0)
				cur_symbol->info.rinfo->size = 1;

			/*
			 * This might be useful for registers too.
			 */
			if (cur_symbol->type != REGISTER) {
				if (cur_symbol->info.rinfo->address == 0)
					cur_symbol->info.rinfo->address =
					    sram_or_scb_offset;
				sram_or_scb_offset +=
				    cur_symbol->info.rinfo->size;
			}
			cur_symbol = NULL;
		}
break;
case 58:
#line 364 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		cur_symbol->info.rinfo->address = yyvsp[0].value;
	}
break;
case 59:
#line 371 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		cur_symbol->info.rinfo->size = yyvsp[0].value;
		if (scb_or_sram_symbol != NULL) {
			u_int max_addr;
			u_int sym_max_addr;

			max_addr = scb_or_sram_symbol->info.rinfo->address
				 + scb_or_sram_symbol->info.rinfo->size;
			sym_max_addr = cur_symbol->info.rinfo->address
				     + cur_symbol->info.rinfo->size;

			if (sym_max_addr > max_addr)
				stop("SCB or SRAM space exhausted", EX_DATAERR);
		}
	}
break;
case 60:
#line 390 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		cur_symbol->info.rinfo->mode = yyvsp[0].value;
	}
break;
case 61:
#line 397 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		cur_symbol->info.rinfo->modes = yyvsp[0].value;
	}
break;
case 62:
#line 404 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.value = yyvsp[0].value;
	}
break;
case 63:
#line 408 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.value = yyvsp[-2].value | yyvsp[0].value;
	}
break;
case 64:
#line 415 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (yyvsp[0].value > 4) {
			stop("Valid register modes range between 0 and 4.",
			     EX_DATAERR);
			/* NOTREACHED */
		}

		yyval.value = (0x1 << yyvsp[0].value);
	}
break;
case 65:
#line 425 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		symbol_t *symbol;

		symbol = yyvsp[0].sym;
		if (symbol->type != CONST) {
			stop("Only \"const\" symbols allowed in "
			     "mode definitions.", EX_DATAERR);
			/* NOTREACHED */
		}
		if (symbol->info.cinfo->value > 4) {
			stop("Valid register modes range between 0 and 4.",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyval.value = (0x1 << symbol->info.cinfo->value);
	}
break;
case 66:
#line 445 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			field_symbol = NULL;
			enum_next_value = 0;
			enum_increment = 1;
		}
break;
case 68:
#line 452 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			process_field(FIELD, yyvsp[-1].sym, yyvsp[0].expression.value);
			field_symbol = yyvsp[-1].sym;
			enum_next_value = 0;
			enum_increment = 0x01 << (ffs(yyvsp[0].expression.value) - 1);
		}
break;
case 70:
#line 460 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		process_field(FIELD, yyvsp[-1].sym, yyvsp[0].expression.value);
	}
break;
case 71:
#line 467 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			field_symbol = NULL;
			enum_next_value = 0;
			enum_increment = 1;
		}
break;
case 73:
#line 474 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			process_field(ENUM, yyvsp[-1].sym, yyvsp[0].expression.value);
			field_symbol = yyvsp[-1].sym;
			enum_next_value = 0;
			enum_increment = 0x01 << (ffs(yyvsp[0].expression.value) - 1);
		}
break;
case 77:
#line 490 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		process_field(ENUM_ENTRY, yyvsp[0].sym, enum_next_value);
		enum_next_value += enum_increment;
	}
break;
case 78:
#line 495 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		process_field(ENUM_ENTRY, yyvsp[-1].sym, yyvsp[0].expression.value);
		enum_next_value = yyvsp[0].expression.value + enum_increment;
	}
break;
case 79:
#line 503 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		process_field(MASK, yyvsp[-1].sym, yyvsp[0].expression.value);
	}
break;
case 80:
#line 510 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (yyvsp[0].sym->type != UNINITIALIZED) {
			stop("Re-definition of register alias",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyvsp[0].sym->type = ALIAS;
		initialize_symbol(yyvsp[0].sym);
		yyvsp[0].sym->info.ainfo->parent = cur_symbol;
	}
break;
case 81:
#line 524 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (accumulator.symbol != NULL) {
			stop("Only one accumulator definition allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		accumulator.symbol = cur_symbol;
	}
break;
case 82:
#line 536 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (mode_ptr.symbol != NULL) {
			stop("Only one mode pointer definition allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		mode_ptr.symbol = cur_symbol;
	}
break;
case 83:
#line 548 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (allones.symbol != NULL) {
			stop("Only one definition of allones allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		allones.symbol = cur_symbol;
	}
break;
case 84:
#line 560 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (allzeros.symbol != NULL) {
			stop("Only one definition of allzeros allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		allzeros.symbol = cur_symbol;
	}
break;
case 85:
#line 572 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (none.symbol != NULL) {
			stop("Only one definition of none allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		none.symbol = cur_symbol;
	}
break;
case 86:
#line 584 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (sindex.symbol != NULL) {
			stop("Only one definition of sindex allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		sindex.symbol = cur_symbol;
	}
break;
case 87:
#line 596 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		 yyval.expression.value = yyvsp[-2].expression.value | yyvsp[0].expression.value;
		 symlist_merge(&yyval.expression.referenced_syms,
			       &yyvsp[-2].expression.referenced_syms,
			       &yyvsp[0].expression.referenced_syms);
	}
break;
case 88:
#line 603 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression.value = yyvsp[-2].expression.value & yyvsp[0].expression.value;
		symlist_merge(&yyval.expression.referenced_syms,
			       &yyvsp[-2].expression.referenced_syms,
			       &yyvsp[0].expression.referenced_syms);
	}
break;
case 89:
#line 610 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression.value = yyvsp[-2].expression.value + yyvsp[0].expression.value;
		symlist_merge(&yyval.expression.referenced_syms,
			       &yyvsp[-2].expression.referenced_syms,
			       &yyvsp[0].expression.referenced_syms);
	}
break;
case 90:
#line 617 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression.value = yyvsp[-2].expression.value - yyvsp[0].expression.value;
		symlist_merge(&(yyval.expression.referenced_syms),
			       &(yyvsp[-2].expression.referenced_syms),
			       &(yyvsp[0].expression.referenced_syms));
	}
break;
case 91:
#line 624 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression.value = yyvsp[-2].expression.value * yyvsp[0].expression.value;
		symlist_merge(&(yyval.expression.referenced_syms),
			       &(yyvsp[-2].expression.referenced_syms),
			       &(yyvsp[0].expression.referenced_syms));
	}
break;
case 92:
#line 631 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression.value = yyvsp[-2].expression.value / yyvsp[0].expression.value;
		symlist_merge(&(yyval.expression.referenced_syms),
			       &(yyvsp[-2].expression.referenced_syms),
			       &(yyvsp[0].expression.referenced_syms));
	}
break;
case 93:
#line 638 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression.value = yyvsp[-2].expression.value << yyvsp[0].expression.value;
		symlist_merge(&yyval.expression.referenced_syms,
			       &yyvsp[-2].expression.referenced_syms,
			       &yyvsp[0].expression.referenced_syms);
	}
break;
case 94:
#line 645 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression.value = yyvsp[-2].expression.value >> yyvsp[0].expression.value;
		symlist_merge(&yyval.expression.referenced_syms,
			       &yyvsp[-2].expression.referenced_syms,
			       &yyvsp[0].expression.referenced_syms);
	}
break;
case 95:
#line 652 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression = yyvsp[-1].expression;
	}
break;
case 96:
#line 656 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression = yyvsp[0].expression;
		yyval.expression.value = (~yyval.expression.value) & 0xFF;
	}
break;
case 97:
#line 661 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression = yyvsp[0].expression;
		yyval.expression.value = -yyval.expression.value;
	}
break;
case 98:
#line 666 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.expression.value = yyvsp[0].value;
		SLIST_INIT(&yyval.expression.referenced_syms);
	}
break;
case 99:
#line 671 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		symbol_t *symbol;

		symbol = yyvsp[0].sym;
		switch (symbol->type) {
		case ALIAS:
			symbol = yyvsp[0].sym->info.ainfo->parent;
		case REGISTER:
		case SCBLOC:
		case SRAMLOC:
			yyval.expression.value = symbol->info.rinfo->address;
			break;
		case MASK:
		case FIELD:
		case ENUM:
		case ENUM_ENTRY:
			yyval.expression.value = symbol->info.finfo->value;
			break;
		case DOWNLOAD_CONST:
		case CONST:
			yyval.expression.value = symbol->info.cinfo->value;
			break;
		case UNINITIALIZED:
		default:
		{
			snprintf(errbuf, sizeof(errbuf),
				 "Undefined symbol %s referenced",
				 symbol->name);
			stop(errbuf, EX_DATAERR);
			/* NOTREACHED */
			break;
		}
		}
		SLIST_INIT(&yyval.expression.referenced_syms);
		symlist_add(&yyval.expression.referenced_syms, symbol, SYMLIST_INSERT_HEAD);
	}
break;
case 100:
#line 711 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (yyvsp[-1].sym->type != UNINITIALIZED) {
			stop("Re-definition of symbol as a constant",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyvsp[-1].sym->type = CONST;
		initialize_symbol(yyvsp[-1].sym);
		yyvsp[-1].sym->info.cinfo->value = yyvsp[0].expression.value;
	}
break;
case 101:
#line 722 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (yyvsp[-2].value) {
			stop("Invalid downloaded constant declaration",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		if (yyvsp[-1].sym->type != UNINITIALIZED) {
			stop("Re-definition of symbol as a downloaded constant",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyvsp[-1].sym->type = DOWNLOAD_CONST;
		initialize_symbol(yyvsp[-1].sym);
		yyvsp[-1].sym->info.cinfo->value = download_constant_count++;
	}
break;
case 102:
#line 741 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (yyvsp[0].sym->type != UNINITIALIZED) {
			stop("Re-definition of symbol as a macro",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		cur_symbol = yyvsp[0].sym;
		cur_symbol->type = MACRO;
		initialize_symbol(cur_symbol);
	}
break;
case 103:
#line 755 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		add_macro_body(yyvsp[0].str);
	}
break;
case 104:
#line 759 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		add_macro_body(yyvsp[0].str);
		cur_symbol->info.macroinfo->narg = yyvsp[-2].value;
	}
break;
case 105:
#line 766 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		/* Macros can take no arguments */
		yyval.value = 0;
	}
break;
case 106:
#line 771 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.value = 1;
		add_macro_arg(yyvsp[0].str, 0);
	}
break;
case 107:
#line 776 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (yyvsp[-2].value == 0) {
			stop("Comma without preceding argument in arg list",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyval.value = yyvsp[-2].value + 1;
		add_macro_arg(yyvsp[0].str, yyvsp[-2].value);
	}
break;
case 108:
#line 789 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			snprintf(errbuf, sizeof(errbuf), "%s%d", SRAM_SYMNAME,
				 num_srams);
			cur_symbol = symtable_get(SRAM_SYMNAME);
			cur_symtype = SRAMLOC;
			cur_symbol->type = SRAMLOC;
			initialize_symbol(cur_symbol);
		}
break;
case 109:
#line 798 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			sram_or_scb_offset = cur_symbol->info.rinfo->address;
		}
break;
case 110:
#line 802 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			scb_or_sram_symbol = cur_symbol;
		}
break;
case 111:
#line 807 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			cur_symbol = NULL;
			scb_or_sram_symbol = NULL;
		}
break;
case 112:
#line 815 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			cur_symbol = symtable_get(SCB_SYMNAME);
			cur_symtype = SCBLOC;
			if (cur_symbol->type != UNINITIALIZED) {
				stop("Only one SRAM definition allowed",
				     EX_SOFTWARE);
				/* NOTREACHED */
			}
			cur_symbol->type = SCBLOC;
			initialize_symbol(cur_symbol);
			/* 64 bytes of SCB space */
			cur_symbol->info.rinfo->size = 64;
		}
break;
case 113:
#line 829 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			sram_or_scb_offset = cur_symbol->info.rinfo->address;
		}
break;
case 114:
#line 833 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			scb_or_sram_symbol = cur_symbol;
		}
break;
case 115:
#line 838 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
			cur_symbol = NULL;
			scb_or_sram_symbol = NULL;
		}
break;
case 122:
#line 858 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		process_register(&yyvsp[0].sym);
		yyval.sym_ref.symbol = yyvsp[0].sym;
		yyval.sym_ref.offset = 0;
	}
break;
case 123:
#line 864 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		process_register(&yyvsp[-3].sym);
		if (yyvsp[-1].sym->type != CONST) {
			stop("register offset must be a constant", EX_DATAERR);
			/* NOTREACHED */
		}
		if ((yyvsp[-1].sym->info.cinfo->value + 1) > yyvsp[-3].sym->info.rinfo->size) {
			stop("Accessing offset beyond range of register",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyval.sym_ref.symbol = yyvsp[-3].sym;
		yyval.sym_ref.offset = yyvsp[-1].sym->info.cinfo->value;
	}
break;
case 124:
#line 879 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		process_register(&yyvsp[-3].sym);
		if ((yyvsp[-1].value + 1) > yyvsp[-3].sym->info.rinfo->size) {
			stop("Accessing offset beyond range of register",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyval.sym_ref.symbol = yyvsp[-3].sym;
		yyval.sym_ref.offset = yyvsp[-1].value;
	}
break;
case 125:
#line 890 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (accumulator.symbol == NULL) {
			stop("No accumulator has been defined", EX_DATAERR);
			/* NOTREACHED */
		}
		yyval.sym_ref.symbol = accumulator.symbol;
		yyval.sym_ref.offset = 0;
	}
break;
case 126:
#line 902 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		test_writable_symbol(yyvsp[0].sym_ref.symbol);
		yyval.sym_ref = yyvsp[0].sym_ref;
	}
break;
case 127:
#line 910 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.expression = yyvsp[0].expression; }
break;
case 128:
#line 915 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (yyvsp[0].expression.value == 0 && is_download_const(&yyvsp[0].expression) == 0) {
			snprintf(errbuf, sizeof(errbuf),
				 "\nExpression evaluates to 0 and thus "
				 "references the accumulator.\n "
				 "If this is the desired effect, use 'A' "
				 "instead.\n");
			stop(errbuf, EX_DATAERR);
		}
		yyval.expression = yyvsp[0].expression;
	}
break;
case 129:
#line 927 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		SLIST_INIT(&yyval.expression.referenced_syms);
		symlist_add(&yyval.expression.referenced_syms, accumulator.symbol,
			    SYMLIST_INSERT_HEAD);
		yyval.expression.value = 0;
	}
break;
case 130:
#line 937 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		test_readable_symbol(yyvsp[0].sym_ref.symbol);
		yyval.sym_ref = yyvsp[0].sym_ref;
	}
break;
case 131:
#line 944 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.sym_ref.symbol = NULL;
		yyval.sym_ref.offset = 0;
	}
break;
case 132:
#line 949 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.sym_ref = yyvsp[0].sym_ref; }
break;
case 133:
#line 953 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = 0; }
break;
case 134:
#line 955 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = 1; }
break;
case 135:
#line 960 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		src_mode = yyvsp[-1].value;
	}
break;
case 136:
#line 967 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		dst_mode = yyvsp[-1].value;
	}
break;
case 137:
#line 974 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		critical_section_t *cs;

		if (in_critical_section != FALSE) {
			stop("Critical Section within Critical Section",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		cs = cs_alloc();
		cs->begin_addr = instruction_ptr;
		in_critical_section = TRUE;
	}
break;
case 138:
#line 990 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		critical_section_t *cs;

		if (in_critical_section == FALSE) {
			stop("Unballanced 'end_cs'", EX_DATAERR);
			/* NOTREACHED */
		}
		cs = TAILQ_LAST(&cs_tailq, cs_tailq);
		cs->end_addr = instruction_ptr;
		in_critical_section = FALSE;
	}
break;
case 139:
#line 1004 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = 0; }
break;
case 140:
#line 1006 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = 1; }
break;
case 141:
#line 1011 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (yyvsp[-1].sym->type != UNINITIALIZED) {
			stop("Program label multiply defined", EX_DATAERR);
			/* NOTREACHED */
		}
		yyvsp[-1].sym->type = LABEL;
		initialize_symbol(yyvsp[-1].sym);
		yyvsp[-1].sym->info.linfo->address = instruction_ptr;
		yyvsp[-1].sym->info.linfo->exported = yyvsp[-2].value;
	}
break;
case 142:
#line 1025 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.sym_ref.symbol = yyvsp[0].sym;
		yyval.sym_ref.offset = 0;
	}
break;
case 143:
#line 1030 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.sym_ref.symbol = yyvsp[-2].sym;
		yyval.sym_ref.offset = yyvsp[0].value;
	}
break;
case 144:
#line 1035 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.sym_ref.symbol = yyvsp[-2].sym;
		yyval.sym_ref.offset = -yyvsp[0].value;
	}
break;
case 145:
#line 1040 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.sym_ref.symbol = NULL;
		yyval.sym_ref.offset = 0;
	}
break;
case 146:
#line 1045 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.sym_ref.symbol = NULL;
		yyval.sym_ref.offset = yyvsp[0].value;
	}
break;
case 147:
#line 1050 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		yyval.sym_ref.symbol = NULL;
		yyval.sym_ref.offset = -yyvsp[0].value;
	}
break;
case 148:
#line 1058 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		scope_t *new_scope;

		add_conditional(yyvsp[-1].sym);
		new_scope = scope_alloc();
		new_scope->type = SCOPE_IF;
		new_scope->begin_addr = instruction_ptr;
		new_scope->func_num = yyvsp[-1].sym->info.condinfo->func_num;
	}
break;
case 149:
#line 1068 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		scope_t *new_scope;
		scope_t *scope_context;
		scope_t *last_scope;

		/*
		 * Ensure that the previous scope is either an
		 * if or and else if.
		 */
		scope_context = SLIST_FIRST(&scope_stack);
		last_scope = TAILQ_LAST(&scope_context->inner_scope,
					scope_tailq);
		if (last_scope == NULL
		 || last_scope->type == T_ELSE) {

			stop("'else if' without leading 'if'", EX_DATAERR);
			/* NOTREACHED */
		}
		add_conditional(yyvsp[-1].sym);
		new_scope = scope_alloc();
		new_scope->type = SCOPE_ELSE_IF;
		new_scope->begin_addr = instruction_ptr;
		new_scope->func_num = yyvsp[-1].sym->info.condinfo->func_num;
	}
break;
case 150:
#line 1093 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		scope_t *new_scope;
		scope_t *scope_context;
		scope_t *last_scope;

		/*
		 * Ensure that the previous scope is either an
		 * if or and else if.
		 */
		scope_context = SLIST_FIRST(&scope_stack);
		last_scope = TAILQ_LAST(&scope_context->inner_scope,
					scope_tailq);
		if (last_scope == NULL
		 || last_scope->type == SCOPE_ELSE) {

			stop("'else' without leading 'if'", EX_DATAERR);
			/* NOTREACHED */
		}
		new_scope = scope_alloc();
		new_scope->type = SCOPE_ELSE;
		new_scope->begin_addr = instruction_ptr;
	}
break;
case 151:
#line 1119 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		scope_t *scope_context;

		scope_context = SLIST_FIRST(&scope_stack);
		if (scope_context->type == SCOPE_ROOT) {
			stop("Unexpected '}' encountered", EX_DATAERR);
			/* NOTREACHED */
		}

		scope_context->end_addr = instruction_ptr;

		/* Pop the scope */
		SLIST_REMOVE_HEAD(&scope_stack, scope_stack_links);

		process_scope(scope_context);

		if (SLIST_FIRST(&scope_stack) == NULL) {
			stop("Unexpected '}' encountered", EX_DATAERR);
			/* NOTREACHED */
		}
	}
break;
case 152:
#line 1143 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_AND; }
break;
case 153:
#line 1144 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_XOR; }
break;
case 154:
#line 1145 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_ADD; }
break;
case 155:
#line 1146 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_ADC; }
break;
case 156:
#line 1151 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		format_1_instr(yyvsp[-6].value, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	}
break;
case 157:
#line 1158 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		format_1_instr(AIC_OP_OR, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	}
break;
case 158:
#line 1165 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, 1);
		format_1_instr(AIC_OP_ADD, &yyvsp[-3].sym_ref, &immed, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	}
break;
case 159:
#line 1175 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, -1);
		format_1_instr(AIC_OP_ADD, &yyvsp[-3].sym_ref, &immed, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	}
break;
case 160:
#line 1185 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, -1);
		format_1_instr(AIC_OP_ADD, &none, &immed, &allzeros, yyvsp[-1].value);
	}
break;
case 161:
#line 1192 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		format_1_instr(AIC_OP_ADD, &yyvsp[-4].sym_ref, &yyvsp[-2].expression, &allzeros, yyvsp[-1].value);
	}
break;
case 162:
#line 1199 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, 1);
		format_1_instr(AIC_OP_ADD, &none, &immed, &allones, yyvsp[-1].value);
	}
break;
case 163:
#line 1206 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, 1);
		format_1_instr(AIC_OP_ADD, &yyvsp[-2].sym_ref, &immed, &allones, yyvsp[-1].value);
	}
break;
case 164:
#line 1216 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		format_1_instr(AIC_OP_BMOV, &yyvsp[-6].sym_ref, &yyvsp[-2].expression, &yyvsp[-4].sym_ref, yyvsp[-1].value);
	}
break;
case 165:
#line 1223 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, 1);
		format_1_instr(AIC_OP_BMOV, &yyvsp[-4].sym_ref, &immed, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	}
break;
case 166:
#line 1233 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		if (yyvsp[-2].expression.value == 0
		 && is_download_const(&yyvsp[-2].expression) == 0) {
			expression_t immed;

			/*
			 * Allow move immediates of 0 so that macros,
			 * that can't know the immediate's value and
			 * otherwise compensate, still work.
			 */
			make_expression(&immed, 1);
			format_1_instr(AIC_OP_BMOV, &yyvsp[-4].sym_ref, &immed, &allzeros, yyvsp[-1].value);
		} else {
			format_1_instr(AIC_OP_OR, &yyvsp[-4].sym_ref, &yyvsp[-2].expression, &allzeros, yyvsp[-1].value);
		}
	}
break;
case 167:
#line 1253 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, 0xff);
		format_1_instr(AIC_OP_XOR, &yyvsp[-3].sym_ref, &immed, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	}
break;
case 168:
#line 1263 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, 0xff);
		format_1_instr(AIC_OP_AND, &yyvsp[-2].sym_ref, &immed, &allzeros, yyvsp[-1].value);
	}
break;
case 169:
#line 1273 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, 0xff);
		format_1_instr(AIC_OP_AND, &none, &immed, &allzeros, yyvsp[-1].value);
	}
break;
case 170:
#line 1283 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, 0xff);
		format_1_instr(AIC_OP_AND, &none, &immed, &allzeros, TRUE);
	}
break;
case 171:
#line 1301 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_SHL; }
break;
case 172:
#line 1302 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_SHR; }
break;
case 173:
#line 1303 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_ROL; }
break;
case 174:
#line 1304 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_ROR; }
break;
case 175:
#line 1309 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		format_2_instr(yyvsp[-6].value, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	}
break;
case 176:
#line 1315 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_JMP; }
break;
case 177:
#line 1316 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_JC; }
break;
case 178:
#line 1317 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_JNC; }
break;
case 179:
#line 1318 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_CALL; }
break;
case 180:
#line 1322 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_JZ; }
break;
case 181:
#line 1323 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_JNZ; }
break;
case 182:
#line 1327 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_JE; }
break;
case 183:
#line 1328 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{ yyval.value = AIC_OP_JNE; }
break;
case 184:
#line 1333 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, 0);
		format_3_instr(yyvsp[-2].value, &sindex, &immed, &yyvsp[-1].sym_ref);
	}
break;
case 185:
#line 1343 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		format_3_instr(yyvsp[-2].value, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-1].sym_ref);
	}
break;
case 186:
#line 1350 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		format_3_instr(yyvsp[-2].value, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-1].sym_ref);
	}
break;
case 187:
#line 1357 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		format_3_instr(yyvsp[-2].value, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-1].sym_ref);
	}
break;
case 188:
#line 1364 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		expression_t immed;

		make_expression(&immed, 0);
		format_3_instr(yyvsp[-2].value, &yyvsp[-3].sym_ref, &immed, &yyvsp[-1].sym_ref);
	}
break;
case 189:
#line 1374 "../../../dev/aic7xxx/aicasm/aicasm_gram.y"
{
		format_3_instr(yyvsp[-2].value, &allzeros, &yyvsp[-3].expression, &yyvsp[-1].sym_ref);
	}
break;
#line 2798 "aicasm_gram.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}

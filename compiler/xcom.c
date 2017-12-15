/*
**	XPL to C source language translator main program.
**
**	Author: Daniel Weaver
**
**	The parser and much of the scanner was based on work done by
**	McKeeman, Horning and Wortman in their XPL compiler called xcom.
**	Their xcom compiler was released into the public domain through the
**	Share orginization.
**
**	Additional inspiration on the scanner from Dave Bodenstab.
*/
#define DEBUG_HASHER 0

   /*  First we initialize the global constants that depend upon the input
      grammar.  The following cards are punched by analyzer.         */

#define NSY 96
#define NT 45
   char *v[NSY + 1] = { "<error: token = 0>", ";", ")", "(", ",", ":", "=", "|",
      "&", "~", "<", ">", "+", "-", "*", "/", "if", "do", "to", "by", "go",
      "||", "_|_", "end", "bit", "xor", "mod", "then", "else", "case", "call",
      "goto", "while", "fixed", "label", "return", "declare", "initial",
      "external", "<string>", "<number>", "procedure", "literally", "character",
      "transparent", "<identifier>", "<type>", "<term>", "<group>", "<go to>",
      "<ending>", "<program>", "<replace>", "<primary>", "<variable>",
      "<bit head>", "<relation>", "<constant>", "<statement>", "<if clause>",
      "<true part>", "<left part>", "<assignment>", "<expression>",
      "<group head>", "<bound head>", "<if statement>", "<while clause>",
      "<initial list>", "<initial head>", "<case selector>", "<statement list>",
      "<call statement>", "<procedure head>", "<procedure name>",
      "<parameter list>", "<parameter head>", "<character head>",
      "<logical factor>", "<subscript head>", "<basic statement>",
      "<go to statement>", "<step definition>", "<identifier list>",
      "<logical primary>", "<return statement>", "<label definition>",
      "<type declaration>", "<iteration control>", "<logical secondary>",
      "<string expression>", "<procedure statement>", "<declaration element>",
      "<procedure definition>", "<declaration statement>",
      "<arithmetic expression>", "<identifier specification>"};
   unsigned char v_index[13] = { 1, 16, 22, 27, 32, 35, 36, 38, 41, 44, 44, 45,
      46};
   char *c1[NSY + 1] = {
      "\000\000\000\000\000\000\000\000\000\000\000\000",
      "\040\000\000\000\240\212\000\212\002\200\000\040",
      "\052\212\252\252\012\040\352\000\074\050\003\200",
      "\003\000\060\360\000\000\000\000\000\003\300\060",
      "\003\000\040\240\000\000\000\000\000\002\200\060",
      "\040\000\000\000\240\202\000\012\002\200\040\040",
      "\002\000\040\240\000\000\000\000\000\002\200\040",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\001\004\005\120\000\000\000\000\000\001\100\020",
      "\002\004\000\240\000\000\000\000\000\002\200\040",
      "\002\004\000\240\000\000\000\000\000\002\200\040",
      "\001\000\000\000\000\000\000\000\000\001\100\020",
      "\001\000\000\000\000\000\000\000\000\001\100\020",
      "\001\000\000\000\000\000\000\000\000\001\100\020",
      "\001\000\000\000\000\000\000\000\000\001\100\020",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\020\000\000\000\000\000\000\020\100\000\000\020",
      "\001\000\020\120\000\000\000\000\000\001\100\060",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\000\000\000\000\004\000\000\000\000\000\000\000",
      "\001\000\000\120\000\000\000\000\000\001\100\020",
      "\020\000\000\000\120\100\000\005\001\100\000\020",
      "\040\000\000\000\000\000\000\000\000\000\000\020",
      "\001\000\000\000\000\000\000\000\000\000\000\000",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\001\000\000\000\000\000\000\000\000\001\100\020",
      "\040\000\000\000\240\200\000\012\002\200\000\040",
      "\040\000\000\000\240\200\000\012\002\200\000\040",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\000\000\000\000\000\000\000\000\000\000\000\020",
      "\000\000\000\000\000\000\000\000\000\000\000\040",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\040\200\000\000\000\000\000\000\000\050\000\200",
      "\040\200\000\000\000\000\000\000\000\050\000\200",
      "\041\000\020\120\000\000\000\000\000\001\100\020",
      "\001\000\000\000\000\000\000\000\000\000\000\020",
      "\001\000\000\000\000\000\000\000\000\000\000\000",
      "\040\000\000\000\240\200\000\012\002\200\000\040",
      "\050\212\252\252\012\040\052\000\000\000\000\000",
      "\050\212\252\252\012\040\052\000\000\000\000\000",
      "\042\000\000\000\000\000\200\000\050\010\002\200",
      "\000\000\000\000\000\000\000\000\000\001\000\000",
      "\041\200\000\000\000\000\000\000\000\050\000\200",
      "\040\000\000\000\240\200\000\012\002\200\000\040",
      "\057\332\252\252\012\040\252\000\050\000\006\000",
      "\040\200\000\000\000\000\000\000\000\050\000\200",
      "\050\212\252\245\012\040\046\000\000\000\000\000",
      "\020\000\000\000\000\000\000\000\000\000\000\000",
      "\000\000\000\000\000\000\000\000\000\000\000\020",
      "\040\000\000\000\000\000\000\000\000\000\000\000",
      "\000\000\000\000\000\010\000\000\000\000\000\000",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\050\212\252\252\012\040\052\000\000\000\000\000",
      "\050\316\252\252\012\040\052\000\000\000\000\000",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\001\000\000\120\000\000\000\000\000\001\100\020",
      "\050\212\252\252\012\040\052\000\000\000\000\000",
      "\040\000\000\000\240\212\000\012\002\200\000\040",
      "\020\000\000\000\120\100\000\005\001\100\000\020",
      "\020\000\000\000\120\100\000\005\001\100\000\020",
      "\000\000\000\000\000\000\000\000\000\000\000\020",
      "\060\000\000\000\000\000\000\000\000\000\000\000",
      "\044\101\000\000\005\000\021\000\000\000\000\000",
      "\020\000\000\000\120\101\000\005\001\100\000\020",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\040\000\000\000\240\212\000\012\002\200\000\040",
      "\020\000\000\000\000\000\000\000\000\000\000\000",
      "\040\200\000\000\000\000\000\000\000\040\000\000",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\020\000\000\000\000\000\000\000\000\000\000\000",
      "\020\000\000\000\120\111\000\005\001\100\000\020",
      "\020\000\000\000\000\000\000\000\000\000\000\000",
      "\020\000\000\000\000\000\000\000\000\004\000\100",
      "\041\000\000\000\000\000\100\000\024\010\001\200",
      "\040\000\000\000\000\000\100\000\024\010\001\200",
      "\000\000\000\000\000\000\000\000\000\000\000\020",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\050\202\100\000\012\000\042\000\000\000\000\000",
      "\001\000\020\120\000\000\000\000\000\001\100\020",
      "\040\000\000\000\240\212\000\312\002\200\000\040",
      "\020\000\000\000\000\000\000\000\000\000\000\000",
      "\020\000\000\000\000\000\000\000\000\000\000\000",
      "\000\000\000\000\000\000\000\000\000\000\000\020",
      "\050\202\200\000\012\000\042\000\000\000\000\000",
      "\020\000\000\000\000\000\000\000\000\000\000\000",
      "\020\000\000\000\120\101\000\005\001\100\020\020",
      "\040\200\000\000\000\000\000\000\000\020\000\000",
      "\040\000\000\000\000\000\000\000\000\000\000\000",
      "\050\202\200\000\012\000\042\000\000\000\000\000",
      "\050\206\225\000\012\020\042\000\000\000\000\000",
      "\020\000\000\000\120\100\000\005\001\100\000\020",
      "\040\200\000\000\000\000\000\000\000\000\000\000",
      "\020\000\000\000\000\000\000\000\000\000\000\000",
      "\020\100\000\000\000\000\000\000\000\000\000\000",
      "\050\212\252\120\012\040\042\000\000\000\000\000",
      "\001\000\000\000\000\000\100\000\024\000\001\000"};
#define NC1TRIPLES 237
   int c1triples[NC1TRIPLES + 1] = { 197379, 197385, 197388, 197389, 197415,
      197416, 197421, 208131, 459523, 459529, 459532, 459533, 459559, 459560,
      459565, 470275, 525059, 525065, 525068, 525069, 525095, 525096, 525101,
      535811, 590595, 590601, 590604, 590605, 590631, 590632, 590637, 601347,
      787203, 787209, 787212, 787213, 787239, 787240, 787245, 797955, 852739,
      852745, 852748, 852749, 852775, 852776, 852781, 863491, 918275, 918281,
      918284, 918285, 918311, 918312, 918317, 929027, 983811, 983817, 983820,
      983821, 983847, 983848, 983853, 994563, 1049347, 1049353, 1049356,
      1049357, 1049383, 1049384, 1049389, 1060099, 1125635, 1127942, 1180419,
      1180425, 1180428, 1180429, 1180455, 1180456, 1180461, 1191171, 1245955,
      1245961, 1245964, 1245965, 1245991, 1245992, 1245997, 1256707, 1377027,
      1377033, 1377036, 1377037, 1377063, 1377064, 1377069, 1387779, 1453315,
      1455620, 1455622, 1457665, 1639171, 1639177, 1639180, 1639181, 1639207,
      1639208, 1639213, 1649923, 1704707, 1704713, 1704716, 1704717, 1704743,
      1704744, 1704749, 1715459, 1901315, 1901321, 1901324, 1901325, 1901351,
      1901352, 1901357, 1912067, 1977603, 2097923, 2097929, 2097932, 2097933,
      2097959, 2097960, 2097965, 2108675, 2294531, 2294537, 2294540, 2294541,
      2294567, 2294568, 2294573, 2305283, 3408643, 3408649, 3408652, 3408653,
      3408679, 3408680, 3408685, 3419395, 3605251, 3605257, 3605260, 3605261,
      3605287, 3605288, 3605293, 3616003, 3670787, 3670793, 3670796, 3670797,
      3670823, 3670824, 3670829, 3681539, 3878147, 3880452, 3880454, 3882497,
      3887132, 3943683, 3945988, 3945990, 3948033, 4009219, 4011524, 4011526,
      4129304, 4129313, 4129314, 4129323, 4133421, 4205827, 4208132, 4208134,
      4210177, 4260611, 4260617, 4260620, 4260621, 4260647, 4260648, 4260653,
      4271363, 4522755, 4522761, 4522764, 4522765, 4522791, 4522792, 4522797,
      4533507, 4664579, 4666884, 4666886, 4668929, 4992258, 4992260, 5047043,
      5047049, 5047052, 5047053, 5047079, 5047080, 5047085, 5057795, 5178115,
      5178121, 5178124, 5178125, 5178151, 5178152, 5178157, 5188867, 5451010,
      5451012, 5647619, 5649924, 5649926, 5651969, 5975299, 5977604, 5977606,
      5979649, 6161411, 6161453};
   int prtb[116] = {0, 4434, 4419, 4422, 73, 17, 62, 48, 93, 85, 72, 81, 94, 0,
      17727, 19501, 21293, 14143, 19775, 831, 20287, 37, 96, 24, 43, 45, 0, 0,
      19501, 21293, 17727, 20287, 54, 45, 9, 10, 11, 0, 0, 9, 0, 9, 0, 20, 0,
      4159, 80, 0, 0, 0, 0, 73, 11562, 0, 0, 86, 0, 73, 23, 49, 0, 0, 4275970,
      19019, 96, 74, 24332, 24333, 12, 13, 0, 23367, 86, 64, 12046, 12047,
      12058, 0, 30, 0, 0, 15164, 71, 64, 59, 0, 61, 1195795, 13876, 18, 32, 29,
      35, 86, 0, 87, 0, 0, 74, 16135, 16153, 0, 86, 0, 9, 0, 0, 3552319, 19976,
      0, 23096, 0, 24068, 36, 23061, 0};
   unsigned char prdtb[116] = {0, 22, 23, 24, 32, 21, 6, 7, 8, 9, 10, 11, 12,
      13, 72, 40, 69, 63, 64, 109, 111, 73, 67, 65, 66, 112, 41, 70, 42, 71, 74,
      113, 78, 46, 91, 94, 95, 88, 77, 92, 89, 93, 90, 51, 43, 18, 19, 52, 60,
      62, 47, 33, 56, 114, 115, 39, 61, 34, 44, 50, 68, 110, 58, 38, 57, 36, 99,
      100, 101, 102, 98, 31, 45, 20, 104, 105, 106, 103, 49, 108, 107, 16, 3,
      25, 15, 2, 76, 28, 75, 27, 29, 30, 48, 17, 5, 59, 1, 35, 37, 80, 81, 79,
      14, 4, 85, 84, 55, 26, 83, 82, 87, 86, 54, 53, 97, 96};
   unsigned char hdtb[116] = {0, 64, 64, 64, 91, 64, 80, 80, 80, 80, 80, 80, 80,
      80, 68, 75, 96, 46, 46, 53, 54, 69, 65, 55, 77, 79, 76, 83, 76, 83, 69,
      79, 61, 86, 56, 56, 56, 56, 52, 56, 56, 56, 56, 49, 50, 59, 60, 49, 46,
      46, 85, 91, 92, 57, 57, 74, 46, 91, 50, 81, 96, 54, 87, 73, 87, 73, 95,
      95, 95, 95, 95, 93, 50, 48, 47, 47, 47, 47, 72, 53, 53, 66, 71, 64, 66,
      71, 62, 88, 62, 88, 67, 70, 85, 66, 58, 87, 51, 73, 73, 63, 63, 63, 80,
      58, 89, 89, 92, 82, 78, 78, 84, 84, 94, 94, 90, 90};
   unsigned char prlength[116] = {0, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3,
      3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 2,
      1, 2, 1, 2, 1, 3, 2, 1, 1, 1, 1, 2, 3, 1, 1, 2, 1, 2, 2, 2, 1, 1, 4, 3, 2,
      2, 3, 3, 2, 2, 1, 3, 2, 2, 3, 3, 3, 1, 2, 1, 1, 3, 2, 2, 2, 1, 2, 4, 3, 2,
      2, 2, 2, 2, 1, 2, 1, 1, 2, 3, 3, 1, 2, 1, 2, 1, 1, 4, 3, 1, 3, 1, 3, 2, 3,
      1};
   unsigned char context_case[116] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0};
   unsigned char left_context[2] = { 90, 74};
   unsigned char left_index[ 52] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
   int context_triple[1] = { 0};
   unsigned char triple_index[ 52] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
   unsigned char pr_index[97] = { 1, 14, 21, 28, 33, 34, 39, 39, 39, 39, 41, 43,
      43, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 46, 47, 47,
      47, 48, 48, 49, 50, 51, 51, 51, 52, 54, 55, 56, 56, 57, 58, 62, 66, 71,
      71, 71, 74, 74, 74, 78, 80, 80, 80, 81, 86, 86, 86, 86, 87, 93, 93, 93,
      95, 95, 96, 96, 96, 97, 97, 97, 98, 99, 99, 99, 102, 102, 104, 104, 104,
      104, 106, 106, 106, 107, 108, 110, 112, 112, 114, 114, 114, 116, 116};
#define if_token 16
#define do_token 17
#define to_token 18
#define by_token 19
#define go_token 20
#define end_token 23
#define bit_token 24
#define xor_token 25
#define mod_token 26
#define then_token 27
#define else_token 28
#define case_token 29
#define call_token 30
#define goto_token 31
#define while_token 32
#define fixed_token 33
#define label_token 34
#define return_token 35
#define declare_token 36
#define initial_token 37
#define external_token 38
#define procedure_token 41
#define literally_token 42
#define character_token 43
#define transparent_token 44
#define reserved_limit 11

   /*  End of cards punched by analyzer  */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "xpl.h"

#define TRUE 1
#define FALSE 0

#define STR(s) {(int) (sizeof(s) - 1), s}
#define STR_NULL {0,0}
#define SHL(v, b) ((v) << (b))
#define SHR(v, b) ((v) >> (b))
#define CAT __xpl_cat
#define CAT_INT __xpl_cat_int
#define SUBSTR2 __xpl_substr2
#define SUBSTR3 __xpl_substr3
#define INPUT __xpl_input
#define OUTPUT __xpl_output
#define COMPARE __xpl_compare
#define BYTE __xpl_get_byte
#define STRING __xpl_string
#define DECIMAL __xpl_decimal
#define MOVE_TO_TOP __xpl_move_to_top

#define blank_line OUTPUT(0, &null)
#define double_space { blank_line; blank_line; }
#define eject_page blank_line

/*
    All character string arrays used by the compiler are allocated here.

    The array size is defined using the C convention which is the exact number
    of elements requested rather than the XPL convention of the number of elements
    minus one.  The lowercase 'c' in the front of the macro name is used to designate
    the difference.
*/
#define cSYTSIZE	1000
#define cSTACKSIZE	75
#define cMAPPER_LIMIT	256
#define cGLOBAL_LIMIT	256
#define cSTRING_LIMIT	128
#define cDIVERT_SIZE	512

/* The C preprocessor has no easy way to allocate space. */
#define SYTSIZE_BASE		100
#define STACKSIZE_BASE		SYTSIZE_BASE+(cSYTSIZE*2)
#define MAPPER_LIMIT_BASE	STACKSIZE_BASE+(cSTACKSIZE*2)
#define GLOBAL_LIMIT_BASE	MAPPER_LIMIT_BASE+(cMAPPER_LIMIT*2)
#define STRING_BASE		GLOBAL_LIMIT_BASE+cGLOBAL_LIMIT
#define DIVERT_SIZE_BASE	STRING_BASE+cSTRING_LIMIT

/* Allocate more arrays above this line. */
/* LAST_BASE must be last and include all the string descriptors used by the compiler */
#define LAST_BASE		DIVERT_SIZE_BASE+cDIVERT_SIZE

int ndescript;
STRING descriptor[LAST_BASE];

#define null descriptor[64]

/* I/O declarations */
char *input_filename;		/* Input filename.  Usually ends in .xpl */
char output_filename[256];	/* C source output filename */
char string_filename[256];	/* String include file for the C source */
int input_unit;			/* XPL input stream unit number */
FILE *out_file;			/* Output file stream pointer (filename.c)*/
FILE *string_file;		/* String file stream pointer (filename.xh)*/
FILE *debug_file;		/* Debug file stream pointer (filename.xh)*/

/* Declarations for the scanner */
char chartype[256],	/* Used to distinguish classes of symbols in the scanner. */
	tx[256],	/* Holds token numbers for special characters. */
	control[256],	/* Holds the value of the compiler control toggles set in $ cards. */
	hex_digit[256],	/* Value of hex digit or 16 if illegal */
	escape_sequence[256],	/* C character string escape sequences */
	not_letter_or_digit[256];	/* Similiar to chartype[] but used in scanning identifiers only.  */

int next_ch;		/* Holds the next value of ch for C string escape sequences */

/* alphabet consists of the symbols considered alphabetic in building identifiers */
static char alphabet[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$@#"};

/* buffer holds the latest cardimage.
   text holds the present state of the input text (including macro
        expansions and not including the portions deleted by the scanner).
   current_procedure holds the name of the function being compiled.
   information holds additional data to be printed on the listing.
   bcd is the last symbol scanned (literal character string).
*/
#define buffer descriptor[65]
#define text descriptor[66]
#define current_procedure descriptor[67]
#define information descriptor[70]
#define bcd descriptor[71]

int text_limit,		/* A convenient place to store the pointer to the end of text. */
    text_used,		/* Count of characters already consumed on this cardimage */
    card_count,		/* Incremented by one for every XPL source card read. */
    error_count,	/* Tabulates the errors as they are detected during compile. */
    severe_errors,	/* Counts severe errors */
    previous_error,	/* Holds the line number of the last error */
    margin_chop;	/* Card position for sequence number */

XPL_LONG number_value;	/* Holds the numeric value of the last constant scanned. */
XPL_LONG scanner_value;	/* Holds a numeric constant while in the scanner */
XPL_LONG value_mask;	/* value_mask is  ~(2**jbase-1)   (i.e., ~(shl(1,jbase)-1) ) */
int jbase;		/* jbase contains the field width in bit strings (default value = 4) */
int charbase;		/* TRUE if scanning a C character string */
int sizeof_long;	/* Set to the sizeof(XPL_LONG) or 4 if control['G'] */

/* Each of the following contains the index into v() of the corresponding
	symbol.   We ask:    IF token = ident_token    etc.    */
int ident_token, string_token, number_token, divide_token, eof_token;
int or_token, concatenate_token;

int line_symbol;	/* Index into the symbol table for the __LINE__ macro */
int address_symbol;	/* Index into the symbol table for the address macro */
int right_conflict(int left);	/* Used to allow redefinition of the address macro */

/*
** Reserved words in the C language must be mapped when used in XPL.
** Feel free to add to this list.  Order is not important.
**
** The following words are omitted because they are also keywords in XPL:
**   case, do, return, while, else, goto, if
*/
static STRING mapper_list[] = {
	STR("auto"), STR("break"), STR("char"), STR("const"), STR("continue"),
	STR("default"), STR("double"), STR("enum"), STR("extern"),
	STR("float"), STR("for"), STR("int"), STR("long"), STR("register"),
	STR("short"), STR("signed"), STR("sizeof"), STR("static"),
	STR("struct"), STR("switch"), STR("typeof"), STR("union"),
	STR("unsigned"), STR("void"), STR("volatile"), STR("size_t"),
	STR("main"), STR("exit"), STR("abort"), STR("time"), STR("date"),
#if 0
	/* Reserved words from <stdio.h> */
	STR("rename"), STR("renameat"), STR("renamex_np"), STR("renameatx_np"),
	STR("RENAME_SWAP"), STR("RENAME_EXCL"), STR("fpos_t"), STR("FILE"),
	STR("clearerr"), STR("fclose"), STR("feof"), STR("ferror"),
	STR("fflush"), STR("fgetc"), STR("fgetpos"), STR("fgets"),
	STR("fopen"), STR("fprintf"), STR("fputc"), STR("fputs"),
	STR("fread"), STR("freopen"), STR("fscanf"), STR("fseek"),
	STR("fsetpos"), STR("ftell"), STR("fwrite"), STR("getc"),
	STR("getchar"), STR("gets"), STR("perror"), STR("printf"),
	STR("putc"), STR("putchar"), STR("puts"), STR("remove"),
	STR("rename"), STR("rewind"), STR("scanf"), STR("setbuf"),
	STR("setvbuf"), STR("sprintf"), STR("sscanf"), STR("tmpfile"),
	STR("tmpnam"), STR("ungetc"), STR("vfprintf"), STR("vprintf"),
	STR("vsprintf"), STR("ctermid"), STR("fdopen"), STR("fileno"),
	STR("pclose"), STR("popen"), STR("flockfile"), STR("ftrylockfile"),
	STR("funlockfile"), STR("getc_unlocked"), STR("getchar_unlocked"),
	STR("putc_unlocked"), STR("putchar_unlocked"), STR("getw"),
	STR("putw"), STR("tempnam"), STR("fseeko"), STR("ftello"),
	STR("snprintf"), STR("vfscanf"), STR("vscanf"), STR("vsnprintf"),
	STR("vsscanf"), STR("ssize_t"), STR("dprintf"), STR("vdprintf"),
	STR("getdelim"), STR("getline"), STR("sys_nerr"), STR("sys_errlist"),
	STR("asprintf"), STR("ctermid_r"), STR("fgetln"), STR("fmtcheck"),
	STR("fpurge"), STR("setbuffer"), STR("setlinebuf"), STR("vasprintf"),
	STR("zopen"), STR("funopen"), STR("fpos_t"), STR("BUFSIZ"),
	STR("EOF"), STR("FOPEN_MAX"), STR("FILENAME_MAX"), STR("TMP_MAX"),
	STR("SEEK_SET"), STR("SEEK_CUR"), STR("SEEK_END"),
	STR("stdin"), STR("stdout"), STR("stderr"),
	/* End <stdio.h> */
#endif
	STR("index"), STR("errno"),
	{0, 0} /* Must be last */};

#define MAPPER_LIMIT (cMAPPER_LIMIT-1)

#define mapper_name(n) descriptor[(MAPPER_LIMIT_BASE) + n]
#define mapper_text(n) descriptor[(MAPPER_LIMIT_BASE+cMAPPER_LIMIT) + n]

short mapper_index[256];	/* Points into the mapper array.  Sorted by length. */
int top_mapper;			/* Number of mapper words defined minus one */
char poison[256];		/* Characters that may not be used in C identifiers */
unsigned char poison_list[] = {"$@#"};

/* Names that have been promoted to global scope are placed in this table. */
#define GLOBAL_LIMIT (cGLOBAL_LIMIT-1)
#define global_name(n) descriptor[(GLOBAL_LIMIT_BASE) + n]
short global_index[256];	/* Points into the global_name() table. */
int top_global;			/* Number of global identifiers minus one. */

/* The following are used in the macro expander.  */
#define EXPANSION_SIZE_LIMIT 4096
#define EXPANSION_COUNT_LIMIT 256
int expansion_count;		/* Number of macro expansions */

void ignore_case_macro(void);

static char hex_digits[] = {"0123456789abcdef"};

/* Stopit[] is a table of symbols which are allowed to terminate the error
      flush process.  In general they are symbols of sufficient syntactic
      hierarchy that we expect to avoid attempting to start compiling again
      right into another error producing situation.  The token stack is also
      flushed down to something acceptable to a stopit[] symbol.
      Failsoft is a bit which allows the compiler one attempt at a gentle
      recovery.   Then it takes a strong hand.   When there is real trouble
      compiling is set to FALSE, thereby terminating the compilation.
      reserved_limit is the length of the longest reserved word in v[]
   */
unsigned char stopit[NT + 1];
int failsoft;		/* Set to TRUE when failing */
int compiling;		/* Set to FALSE to abort the compilation */

/* The entries in prmask[] are used to select out portions of coded
      productions and the stack top for comparison in the analysis algorithm */
int prmask[6]= {0, 0, 0xff, 0xffff, 0xffffff, 0xffffffff};

/* Count the number of comparisons of identifiers in symbol table look-ups
   this can, in general, be expected to be a substantial part of compile time.
*/
int idcompares;

int count_stack, count_scan, count_force, count_emit,
      count_arith, count_store, count_desc;

/* statement_count counts the number of semicolons.  (sort of) */
int statement_count;

int cp;		/* Current index into the line being parsed */
int ch;		/* the last character scanned (hex code) */
int token;	/* the index into the vocabulary v() of the last symbol scanned */

static STRING x1 = STR(" ");
static STRING x2 = STR("  ");
static STRING x4 = STR("    ");
static STRING x70 = STR(
	"                                                                      ");
static STRING vbar = STR("|");
static STRING x1vbar = STR(" |");
static STRING star = STR("*");
static STRING colon = STR(":");
static STRING period = STR(".");
static STRING comma_x1 = STR(", ");
static STRING colon_x1 = STR(": ");
static STRING newline = STR("\n");
static STRING semicolon = STR(";");
static STRING semicolon_x1 = STR("; ");
static STRING single_quote = STR("'");
static STRING ampersand = STR("&");
static STRING open_paren = STR("(");
static STRING close_paren = STR(")");
static STRING close_close = STR("))");
static STRING open_brace = STR("{");
static STRING close_brace = STR("}");
static STRING left_bracket = STR("[");
static STRING right_bracket = STR("]");
static STRING while_tail = STR(") {");
static STRING index_zero = STR("[0]");
static STRING comma_zero = STR(", 0");
static STRING x1_equal_x1 = STR(" = ");
static STRING dot_length = STR("._Length");
static STRING point_length = STR("->_Length");
static STRING null_terminator = STR("\0");
static STRING close_comma_x1 = STR("), ");
static STRING bracket_comma_x1 = STR("], ");
static STRING bracket_semicolon = STR("];");
static STRING close_brace_semicolon = STR("};");
static STRING unimplemented = STR("Unimplemented feature");
static STRING oops = STR("Internal error");
static STRING overflow = STR("symbol table overflow");
static STRING abort_me = STR("*** compilation aborted  ****");
static STRING ill_bit_char = STR("Illegal character in bit string: ");
static STRING unused = STR("Unused variable: ");
static STRING truncation = STR("Data truncated for: ");

/* Table of compiler support functions that return a descriptor */
static STRING suproc[] = {
	STR("__xpl_cat(&descriptor["),
	STR("__xpl_input(&descriptor["),
	STR("__xpl_cat_int(&descriptor["),
	STR("__xpl_decimal(&descriptor["),
	STR("__xpl_substr2(&descriptor["),
	STR("__xpl_substr3(&descriptor["),
	STR("__xpl_wrapper(&descriptor["),
	STR("__xpl_move_to_top(&descriptor["),
	STR("__xpl_get_charfixed(&descriptor["),
	STR("__xpl_build_descriptor(&descriptor["),
	STR_NULL /* Must be last */
};

#define cat_descriptor suproc[0]
#define input_descriptor suproc[1]
#define cat_int_descriptor suproc[2]
#define decimal_descriptor suproc[3]
#define substr2_descriptor suproc[4]
#define substr3_descriptor suproc[5]
#define wrapper_descriptor suproc[6]
#define move_to_top_descriptor suproc[7]
#define get_charfixed_descriptor suproc[8]
#define build_descriptor_descriptor suproc[9]

/*                    Code emitters                             */

#define XPLRT_STRINGS	32
int dsp, newdsp;	/* Emitter pointers */
int argv_limit;		/* Max number of entries in the argv[] array */
int nest_block;		/* Count the number of braces {} */
int temp_variable_name; /* Numeric portion of temporary variable names */
int output_line;	/* Output line number */
int return_statement;	/* last return statement */
#define emit_delay descriptor[72]

#define DIVERT_LIMIT	16
#define DIVERT_LEVEL	(cDIVERT_SIZE/DIVERT_LIMIT)

#define divert(n) descriptor[(DIVERT_SIZE_BASE) + n]
int proc_nest;		/* Count procedure nesting */
int divert_code[DIVERT_LEVEL];	/* TRUE if code is being diverted */
int divert_index[DIVERT_LEVEL];	/* Next available slot in divert() */
int divert_nested[DIVERT_LEVEL];	/* TRUE is this is a nested procedure */

/* Character string initialization tables. */
#define INIT_STRING_MAX 100
short string_index[INIT_STRING_MAX];	/* Index into the descriptor[] array */
short string_number[INIT_STRING_MAX];	/* Variable name of string that holds the text */
int init_string;			/* Next available string in sring_index[] table */
int string_proc;			/* String procedure number */
char printing[256];			/* TRUE if this is a printing character */

#define TSP_MAX 64
int tsp;		/* Descriptor temp stack pointer */
short temp_stack[TSP_MAX];	/* Holds descriptor addresses */

/* String table cache */
#define STRING_LIMIT	(cSTRING_LIMIT-1)
#define string_cache(n) descriptor[(STRING_BASE) + n]
int string_cache_number[cSTRING_LIMIT];	/* variable number */
int string_cache_hits;	/* Number of cache hits */
int string_cache_max;	/* Number of strings in table */
int strack;		/* String replacement point */

/*                    Symbol table variables                    */

/* XPL does not have function prototypes and declaring a variable the type
LABEL does little to clarify the situation.  The LABEL declaration is used
for both function and targets of GOTO statements.  In order to clarify
some of this disorder I have come up with the following rules.
Declaring something a label will mark it as FORWARD_LABEL in the symbol
table unless it is already defined.  Then in that case the declaration
will be ignored.  FORWARD_LABELs may be promoted to FORWARD_CALL or
FORWARD_GOTO depending on how the label is referenced.  When the label
is defined in the code with a <label definition> it will become a
DEFINED_LABEL if it is not followed by a PROCEDURE definition.
If it's a procedure then it will become one of the procedure definitions
in the range of PROC_VOID to CHAR_PROC_TYPE depending on the return type.
A FORWARD_CALL may be called with arguments.  The arguments are
not checked for type match.  The type matching will be done by the C
compiler.
*/

#define VOID_TYPE	0
#define BOOLEAN		1
#define BYTETYPE	2
#define HALFWORD 	3
#define FIXEDTYPE	4
#define DOUBLEWORD	5
#define CHRTYPE 	6
#define DESCRIPT	7

#define PROC_VOID	8
#define PROC_BIT1	9
#define PROC_BIT8	10
#define PROC_BIT16 	11
#define PROC_BIT32	12
#define PROC_BIT64	13
#define CHAR_PROC_TYPE	14
#define FORWARD_CALL	15

#define FORWARD_LABEL	16
#define FORWARD_GOTO	17
#define DEFINED_LABEL	18
#define CHARFIXED	19
#define SPECIAL 	20
#define LITERAL		21
#define POINTER_TYPE	22
#define UNDEF_ARG	23
#define CONSTANT	24
#define STRINGCON	25
#define LAST_TYPE	26

static STRING typename[LAST_TYPE] = {
	STR_NULL,
	STR("bit(1)   "),	/* char */
	STR("bit(8)   "),	/* unsigned char */
	STR("bit(16)  "),	/* short */
	STR("bit(32)  "),	/* int or long */
	STR("bit(64)  "),	/* long or character */
	STR("character"),	/* unsigned char */
	STR("character*"),

	STR("procedure"),
	STR("procedure bit(1)"),	/* char */
	STR("procedure bit(8)"),	/* unsigned char */
	STR("procedure bit(16)"),	/* short */
	STR("procedure bit(32)"),	/* int or long */
	STR("procedure bit(64)"),	/* long or character */
	STR("procedure character"),	/* unsigned char*  */
	STR("<undefined call>"),

	STR("<undefined label>"),
	STR("<undefined goto>"),
	STR("label    "),
	STR("character"),
	STR("special  "),
	STR("literally"),
	STR("void *   "),
	STR("<undefined arg>"),
	STR("24"),
	STR("25"),
};

static STRING dcltypes[LAST_TYPE] = {
	STR("void "),		/* no type */
	STR("char "),		/* bit(1) */
	STR("unsigned char "),	/* bit(8) */
	STR("short "),		/* bit(16) */
	STR("int "),		/* bit(32) */
	STR("XPL_LONG "),	/* bit(64) */
	STR("__xpl_string *"),	/* character * */
	STR("__xpl_string *"),	/* character * */
	STR("void "),		/* no type */
	STR("char "),		/* bit(1) */
	STR("unsigned char "),	/* bit(8) */
	STR("short "),		/* bit(16) */
	STR("int "),		/* bit(32) */
	STR("XPL_LONG "),	/* bit(64) */
	STR("__xpl_string *"),	/* character * */
	STR("__xpl_string *"),	/* character * */
	STR_NULL,
	STR_NULL,
	STR_NULL,
	STR("char "),	/* CHARFIXED */
};

int returned_type;      /* Current procedure type */
int no_body;	/* TRUE if not emitting the body of a procedure */
int procmark;   /* Start of local variables in symbol table */
int initmark;   /* Last of the builtin function symbol table entries */
int parct;      /* Number of parameters to current procedure */
int ndecsy;     /* Current number of declared symbols */
/* Maxndecsy is the maximum of ndecsy over a compilation.  If maxndecsy
   begins to approach SYTSIZE then SYTSIZE should be increased */
int maxndecsy;

#define SYTSIZE (cSYTSIZE - 1)

#define syt_name(n) descriptor[(SYTSIZE_BASE) + n]
#define syt_mapped(n) descriptor[(SYTSIZE_BASE+cSYTSIZE) + n]
unsigned char syt_type[cSYTSIZE];	/* Type of the variable */
short declared_on_line[cSYTSIZE];	/* Source line number of DCL */
unsigned int syt_dim[cSYTSIZE];	/* Dimentions */
short syt_disp[cSYTSIZE];	/* Address */
short syt_count[cSYTSIZE];	/* Count of references to symbol */
short ptr[cSYTSIZE];	/* Next entry in hash list */
short hash[256];	/* Hash list array */
int idx;		/* Hash index */

/* Built-in function definitions */
struct builtin_s {
	int type;	/* Symbol type */
	int location;	/* Address displacement */
	int dimension;	/* Number of elements in the array */
	STRING name;	/* XPL string for name */
};

enum specials {BIN_SCALAR, BIN_ARRAY, BIN_STRING, BIN_CHAR_POINTER, BIN_CALL,
	BIN_MOPAR, BIN_ACCUMULATOR, BIN_LENGTH, BIN_SUBSTR, BIN_BYTE, BIN_SHL, BIN_SHR,
	BIN_INPUT, BIN_OUTPUT, BIN_FILE, BIN_INLINE, BIN_ADDR, BIN_SADDR,
	BIN_COREBYTE, BIN_COREHALFWORD, BIN_COREWORD, BIN_CORELONGWORD,
	BIN_BUILD_DESCRIPTOR, BIN_XFPRINTF, BIN_XPRINTF, BIN_XSPRINTF};

struct builtin_s builtin[] = {
	{FIXEDTYPE, 0, 0, STR("time_of_generation")},
	{FIXEDTYPE, 0, 0, STR("date_of_generation")},
	{FIXEDTYPE, 0, 0, STR("ndescript")},
	{CHRTYPE, 0, 1, STR("descriptor")},
	{CHRTYPE, 32, 32, STR("argv")},
	{FIXEDTYPE, 0, 0, STR("argc")},
	{PROC_BIT32, 0, 0, STR("time")},
	{PROC_BIT32, 0, 0, STR("date")},
	{PROC_VOID, 0, 0, STR("trace")},
	{PROC_VOID, 0, 0, STR("untrace")},
	{PROC_VOID, 0, 0, STR("compactify")},
	{PROC_VOID, 0, 0, STR("exit")},
	{FIXEDTYPE, 0, 0, STR(" ")},	/* Parameter 1.  Must follow exit()  */
	{PROC_VOID, 0, 0, STR("abort")},
	{FIXEDTYPE, 0, 0, STR("xerrno")},
	{PROC_BIT32, 0, 0, STR("xfopen")},
	{CHRTYPE, 0, 0, STR(" ")},	/* Parameter 1.  Must follow xfopen()  */
	{CHRTYPE, 0, 0, STR(" ")},	/* Parameter 2.  Must follow xfopen()  */
	{PROC_BIT32, 0, 0, STR("xfclose")},
	{FIXEDTYPE, 0, 0, STR(" ")},	/* Parameter 1.  Must follow xfclose()  */
	{PROC_BIT32, 0, 0, STR("xrewind")},
	{FIXEDTYPE, 0, 0, STR(" ")},	/* Parameter 1.  Must follow xrewind()  */
	{PROC_BIT32, 0, 0, STR("xmkstemp")},
	{CHRTYPE, 0, 0, STR(" ")},	/* Parameter 1.  Must follow xmkstemp()  */
	{CHRTYPE, 0, 0, STR(" ")},	/* Parameter 2.  Must follow xmkstemp()  */
	{PROC_BIT32, 0, 0, STR("xunlink")},
	{CHRTYPE, 0, 0, STR(" ")},	/* Parameter 1.  Must follow xunlink()  */
	{CHAR_PROC_TYPE, 0, 0, STR("unique")},
	{CHRTYPE, 0, 0, STR(" ")},	/* Parameter 1.  Must follow unique()  */
	{CHAR_PROC_TYPE, 0, 0, STR("expand_tabs")},
	{CHRTYPE, 0, 0, STR(" ")},	/* Parameter 1.  Must follow expand_tabs()  */
	{FIXEDTYPE, 0, 0, STR(" ")},	/* Parameter 2.  Must follow expand_tabs()  */
	{CHAR_PROC_TYPE, 0, 0, STR("hex")},
	{FIXEDTYPE, 0, 0, STR(" ")},	/* Parameter 1.  Must follow hex()  */
	{SPECIAL, BIN_LENGTH, 0, STR("length")},
	{SPECIAL, BIN_SUBSTR, 0, STR("substr")},
	{SPECIAL, BIN_BYTE, 0, STR("byte")},
	{SPECIAL, BIN_SHL, 0, STR("shl")},
	{SPECIAL, BIN_SHR, 0, STR("shr")},
	{SPECIAL, BIN_INPUT, 0, STR("input")},
	{SPECIAL, BIN_OUTPUT, 0, STR("output")},
	{SPECIAL, BIN_FILE, 0, STR("file")},
	{SPECIAL, BIN_INLINE, 0, STR("inline")},
	{SPECIAL, BIN_ADDR, 0, STR("addr")},
	{SPECIAL, BIN_SADDR, 0, STR("saddr")},
	{SPECIAL, BIN_COREBYTE, 0, STR("corebyte")},
	{SPECIAL, BIN_COREHALFWORD, 0, STR("corehalfword")},
	{SPECIAL, BIN_COREWORD, 0, STR("coreword")},
	{SPECIAL, BIN_CORELONGWORD, 0, STR("corelongword")},
	{SPECIAL, BIN_BUILD_DESCRIPTOR, 0, STR("build_descriptor")},
	{SPECIAL, BIN_XFPRINTF, 0, STR("xfprintf")},
	{SPECIAL, BIN_XPRINTF, 0, STR("xprintf")},
	{SPECIAL, BIN_XSPRINTF, 0, STR("xsprintf")},
	{0, 0, 0, {0, 0}}
};

/* These variables are either 32 to 64 bit addresses */
struct builtin_s builtin_pointers[] = {
	{FIXEDTYPE, 0, 0, STR("file_record_size")},
	{FIXEDTYPE, 0, 0, STR("freebase")},
	{FIXEDTYPE, 0, 0, STR("freelimit")},
	{FIXEDTYPE, 0, 0, STR("freepoint")},
	{0, 0, 0, {0, 0}}
};

/*  The compiler stacks declared below are used to drive the syntactic
    analysis algorithm and store information relevant to the interpretation
    of the text.  The stacks are all pointed to by the stack pointer sp.  */

#define STACKSIZE (cSTACKSIZE - 1)
unsigned char parse_stack[cSTACKSIZE]; /* Tokens of the partially parsed text */
unsigned char ps_type[cSTACKSIZE]; /* Operand type for expressions */
unsigned char ps_code[cSTACKSIZE]; /* Operand Code */
unsigned char ps_cnt[cSTACKSIZE]; /* Any count, parameters, subscripts... */
XPL_LONG ps_value[cSTACKSIZE]; /* holds number_value */
int ps_line[cSTACKSIZE]; /* holds card_count */
short ps_symtab[cSTACKSIZE]; /* Symbol table pointer */
unsigned char ps_bin[cSTACKSIZE]; /* Builtin function number, call, scalar or array */
#define ps_name(n) descriptor[(STACKSIZE_BASE) + n]
#define ps_text(n) descriptor[(STACKSIZE_BASE+cSTACKSIZE) + n]

/* ps_code values */
#define CODE_BOOLEAN	1
#define CODE_NUMERIC	3

/* sp points to the right end of the reducible string in the parse stack,
   mp points to the left end, and
   mpp1 = mp+1.
*/
int sp, mp, mpp1;
int stackmax;	/* The highest value of sp */
int itype;      /* Initialization type */
int initial_count;	/* Max number of elements in initial() list */

#define DO_GROUP	0
#define DO_STEP		1
#define DO_WHILE	2
#define DO_CASE		3

/* Declare statements and case statements require an auxiliary stack */

#define CASELIMIT 255
int casestack[CASELIMIT + 1];
int casep;   /* Points to the current position in casestack */
static STRING dclrm = STR("Identifier list too long");

/* Temporary descriptor stack */
#define DESCRIPTOR_STACK	80
int temp_descriptor;		/* Index of compiler temporary descriptor stack */
int max_temp_descriptor;	/* Max number of descriptors used */

/*                  P r o c e d u r e s :                                    */

/*
**	get_temp_descriptor()
**
**	Get a temporoary string descriptor to be used by the compiler.
**	These descriptors are NOT for the generated code.
*/
STRING *
get_temp_descriptor(void)
{
	temp_descriptor++;
	if (max_temp_descriptor < temp_descriptor) {
		max_temp_descriptor = temp_descriptor;
		if (temp_descriptor >= SYTSIZE_BASE) {
			printf("Descriptor stack overflow.\n");
			exit(1);
		}
	}
	return &descriptor[temp_descriptor];
}

/*
**	release_temp_descriptor(n)
**
**	Return temporoary string descriptors to the stack.
*/
void
release_temp_descriptor(int n)
{
	temp_descriptor -= n;
	if (temp_descriptor < DESCRIPTOR_STACK) {
		printf("Descriptor stack underflow.\n");
		exit(1);
	}
}

/*
**	hasher()
**
**	Symbol table hash function
*/
int
hasher(STRING *id)
{
	/*  Generates a hash code for the identifier id  */
	int l;

	l = id->_Length;
	return (BYTE(id, 0) + BYTE(id, SHR(l, 1)) * 29 + l * 118) & 255;
}

/*
**	symbol_lookup(identifier)
**
**	Lookup the identifer in the symbol table and return the symbol table index.
**	Return -1 if not found.
*/
int
symbol_lookup(STRING *ident)
{
	int i;

	i = hash[hasher(ident)];
	while (i >= 0) {
		idcompares++;
		if (COMPARE(&syt_name(i), ident) == 0) {
			return i;
		}
		i = ptr[i];
	}
	return -1;
}

/*
**	pad(string, width)
**
**	Pad a string with blanks
*/
STRING *
pad(STRING *string, int width)
{
#define pad_temp descriptor[73]
	STRING *pad_substr;
	int l;

	l = string->_Length;
	if (l >= width) return string;
	pad_substr = get_temp_descriptor();
	pad_temp = *string;
	while(width - l > x70._Length) {
		CAT(&pad_temp, &pad_temp, &x70);
		l += x70._Length;
	}
	SUBSTR3(pad_substr, &x70, 0, width - l);
	CAT(&pad_temp, &pad_temp, pad_substr);
	release_temp_descriptor(1);
	return &pad_temp;
}

/*
**	i_format(number, width)
**
**	Return a right justified number padded to width.
*/
STRING *
i_format(int number, int width)
{
#define i_format_string descriptor[74]
	STRING *i_format_substr;
	int l;

	DECIMAL(&i_format_string, number);
	l = i_format_string._Length;
	if (l >= width) return &i_format_string;
	i_format_substr = get_temp_descriptor();
	SUBSTR3(i_format_substr, &x70, 0, width - l);
	CAT(&i_format_string, &i_format_string, i_format_substr);
	release_temp_descriptor(1);
	return &i_format_string;
}

/*
**	us_decimal(outstr, number)
**
**	Return an integer as an unsigned decimal sring.
*/
STRING *
us_decimal(STRING *outstr, XPL_LONG number)
{
	char numbuf[32];
	XPL_UNSIGNED_LONG value, n;
	int i;

	value = number;
	for (i = (int) sizeof(numbuf) - 1; i > 1; i--) {
		n = value % 10;
		numbuf[i] = (char) (n + '0');
		value /= 10;
		if (value == 0) {
			break;
		}
	}
        outstr->_Length = (int) sizeof(numbuf) - i;
        outstr->_Address = &numbuf[i];
        return MOVE_TO_TOP(outstr, outstr);
}

/*
**	xFormat(number, width)
**
**	Return a hex number padded with zeros to width.
*/
STRING *
xFormat(int number, int width)
{
#define xFormat_temp descriptor[75]
	static char format_buffer[9];
	int i, j;

	if (width > 8 || width < 0) {
		width = 8;
	}
	for (i = 7; i >= 0; i--) {
		j = number & 15;
		format_buffer[i] = hex_digits[j];
		number >>= 4;
	}
	xFormat_temp._Length = width;
	xFormat_temp._Address = format_buffer + 8 - width;
	return &xFormat_temp;
}

/*
**	error(message, line, severity)
**
**	Prints and accounts for all error messages
*/
void
error(STRING *msg, int line, int severity)
{
	static STRING stars = STR("*** Error, ");
	static STRING detected = STR(" (detected at line ");
	static STRING xcom = STR(" in xcom). ***");
	static STRING previous = STR("*** Last previous error was detected on line ");
	static STRING tail = STR(". ***");
	static STRING give_up = STR("*** Too many severe errors, COMPILATION ABORTED ***");
	STRING *error_msg;

	error_count++;
	error_msg = get_temp_descriptor();
	/* If listing is supressed, force printing of this line. */
	if ((control['L'] & 1) == 0 || (control['M'] & 1) == 1) {
		CAT(error_msg, i_format(card_count, 4), &x1vbar);
		CAT(error_msg, error_msg, &buffer);
		CAT(error_msg, pad(error_msg, 86), &vbar);
		OUTPUT(0, error_msg);
	}
	OUTPUT(0, CAT(error_msg, pad(&x1, text_used + 6), &vbar));
	CAT(error_msg, &stars, msg);
	CAT(error_msg, error_msg, &detected);
	CAT_INT(error_msg, error_msg, line);
	CAT(error_msg, error_msg, &xcom);
	OUTPUT(0, error_msg);
	if (error_count > 1) {
		CAT_INT(error_msg, &previous, previous_error);
		CAT(error_msg, error_msg, &tail);
		OUTPUT(0, error_msg);
	}
	previous_error = card_count;
	if (severity > 0) {
		if (severe_errors > 25) {
			OUTPUT(0, &give_up);
			compiling = FALSE;
		} else {
			severe_errors++;
		}
	}
	release_temp_descriptor(1);
}

/*
**	get_card()
**
**	Does all card reading and listing
*/
void
get_card(void)
{
	int i, line;
	static STRING got_eof = STR(" /*\"/*'/* */ eof;end;eof ");
	STRING *get_card_rest;
	STRING *get_card_string;

	get_card_rest = get_temp_descriptor();
	get_card_string = get_temp_descriptor();
	expansion_count = 0;   /* Checked in scanner macro expansion */
	while (1) {
		INPUT(&buffer, input_unit);
		if (buffer._Length == 0) {	/* EOF or error */
			buffer = got_eof;
			control['L'] = FALSE;
		} else {
			__xpl_expand_tabs(&buffer, &buffer, 8);	/* Expand the tabs */
			card_count = card_count + 1;	/* Used to print on listing */
		}
		if (margin_chop > 0) {
			/* The margin control from dollar | */
			i = buffer._Length - margin_chop;
			SUBSTR2(get_card_rest, &buffer, i);
			SUBSTR3(&buffer, &buffer, 0, i);
		} else {
			get_card_rest->_Length = 0;
		}
		text = buffer;
		text_limit = text._Length - 1;
		text_used = 0;
		if (control['M'] & 1) {
			OUTPUT(0, &buffer);
		} else
		if (control['L'] & 1) {
			CAT(get_card_string, i_format(card_count, 4), &x1vbar);
			CAT(get_card_string, get_card_string, &buffer);
			CAT(get_card_string, pad(get_card_string, 86), &vbar);
			CAT(get_card_string, get_card_string, get_card_rest);
			CAT(get_card_string, get_card_string, &current_procedure);
			CAT(get_card_string, get_card_string, &information);
			OUTPUT(0, get_card_string);
		}
		information._Length = 0;
		if (BYTE(&buffer, 0) != '#') {
			break;
		}
		/* C preprocessor statements start with # in column one */
		line = 0;
		while (1) {	/* # number "filename" digit */
			SUBSTR2(&buffer, &buffer, 1);
			i = BYTE(&buffer, 0);
			if (i >= '0' && i <= '9') {
				line = (line * 10) + i - '0';
			} else
			if (i == ' ' && line == 0) {
				continue;
			} else
			if (line) {
				/* This is a linemarker */
				card_count = line - 1;
				break;
			} else {
				/* All other preprocessor commands are discarded */
				break;
			}
		}
	}
	release_temp_descriptor(2);
	cp = 0;
}


/*			The scanner procedures				*/

/*
**	cchar()
**
**	Move to the next character.
**	Read a new card if at the end of line.
*/
void
cchar(void)
{
	/* Used for strings to avoid card boundary problems */
	cp = cp + 1;
	if (cp <= text_limit) return;
	get_card();
}

/*
**	deblank()
**
**	Scan past the blanks.
*/
void
deblank(void)
{
	/* Used by bchar() */
	cchar();
	while(BYTE(&text, cp) == ' ') {
		cchar();
	}
}

/*
**	bchar()
**
**	Calculate the bit width for bit strings.  Like this "(1)"
**
**	Return FALSE if at end of string.
**
**      A note on language design:  Allowing strings to span multiple input
**      lines without using some formal escape sequence makes it difficult
**      for the scanner to stop scanning when the input stream has unbalanced
**      quotes.  It is a common error condition where a programmer will
**      forget to terminate a string.  In this case the compiler will try
**      to parse the text of subsequent strings and build strings out of
**      the body of the program.  Since this implementation allows effectively
**      unlimited strings the problem gets worse.
**         In an effort to mitigate this problem all the errors found within
**      strings are considered severe.  Eventually the "compiling" flag will
**      be cleared and the program will exit.
**         A much better design would be to require strings to be terminated
**      at the end of each line and restarted on the next line.  The scanner
**	could be used to paste them back together.
*/
int
bchar(void)
{
	static STRING illegal_bit_string = STR("Illegal bit string width: ");
	static STRING missing = STR("Missing ) in bit string");
	static STRING ill_escape = STR("Illegal escape sequence");
	STRING *bchar_string;
	int i, s1;

	s1 = 0;
	scanner_value = 0;
	while(charbase == FALSE) {
		if (next_ch >= 0) {
			ch = next_ch;
			next_ch = -1;
		} else {
			cchar();
			ch = BYTE(&text, cp);
		}
		while(compiling) {
			if (hex_digit[ch] < 16) {
				scanner_value = (scanner_value << 4) + hex_digit[ch];
				s1 += 4;
				if (s1 >= jbase) {
					return TRUE;
				}
			} else
			if (ch == '(') {
				if (s1 == 0) {
					break;
				}
				next_ch = ch;
				return TRUE;
			} else
			if (ch == '\"') {
				if (s1 == 0) {
					return FALSE;
				}
				next_ch = ch;
				return TRUE;
			} else
			if (ch == ' ' && s1 != 0) {
				return TRUE;
			} else
			if (ch != ' ') {
				bchar_string = get_temp_descriptor();
				SUBSTR3(bchar_string, &text, cp, 1);
				error(CAT(bchar_string, &ill_bit_char,
					bchar_string), __LINE__, 1);
				release_temp_descriptor(1);
			}
			cchar();
			ch = BYTE(&text, cp);
		}
		/*  (base width)  */
		deblank();
		for (jbase = 0; compiling; ) {
			ch = BYTE(&text, cp);
			if (hex_digit[ch] <= 9) {
				jbase = (jbase * 10) + hex_digit[ch];
			} else
			if (ch == 'c' || ch == 'C') {
				charbase = TRUE;
				jbase = 8;
				deblank();
				break;
			} else
			if (ch != ' ') {
				break;
			}
			cchar();
		}
		if (jbase < 1 || jbase > sizeof(scanner_value) * 8) {
			bchar_string = get_temp_descriptor();
			error(CAT_INT(bchar_string, &illegal_bit_string, jbase),
				__LINE__, 1);
			release_temp_descriptor(1);
			jbase = 4;  /* default width for error */
		}
		value_mask = 1;
		value_mask = ~((value_mask << jbase) - 1);
		if (BYTE(&text, cp) != ')') {
			error(&missing, __LINE__, 1);
		}
		next_ch = -1;
		s1 = 0;
		if (!compiling) {
			return FALSE;
		}
	}
	/* C style character string */
	while(compiling) {
		if (next_ch >= 0) {
			ch = next_ch;
			next_ch = -1;
		} else {
			cchar();
			ch = BYTE(&text, cp);
		}
		if (ch != '\\') {
			scanner_value = ch;
			return ch != '\"';
		}
		cchar();
		ch = BYTE(&text, cp);
		if (escape_sequence[ch]) {
			scanner_value = escape_sequence[ch];
			return TRUE;
		}
		if (hex_digit[ch] <= 7) {	/* Octal */
			scanner_value = 0;
			for (i = 0; i < 3; i++) {
				if (hex_digit[ch] <= 7) {
					scanner_value = (scanner_value << 3) + hex_digit[ch];
				} else {
					break;
				}
				cchar();
				ch = BYTE(&text, cp);
			}
			next_ch = ch;
			return TRUE;
		}
		if (ch == 'x') {	/* hex */
			scanner_value = 0;
			while(1) {
				cchar();
				ch = BYTE(&text, cp);
				if (hex_digit[ch] < 16) {
					scanner_value = (scanner_value << 4) + hex_digit[ch];
				} else {
					next_ch = ch;
					return ch != '\"';
				}
			}
		} else {
			error(&ill_escape, __LINE__, 1);
			/* ch can't be Double Quote */
			return TRUE;
		}
	}
	return FALSE;
}

/*
**	build_bcd(c)
**
**	Force the string to the top of the free string area and add a byte to it.
*/
void
build_bcd(int c)
{
	if (bcd._Length > 0) {
		CAT(&bcd, &bcd, &x1);
	} else {
		SUBSTR2(&bcd, CAT(get_temp_descriptor(), &x1, &x1), 1);
		release_temp_descriptor(1);
	}
	__xpl_put_byte(&bcd, bcd._Length - 1, c);
}

/*
**	scan()
**
**	Get the next token.
*/
void
scan(void)
{
	int s1, s2, i;
	STRING *scan_string;
//	static STRING lstrgm = STR("string too long");
	static STRING ill_char = STR("Illegal character: ");
	static STRING macro_overflow = STR("Macro expansion too long");
	static STRING too_many_expansions = STR("Too many expansions for ");
	static STRING literally = STR(" literally: ");
	static STRING macro_left = STR("     +");
	static STRING macro_right = STR("+");

	count_scan++;
	failsoft = TRUE;
	bcd = null;
	number_value = 0;
    scan1:
	while (1) {
		if (cp > text_limit) {
			get_card();
		} else { /* Discard last scanned value */
			text_used = text_used + cp;
			text_limit = text_limit - cp;
			SUBSTR2(&text, &text, cp);
			cp = 0;
		}
		/*  Branch on next character in text */
		switch(chartype[BYTE(&text, 0)]) {
		case 0:
			/* Illegal characters fall here  */
			error(CAT(get_temp_descriptor(), &ill_char,
				xFormat(BYTE(&text, 0), 2)), __LINE__, 0);
			release_temp_descriptor(1);
			break;
		case 1:
			/* blank */
			for (cp = 1; cp <= text_limit && BYTE(&text, cp) == ' '; cp++);
			cp--;
			break;
		case 2:
			/* Single quote ('):  Character string */
			while (1) {
				token = string_token;
				cchar();
				s1 = cp;
				scan_string = get_temp_descriptor();
				while(BYTE(&text, cp) != '\'') {
					cp = cp + 1;
					if (cp > text_limit) {
						SUBSTR3(scan_string, &text, s1, cp - s1);
						CAT(&bcd, &bcd, scan_string);
						cp = s1 = 0;
						get_card();
					}
				}
				if (cp > s1) {
					SUBSTR3(scan_string, &text, s1, cp - s1);
					CAT(&bcd, &bcd, scan_string);
				}
				release_temp_descriptor(1);
				cchar();
				if (BYTE(&text, cp) != '\'') {
					return;
				}
				CAT(&bcd, &bcd, &single_quote);
				text_used = text_used + cp;
				text_limit = text_limit - cp;
				SUBSTR2(&text, &text, cp);
				cp = 0;   /* Prepare to resume scanning string */
			}
		case 3:
			/* Double quote ("):  Bit string */
			jbase = 4;	/* Default width */
			value_mask = ~(SHL(1, jbase) - 1);
			token = number_token;  /* Assume short bit string */
			s1 = s2 = 0;
			next_ch = -1;
			charbase = FALSE;
			/* bchar() returns value in scanner_value */
			while (bchar()) {
				if (scanner_value & value_mask) {
					error(CAT_INT(get_temp_descriptor(),
						&ill_bit_char, scanner_value), __LINE__, 1);
					release_temp_descriptor(1);
					scanner_value &= value_mask;
				}
				s1 += jbase;
				if (s1 > sizeof_long * 8) {
					token = string_token;     /* Bit string */
				}
				while (s2 >= 8) {
					s2 -= 8;
					build_bcd(SHR(number_value, s2));
				}
				number_value = SHL(number_value, jbase) + scanner_value;
				s2 += jbase;
			} /* Of do while ch...  */
			cp = cp + 1;
			while (s2 > 8) {
				s2 -= 8;
				build_bcd(SHR(number_value, s2));
			}
			build_bcd(SHL(number_value, 8 - s2));
			return;
		case 4:
			/* A letter: Identifiers and reserved words */
			while (1) {
				if (++cp > text_limit) {
					/*  End of card  */
					CAT(&bcd, &bcd, &text);
					get_card();
					cp = 0;
				}
				if (not_letter_or_digit[BYTE(&text, cp)]) {
					/* End of identifier */
					if (cp > 0) {
						scan_string = get_temp_descriptor();
						SUBSTR3(scan_string, &text, 0, cp);
						CAT(&bcd, &bcd, scan_string);
						release_temp_descriptor(1);
					}
					break;
				}
			}
			s1 = bcd._Length;
			if (s1 > 255) {
				static STRING its_long = STR("identifier too long");

				error(&its_long, __LINE__, 0);
				SUBSTR3(&bcd, &bcd, 0, 255);
				s1 = bcd._Length;
			}
			if (s1 > 1 && s1 <= reserved_limit) {
				/* Check for reserved words */
				for (i = v_index[s1-1]; i < v_index[s1]; i++) {
					if (strncmp(bcd._Address, v[i], s1) == 0) {
						token = i;
						bcd._Length = 0;
						return;
					}
				}
			}
			i = symbol_lookup(&bcd);
			if (i < 0 || syt_type[i] != LITERAL) {
				/* Not a macro.  Must be an <identifier> */
				token = ident_token;
				return;
			}
			if (i == address_symbol) {
				/* This will allow the address macro to be redefined */
				token = bit_token;
				if (right_conflict(parse_stack[sp])) {
					token = ident_token;
					return;
				}
			}
			if (i == line_symbol) {
				us_decimal(&bcd, card_count);
			} else {
				bcd = syt_mapped(i);
			}
			scan_string = get_temp_descriptor();
			syt_count[i]++;
			if (expansion_count < EXPANSION_COUNT_LIMIT) {
				expansion_count++;
			} else {
				CAT(scan_string, &too_many_expansions, &syt_name(i));
				CAT(scan_string, scan_string, &literally);
				CAT(scan_string, scan_string, &bcd);
				error(scan_string, __LINE__, 1);
				bcd._Length = 0;
			}
			SUBSTR2(&text, &text, cp);
			text_limit = text_limit - cp;
			/* This test is to catch run away expansions */
			if (bcd._Length + text_limit > EXPANSION_SIZE_LIMIT) {
				error(&macro_overflow, __LINE__, 1);
			} else {
				CAT(&text, &bcd, &text);
			}
			bcd._Length = 0;
			text_limit = text._Length - 1;
			if (control['X'] & 1) {
				CAT(scan_string, pad(&macro_left,
					text_used + 6), &text);
				CAT(scan_string, pad(scan_string, 86),
					&macro_right);
				OUTPUT(0, scan_string);
			}
			release_temp_descriptor(1);
			cp = 0;
			goto scan1;
		case 5:
			/* Digit: a number */
			token = number_token;
			while (1) {
				for ( ; cp <= text_limit; cp++) {
					s1 = BYTE(&text, cp);
					if (s1 < '0' || s1 > '9') {
						return;
					}
					number_value = 10 * number_value + s1 - '0';
				}
				get_card();
			}
		case 6:
			/*  A /:  may be divide or start of comment */
			cchar();
			if (BYTE(&text, cp) == '/') {
				/* We have a C++ style comment */
				cp = text_limit;
				break;
			}
			if (BYTE(&text, cp) != '*') {
				token = divide_token;
				return;
			}
			/* We have a C or PL/I style comment */
			s1 = s2 = ' ';
			while (s1 != '*' || s2 != '/') {
				/* Look for a control character */
				if (s1 == '$') {
					/* Control character */
					if (s2 == '>') {	/* push */
						cchar();
						s2 = BYTE(&text, cp);
						control[s2] <<= 1;
						/* The toggle is now off */
					} else
					if (s2 == '<') {	/* pop */
						cchar();
						s2 = BYTE(&text, cp);
						control[s2] >>= 1;
					} else {
						control[s2] ^= 1;
					}
					if (s2 == '|') {
						if (control['|'] & 1) {
							margin_chop = text_limit - cp + 1;
						} else {
							margin_chop = 0;
						}
					} else
					if (s2 == 'I' && (control['I'] & 1) == 1) {
						/* Ignore case */
						ignore_case_macro();
					}
				}
				s1 = s2;
				cchar();
				s2 = BYTE(&text, cp);
			}
			break;
		case 7:
			/* Special characters */
			token = tx[BYTE(&text, 0)];
			cp = 1;
			return;
		case 8:
			/* A |:  may be "or" or "cat" */
			cchar();
			if (BYTE(&text, cp) == '|') {
				cchar();
				token = concatenate_token;
			} else {
				token = or_token;
			}
			return;
		default:
			error(&unimplemented, __LINE__, 0);
			break;
		} /* End of case on chartype */
		cp = cp + 1;  /* Advance scanner and resume search for token  */
	}
}

/*                    Code emission procedures                       */

/*
**	next_temp()
**
**	Return the next temporary variable name.
*/
int
next_temp(void)
{
	temp_variable_name++;
	return temp_variable_name;
}

/*
**	temp_name(outstr, id, extra)
**
**	When (extra == 1) outstr->_Address may be used in a C string
**	functions such as printf().
**
**	Return the temporary variable name as a string.
*/
STRING *
temp_name(STRING *outstr, int id, int extra)
{
	char name[32];

	sprintf(name, "__xpl_%d", id);
	outstr->_Length = strlen(name) + extra;
	outstr->_Address = name;
	return MOVE_TO_TOP(outstr, outstr);
}

/*
**	global_scope(name)
**
**	Return TRUE if this name is in the Global Scope table.
**	Returns TRUE if the table is full.
*/
int
global_scope(STRING *name)
{
	int i, s1;

	if (top_global >= GLOBAL_LIMIT) {
		/* If the table gets full we must map everything */
		return TRUE;
	}
	s1 = name->_Length;
	for (i = global_index[s1 - 1]; i < global_index[s1]; i++) {
		if (COMPARE(name, &global_name(i)) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

/*
**	enter_global(name)
**
**	Identifiers are entered into this table to commemorate the fact that they
**	were promoted to Global Scope.  Only the first declaration can be promoted.
**	Subsequent declarations must be mangled.
*/
void
enter_global(STRING *name)
{
	int i, j, k, l;

	if (global_scope(name)) {
		/* The table is full or the name is already in the table */
		return;
	}
	top_global++;
	i = name->_Length;
	j = global_index[i];
	for (l = 1; l <= top_global - j; l++) {
		k = top_global - l;
		global_name(k+1) = global_name(k);
	}
	global_name(j) = *name;
	for (j = i; j < (sizeof(global_index) / sizeof(global_index[0])); j++) {
		global_index[j]++;
	}
}

/*
**	flush_emit_delay()
**
**	If the string emit_delay is not empty, write it to the output file.
**	Reset the emit_delay string.
*/
void
flush_emit_delay(void)
{
	int k, len;
	char *stp;

	if (emit_delay._Length) {
		/* Write out the text string */
		len = emit_delay._Length;
		stp = emit_delay._Address;
		for (k = 0; k < len; k++) {
			putc(*stp, out_file);
			stp++;
		}
		emit_delay._Length = 0;
		putc('\n', out_file);
		output_line++;
	}
}

/*
**	emit_divert(level)
**
**	Emit diverted code
*/
void
emit_divert(int level)
{
	int i, j, k, len;
	char *stp;

	flush_emit_delay();
	j = level * DIVERT_LIMIT;
	for (i = 0; i < DIVERT_LIMIT; i++) {
		/* Write out the text string */
		len = divert(i + j)._Length;
		stp = divert(i + j)._Address;
		for (k = 0; k < len; k++) {
			if (*stp == '\n') {
				output_line++;
			}
			putc(*stp, out_file);
			stp++;
		}
		divert(i + j)._Length = 0;
	}
	divert_index[level] = 0;
}

/*
**	emit_string_divert(level)
**
**	Emit diverted code to the string file
**	Do not write the procedure definition in location zero.
*/
void
emit_string_divert(int level)
{
	int i, j, k, len;
	char *stp;

	j = level * DIVERT_LIMIT;
	for (i = 1; i < DIVERT_LIMIT; i++) {
		/* Write out the text string */
		len = divert(i + j)._Length;
		stp = divert(i + j)._Address;
		for (k = 0; k < len; k++) {
			putc(*stp, string_file);
			stp++;
		}
		divert(i + j)._Length = 0;
	}
	divert_index[level] = 1;
}

/*
**	emit(text, line_number)
**
**	Emit one line of text to the .c file
*/
void
emit(STRING *string, int line_number)
{
	static STRING tabs = STR("\t\t\t\t\t");
	STRING *emit_text;
	int i, j, len;
	char *stp;

	if (no_body) {
		/* an External or Transparent procedure */
		return;
	}
	count_emit++;
	if (control['E'] & 1) {
		/* Indent the text */
		for (i = (nest_block > 5) ? 5 : nest_block; i > 0; i--) {
			putc('\t', stdout);
		}
		/* Write out the text string */
		len = string->_Length;
		stp = string->_Address;
		for (i = 0; i < len; i++) {
			putc(*stp, stdout);
			stp++;
		}
		/* Add a <newline> character */
		putc('\n', stdout);
	}
	if (proc_nest < DIVERT_LEVEL && divert_code[proc_nest]) {
		j = proc_nest * DIVERT_LIMIT;
		if (divert_index[proc_nest] >= DIVERT_LIMIT) {
			for (i = 2; i < DIVERT_LIMIT; i++) {
				CAT(&divert(1 + j), &divert(1 + j), &divert(i + j));
				divert(i + j)._Length = 0;
			}
			divert_index[proc_nest] = 2;
		}
		emit_text = get_temp_descriptor();
		CAT(emit_text, SUBSTR3(emit_text, &tabs, 0, nest_block), string);
		CAT(&divert(j + divert_index[proc_nest]), emit_text, &newline);
		divert_index[proc_nest]++;
		release_temp_descriptor(1);
		return;
	}
	if (control['K'] & 1) {
		if (output_line < line_number || emit_delay._Length + string->_Length > 120) {
			flush_emit_delay();
		}
		if (output_line < line_number && output_line + 8 > line_number) {
			while (output_line < line_number) {
				putc('\n', out_file);
				output_line++;
			}
		} else
		if (output_line != line_number && line_number > 0) {
			fprintf(out_file, "#line %d\n", output_line = line_number);
		}
		if (emit_delay._Length == 0) {
			SUBSTR3(&emit_delay, &tabs, 0, nest_block);
		} else {
			CAT(&emit_delay, &emit_delay, &x2);
		}
		CAT(&emit_delay, &emit_delay, string);
		return;
	}
	flush_emit_delay();
	/* Indent the text */
	for (i = (nest_block > 5) ? 5 : nest_block; i > 0; i--) {
		putc('\t', out_file);
	}
	/* Write out the text string */
	len = string->_Length;
	stp = string->_Address;
	for (i = 0; i < len; i++) {
		putc(*stp, out_file);
		stp++;
	}
	/* Add a <newline> character */
	putc('\n', out_file);
	output_line++;
}

/*
**	emit_preprocessor(text, line_number)
**
**	Emit one line of text to the .c file, left justified.
*/
void
emit_preprocessor(STRING *string, int line_number)
{
	int i;

	i = nest_block;
	nest_block = 0;
	emit(string, line_number);
	nest_block = i;
}

/*
**	emit_declare(string, line_number)
**
**	Emit declaration or procedure definition.
*/
#define emit_declare(string, line_number) emit(string, line_number)

/*
**	emit_code(string, line_number)
**
**	Emit one line of code.  This function controls code diverts.
*/
void
emit_code(STRING *string, int line_number)
{
	static STRING undef = STR("Undefined parameter: ");
	static STRING equal_star = STR(" = *");
	STRING *emit_code_text;
	STRING *emit_code_mapped;
	int sym, i, t, s;

	if (proc_nest == 0) {
		if (divert_code[0] == FALSE) {
			divert_code[0] = TRUE;
			nest_block++;
		}
		emit(string, line_number);
		return;
	}
	if (proc_nest >= DIVERT_LEVEL) {
		emit(string, line_number);
		return;
	}
	emit_code_text = get_temp_descriptor();
	emit_code_mapped = get_temp_descriptor();
	if (divert_code[proc_nest] && divert_nested[proc_nest]) {
		emit_divert(proc_nest);
		divert_code[proc_nest] = FALSE;
		if (proc_nest > 1) {
			enter_global(&syt_name(procmark - 1));
		}
		/* Enter symbol names that were just promoted to Global Scope */
		for (i = procmark; i <= ndecsy; i++) {
			if (syt_name(i)._Length != 0 &&
				COMPARE(&syt_name(i), &syt_mapped(i)) == 0) {
				enter_global(&syt_name(i));
			}
		}
		/* Search symbol table for parameters */
		/* Send the definition and copy the data for all parameters */
		for (i = 0; i < parct; i++) {
			sym = procmark + i;
			if (syt_type[sym] == UNDEF_ARG) { /* undefined */
				CAT(emit_code_text, &undef, &syt_name(sym));
				error(emit_code_text, __LINE__, 1);
				continue;
			}
			/* #define __xpl_99 type syt_mapped() */
			CAT(emit_code_mapped, &syt_mapped(sym), &null_terminator);
			t = syt_disp[procmark - 1] + i;
			s = next_temp();
			fprintf(string_file, "#define __xpl_%d %s__xpl_%d\n", t,
				dcltypes[syt_type[sym]]._Address, s);
			if (syt_type[sym] == CHRTYPE) { /* character */
				fprintf(string_file, "#define %s descriptor[%d]\n",
					emit_code_mapped->_Address, syt_disp[sym]);
				CAT(emit_code_text, &syt_mapped(sym), &equal_star);
				temp_name(emit_code_mapped, s, 0);
				CAT(emit_code_text, emit_code_text, emit_code_mapped);
				CAT(emit_code_text, emit_code_text, &semicolon);
			} else {
				fprintf(string_file, "static %s%s;\n",
					dcltypes[syt_type[sym]]._Address,
					emit_code_mapped->_Address);
				CAT(emit_code_text, &syt_mapped(sym), &x1_equal_x1);
				temp_name(emit_code_mapped, s, 0);
				CAT(emit_code_text, emit_code_text, emit_code_mapped);
				CAT(emit_code_text, emit_code_text, &semicolon);
			}
			emit(emit_code_text, line_number);
		}
	} else
	if (divert_code[proc_nest]) {
		emit_divert(proc_nest);
		divert_code[proc_nest] = FALSE;
		if (proc_nest > 1) {
			enter_global(&syt_name(procmark - 1));
		}
		/* Search symbol table for parameters */
		/* Send the definitions to the string file and copy descriptors */
		for (i = 0; i < parct; i++) {
			sym = procmark + i;
			if (syt_type[sym] == UNDEF_ARG) { /* undefined */
				CAT(emit_code_text, &undef, &syt_name(sym));
				error(emit_code_text, __LINE__, 1);
				continue;
			}
			/* #define __xpl_99 type syt_mapped() */
			CAT(emit_code_mapped, &syt_mapped(sym), &null_terminator);
			t = syt_disp[procmark - 1] + i;
			if (syt_type[sym] == CHRTYPE) { /* character */
				s = next_temp();
				fprintf(string_file, "#define __xpl_%d %s__xpl_%d\n", t,
					dcltypes[DESCRIPT]._Address, s);
				fprintf(string_file, "#define %s descriptor[%d]\n",
					emit_code_mapped->_Address, syt_disp[sym]);
				CAT(emit_code_text, &syt_mapped(sym), &equal_star);
				temp_name(emit_code_mapped, s, 0);
				CAT(emit_code_text, emit_code_text, emit_code_mapped);
				CAT(emit_code_text, emit_code_text, &semicolon);
				emit(emit_code_text, line_number);
			} else
			if (syt_dim[sym]) {	/* An array */
				static STRING index_zero_eq = STR("[0] = __xpl_");

				s = next_temp();
				fprintf(string_file, "#define __xpl_%d %s__xpl_%d\n", t,
					dcltypes[syt_type[sym]]._Address, s);
				/* type syt_mapped[syt_dim]; */
				CAT(emit_code_text, &dcltypes[syt_type[sym]],
					&syt_mapped(sym));
				CAT(emit_code_text, emit_code_text, &left_bracket);
				CAT_INT(emit_code_text, emit_code_text, syt_dim[sym]);
				CAT(emit_code_text, emit_code_text, &bracket_semicolon);
				emit(emit_code_text, line_number);
				/* syt_mapped[0] = __xpl_s; */
				CAT(emit_code_text, &syt_mapped(sym), &index_zero_eq);
				CAT_INT(emit_code_text, emit_code_text, s);
				CAT(emit_code_text, emit_code_text, &semicolon);
				emit(emit_code_text, line_number);
			} else {
				fprintf(string_file, "#define __xpl_%d %s %s\n", t,
					dcltypes[syt_type[sym]]._Address, emit_code_mapped->_Address);
			}
		}
	}
	release_temp_descriptor(2);
	emit(string, line_number);
}

/*
**	push_procmark(p)
**
**	Called at the beginning of a function declaration to save procmark and parct.
**	Handles diverts for nested functions.
*/
void
push_procmark(int p)
{
	STRING *push_procmark_text;
	int sym, i;

	push_procmark_text = get_temp_descriptor();
	if (proc_nest > 0 && proc_nest < DIVERT_LEVEL) {
		if (divert_nested[proc_nest] == 0) {
			divert_nested[proc_nest] = 1;
			sym = procmark;
			for (i = 0; i < parct; i++, sym++) {
				CAT(push_procmark_text, &dcltypes[syt_type[sym]],
						&syt_mapped(sym));
				CAT(push_procmark_text, push_procmark_text, &semicolon);
				emit(push_procmark_text, ps_line[p]);
			}
		}
		emit_string_divert(proc_nest);
	}
	release_temp_descriptor(1);

	/* Save the old value */
	ps_symtab[p] = procmark;

	/* Set up the new one */
	procmark = ndecsy + 1;
}

/*
**	push_divert(p)
**
**	Called at the start of a new procedure to emit diverted code and
**	bump proc_nest.
*/
void
push_divert(int p)
{
	proc_nest++;
	if (proc_nest < DIVERT_LEVEL) {
		divert_index[proc_nest] = 0;
		divert_code[proc_nest] = TRUE;
		divert_nested[proc_nest] = FALSE;
	}
}

/*
**	pop_divert()
**
**	Called at the end of a procedure to emit diverted code.
*/
void
pop_divert(void)
{
	if (proc_nest < DIVERT_LEVEL) {
		emit_divert(proc_nest);
	}
	proc_nest--;
}

/*
**	string_constant(string)
**
**	Emit a string constant to the .xh file.
**	Return the fabricated variable number.
*/
int
string_constant(STRING *string)
{
	STRING *string_constant_text;
	int v, i, len, ch;

	for (i = 0; i < string_cache_max; i++) {
		if (COMPARE(string, &string_cache(i)) == 0) {
			string_cache_hits++;
			return string_cache_number[i];
		}
	}
	v = next_temp();
	string_cache(strack) = *string;
	string_cache_number[strack] = v;
	strack++;
	if (strack >= cSTRING_LIMIT) {
		strack = 0;
	}
	if (string_cache_max < cSTRING_LIMIT) {
		string_cache_max++;
	}
	string_constant_text = get_temp_descriptor();
	temp_name(string_constant_text, v, 1);
	fprintf(string_file, "static __xpl_string %s = {%d, \"",
		string_constant_text->_Address, string->_Length);
	release_temp_descriptor(1);
	len = 25 + 9 + 3;
	for (i = 0; i < string->_Length; i++) {
		if (len > 70) {
			fprintf(string_file, "\"\n\"");
			len = 1;
		}
		ch = string->_Address[i] & 255;
		if (printing[ch]) {
			putc(ch, string_file);
			len++;
		} else {
			fprintf(string_file, "\\%03o", ch);
			len += 4;
		}
	}
	fprintf(string_file, "\"};\n");
	return v;
}

/*
**	dump_init_strings(last)
**
**	Dump the init strings to the .xh file.
**	Last is set TRUE if this is the last call.
*/
void
dump_init_strings(int last)
{
	int i;

	if (last) {
		fprintf(string_file, "void __xpl_init_strings(void) {\n");
		/* "}" */
	} else {
		fprintf(string_file, "static void __xpl_init_string%d(void) {\n",
			string_proc + 1); /* "}" */
	}
	if (string_proc > 0) {
		/* Initialization functions are chained */
		fprintf(string_file, "__xpl_init_string%d();\n", string_proc);
	}
	for (i = 0; i < init_string; i++) {
		fprintf(string_file, "descriptor[%d] = __xpl_%d;\n",
			string_index[i], string_number[i]);
	}
	fprintf(string_file, "}\n");
	init_string = 0;
	string_proc++;
}

/*
**	enter_init_string(desc, identifier)
**
**	Enter a new descriptor to be initialized at runtime start-up.
**	These values get set when the program is first executed.  The initialization
**	covers the entire program and is only done once.
*/
void
enter_init_string(int desc, int identifier)
{
	if (init_string >= INIT_STRING_MAX) {
		dump_init_strings(FALSE);
	}
	string_index[init_string] = desc;
	string_number[init_string] = identifier;
	init_string++;
}

/*
**	emit_headers()
**
**	Emit the start of the .c file.  Starting with the include headers.
*/
void
emit_headers(void)
{
	fprintf(out_file, "#include \"xpl.h\"\n");
	fprintf(out_file, "#include \"%s\"\n", string_filename);
	fprintf(out_file, "static int date_of_generation = {%d};\n", __xpl__date());
	fprintf(out_file, "static int time_of_generation = {%d};\n", __xpl__time());
	fprintf(out_file, "static unsigned char *corebyte;\n");
	fprintf(out_file, "static short *corehalfword;\n");
	fprintf(out_file, "static int *coreword, argc;\n");
	fprintf(out_file, "static XPL_LONG *corelongword;\n");
	output_line = 7;
}

/*
**	emit_main_program()
**
**	Emit the function main()
*/
void
emit_main_program(void)
{
	static STRING freespace = STR("FREESPACE");
	int sym, i;
	char *s;

	fprintf(string_file, "int ndescript = {%d};\n", dsp - 1);
	fprintf(string_file, "__xpl_string descriptor[%d];\n", dsp);
	dump_init_strings(TRUE);
	flush_emit_delay();
	fprintf(out_file, "int __xpl_user_main(void) {\n");  output_line++;
	emit_divert(0);
	fprintf(out_file, "\treturn 0;\n");
	fprintf(out_file, "}\n");
	if (control['m'] & 1) {
		/* Do not emit code for main().  This allows XPL to be called from
		   some other program or language. */
		return;
	}
	fprintf(out_file, "#ifndef FREESPACE\n");
	sym = symbol_lookup(&freespace);
	if (sym >= 0) {
		/* This will allow:  declare FREESPACE literally '0x80000';
		   to set the FREESPACE from inside the compiled program.
		*/
		fprintf(out_file, "#define FREESPACE ");
		s = syt_mapped(sym)._Address;
		for (i = 0; i < syt_mapped(sym)._Length; i++) {
			putc(*s, out_file);
			s++;
		}
		putc('\n', out_file);
	} else {
		fprintf(out_file, "#define FREESPACE 0\n");
	}
	fprintf(out_file, "#endif\n");
	fprintf(out_file, "static int __xpl_strlen(char *s){int i;for(i=0;*s++;i++){};return i;}\n");
	fprintf(out_file, "int main(int __c_argc, char **__c_argv) {\n");
	fprintf(out_file, "\tfor (argc = 0; argc < __c_argc && argc < %d; argc++) {\n",
		argv_limit);
	fprintf(out_file, "\t\tdescriptor[argc+%d]._Address = __c_argv[argc];\n",
		XPLRT_STRINGS);
	fprintf(out_file, "\t\tdescriptor[argc+%d]._Length = __xpl_strlen(__c_argv[argc]);\n",
		XPLRT_STRINGS);
	fprintf(out_file, "\t}\n");
	fprintf(out_file, "\tif (__xpl_runtime_init(FREESPACE)) return 1;\n");
	fprintf(out_file, "\t__xpl_init_strings();\n");
	fprintf(out_file, "\treturn __xpl_user_main();\n");
	fprintf(out_file, "}\n");
}

/*
**	get_temp()
**
**	Get a temporory descriptor address from the stack.
*/
int
get_temp(void)
{
	if (tsp < TSP_MAX) {
		if (temp_stack[tsp] == 0) {
			temp_stack[tsp] = dsp++;
		}
		return temp_stack[tsp++];
	}
	return dsp++;
}

/*
**	reset_temp()
**
**	Reset the temporory descriptor address stack.
**	This should be called after each statement.
*/
void
reset_temp(void)
{
	tsp = 0;
}

/*
**	clear_temp()
**
**	Clear the temporory descriptor address stack.
**	This should be called after each procedure.
*/
void
clear_temp(void)
{
	for (tsp = 0; tsp < TSP_MAX; tsp++) {
		temp_stack[tsp] = 0;
	}
	tsp = 0;
}

/*
**	valid_temp(dx)
**
**	Return TRUE if this is a valid temparary string address
*/
int
valid_temp(int dx)
{
	int i;

	for (i = 0; i < tsp; i++) {
		if (temp_stack[i] == dx) {
			return TRUE;
		}
	}
	return FALSE;
}

/*    Procedures  for  manipulating  the  symbol  table  */

/*
**	enter_mapper(name, value)
**
**	Map a C reserved word to a unique identifier in XPL.
**	This table also translates identifiers with illegal characters in C.
*/
void
enter_mapper(STRING *name, STRING *value)
{
	static STRING overflow = STR("Mapper table overflow");
	int i, j, k, l;

	if (top_mapper >= MAPPER_LIMIT) {
		error(&overflow, __LINE__, 1);
		return;
	}
	top_mapper = top_mapper + 1;
	i = name->_Length;
	j = mapper_index[i];
	for (l = 1; l <= top_mapper - j; l++) {
		k = top_mapper - l;
		mapper_name(k+1) = mapper_name(k);
		mapper_text(k+1) = mapper_text(k);
	}
	mapper_name(j) = *name;
	mapper_text(j) = *value;
	for (j = i; j < (sizeof(mapper_index) / sizeof(mapper_index[0])); j++) {
		mapper_index[j]++;
	}
}

/*
**	enter_keywords()
**
**	Enter the C language reserved words into the mapper table.
*/
void
enter_keywords(void)
{
	STRING *enter_keywords_text;
	static STRING xpl = STR("__xpl__");
	int i;

	enter_keywords_text = get_temp_descriptor();
	for (i = 0; mapper_list[i]._Length; i++) {
		CAT(enter_keywords_text, &xpl, &mapper_list[i]);
		enter_mapper(&mapper_list[i], enter_keywords_text);
	}
	release_temp_descriptor(1);
}

/*
**	squirrley_name(outstr, ident)
**
**	Map XPL identifiers to C squirrley name
**	A squirrley_name is a name that contains both a unique number and
**	most, it not all, of the original identifier.
*/
STRING *
squirrley_name(STRING *outstr, STRING *ident)
{
	int i, j, s1;
	STRING squirrley_name_text;
	unsigned char *str;
	char id[256];

	/* Mangle the identifier to cleanse the poison */
	sprintf(id, "__xpl_%d_", next_temp());
	j = strlen(id);
	s1 = ident->_Length;
	str = (unsigned char *) ident->_Address;
	for (i = 0; i < s1 && j < 254 && *str; i++) {
		if (poison[*str] == 0) {
			id[j++] = *str;
		}
		str++;
	}
	id[j] = '\0';
	squirrley_name_text._Length = j;
	squirrley_name_text._Address = id;
	return MOVE_TO_TOP(outstr, &squirrley_name_text);
}

/*
**	poison_identifier(ident)
**
**	Return TRUE if this identifier contains characters not permitted
**	in an identifer in the C language.
*/
int
poison_identifier(STRING *ident)
{
	int i, s1;
	unsigned char *str;

	s1 = ident->_Length;
	str = (unsigned char *) ident->_Address;
	for (i = 0; poison[*str++] == 0; i++) {
		if (i >= s1) {
			return FALSE;
		}
	}
	return TRUE;
}

/*
**	keyword_map(outstr, ident)
**
**	Map C keywords to XPL identifiers
*/
int
keyword_map(STRING *outstr, STRING *ident)
{
	int i, s1;

	s1 = ident->_Length;
	for (i = mapper_index[s1 - 1]; i < mapper_index[s1]; i++) {
		if (COMPARE(ident, &mapper_name(i)) == 0) {
			outstr->_Length = mapper_text(i)._Length;
			outstr->_Address = mapper_text(i)._Address;
			return TRUE;
		}
	}
	return FALSE;
}

/*
**	enter(name, type, location, line)
**
**	Enter a symbol in the symbol table
**	Return the index of the new entry
*/
int
enter(STRING *name, int type, int location, int line)
{
	static STRING duplicate = STR("duplicate declaration for: ");
	int i, k, mangle;

	idx = hasher(name);
	i = hash[idx];
	while (i >= procmark) {
		/*  Now test for name using indirect indices  */
		idcompares = idcompares + 1;
		if (COMPARE(name, &syt_name(i)) == 0) {
			k = syt_type[i];
			if (type == PROC_VOID && (k == FORWARD_LABEL || k == FORWARD_CALL)) {
				/* Define a function previously declared */
				syt_type[i] = type;
			} else
			if (type == DEFINED_LABEL && (k == FORWARD_LABEL || k == FORWARD_GOTO)) {
				/* Define a goto label previously declared */
				syt_type[i] = type;
			} else
			if (procmark + parct < i) {
				error(CAT(get_temp_descriptor(), &duplicate, name),
					__LINE__, 0);
				release_temp_descriptor(1);
			}
			declared_on_line[i] = line;
			return i;
		}
		i = ptr[i];         /*  Check next link  */
	}
#if DEBUG_HASHER
	if (debug_file) {
		char *stp = name->_Address;
		int j;

		k = name->_Length;
		for (j = 0; j < k; j++) {
			putc(*stp, debug_file);
			stp++;
		}
		putc('\n', debug_file);
	}
#endif
	ndecsy = ndecsy + 1;
	if (ndecsy > maxndecsy) {
		if (ndecsy > SYTSIZE) {
			error(&overflow, __LINE__, 1);
			ndecsy = ndecsy - 1;
			compiling = FALSE;
			OUTPUT(0, &abort_me);
		} else {
			maxndecsy = ndecsy;
		}
	}
	syt_name(ndecsy) = *name;
	syt_mapped(ndecsy) = syt_name(ndecsy);
	ptr[ndecsy] = hash[idx];	/* Link the new symbol into the hash table */
	hash[idx] = ndecsy;
	syt_type[ndecsy] = type;
	declared_on_line[ndecsy] = line;
	syt_count[ndecsy] = 0;
	syt_disp[ndecsy] = location;
	syt_dim[ndecsy] = 0;
	mangle = FALSE;
	if (type == UNDEF_ARG) {
		/* The mapping for parameters will be completed by tdeclare() */
		syt_mapped(ndecsy)._Length = 0;
	} else
	if (control['H'] & 1) {
		mangle = TRUE;
	} else
	if (poison_identifier(name)) {
		mangle = TRUE;
	} else
	if (global_scope(name)) {
		/* If this identifier has been promoted to the global scope
		   then all subsequent definitions must be mangled, even
		   global definitions.
		*/
		mangle = TRUE;
	} else {
		/* Even though XPL puts builtin functions in a outer scope
		   the declarations in the C code must live on the same scope
		   level.  So any builtin functions used as identifiers at
		   the top level must be mangled.
		*/
		while (i >= 0) {
			idcompares = idcompares + 1;
			if (COMPARE(name, &syt_name(i)) == 0) {
				/* Change the mapping to a unique identifier */
				mangle = TRUE;
				break;
			}
			i = ptr[i];         /*  Check next link  */
		}
	}
	if (mangle) {
		squirrley_name(&syt_mapped(ndecsy), name);
	} else
	if (syt_mapped(ndecsy)._Length != 0) {
		/* Map C keywords to XPL identifiers */
		keyword_map(&syt_mapped(ndecsy), name);
	}
	return ndecsy;
}

/*
**	enter_parameter(type, location, line)
**
**	Enter a parameter for the last function inserted into the symbol table
**	Return the index of the new entry
*/
int
enter_parameter(int type, int location, int line)
{
	ndecsy = ndecsy + 1;
	if (ndecsy > maxndecsy) {
		if (ndecsy > SYTSIZE) {
			error(&overflow, __LINE__, 1);
			ndecsy = ndecsy - 1;
			compiling = FALSE;
			OUTPUT(0, &abort_me);
		} else {
			maxndecsy = ndecsy;
		}
	}
	syt_name(ndecsy)._Length = 0;
	syt_type[ndecsy] = type;
	declared_on_line[ndecsy] = line;
	syt_count[ndecsy] = 0;
	syt_disp[ndecsy] = location;
	syt_dim[ndecsy] = 0;
	return ndecsy;
}


/*
**	id_lookup(p, default_type)
**
**	Looks up the identifier at p in the analysis stack in the
**	symbol table and initializes parse stack variables appropriately.
**	If the identifier is not found a symbol table entry is created
**	with the default_type.
*/
void
id_lookup(int p, int default_type)
{
	static STRING undeclared = STR("undeclared identifier: ");
	STRING *char_temp;
	int i;

	char_temp = &ps_name(p);
	i = hash[hasher(char_temp)];
	while (1) {
		if (i == -1) {
			if (default_type != FORWARD_LABEL) {
				error(CAT(get_temp_descriptor(), &undeclared, char_temp),
					__LINE__, 0);
				release_temp_descriptor(1);
			}
			i = enter(char_temp, default_type, 0, card_count);
			ps_symtab[p] = i;
			break;
		}
		idcompares = idcompares + 1;
		if (COMPARE(&syt_name(i), char_temp) == 0) {
			ps_symtab[p] = i;
			break;
		}
		i = ptr[i];
	}
	syt_count[i]++;  /* Count this references */
	ps_name(p) = syt_name(i);	/* conserve string space */
	ps_text(p) = syt_mapped(i);
	ps_type[p] = syt_type[i];
	ps_cnt[p] = 0;   /* Initialize subscript count  */
	ps_code[p] = 0;   /* Initialize code value */

	/* Set the builtin function type */
	if (syt_type[i] == SPECIAL) {
		ps_bin[p] = syt_disp[i];
	} else
	if (syt_type[i] == CHRTYPE) {
		ps_bin[p] = BIN_STRING;
	} else
	if (syt_type[i] == DESCRIPT) {
		ps_bin[p] = BIN_CHAR_POINTER;
	} else
	if (syt_type[i] < CHRTYPE || syt_type[i] == CHARFIXED) {
		if (syt_dim[i] == 0) {
			ps_bin[p] = BIN_SCALAR;
		} else {
			ps_bin[p] = BIN_ARRAY;
		}
	} else {
		ps_bin[p] = BIN_CALL;
	}
}

/*
**	enter_macro(name, value)
**
**	Enter a macro into the symbol table.
**
**	Putting the macros in the symbol table will give them scope.
*/
int
enter_macro(STRING *name, STRING *value)
{
	int sym;

	sym = enter(name, LITERAL, 0, card_count);
	if (sym >= 0) {
		syt_mapped(sym)._Length = value->_Length;
		syt_mapped(sym)._Address = value->_Address;
	}
	return sym;
}

/*
**	upper_case_macro()
**
**	Create a macro that will translate an uppercase <identifier> to lowercase.
*/
void
upper_case_macro(char *name)
{
	STRING *upper_case_macro_text, *upper_case_macro_string;
	int i;
	char data[32];

	for (i = 0; i < sizeof(data) - 1; i++) {
		if (name[i]) {
			data[i] = toupper(name[i]);
		} else {
			break;
		}
	}
	data[i] = '\0';
	upper_case_macro_text = get_temp_descriptor();
	upper_case_macro_string = get_temp_descriptor();
	enter_macro(MOVE_TO_TOP(upper_case_macro_text,
		c2x_string(upper_case_macro_text, data)),
		c2x_string(upper_case_macro_string, name));
	release_temp_descriptor(2);
}

/*
**	ignore_case_macro()
**
**	Create a set of macros that will translate uppercase keywords and builtin
**	function names to lowercase.  This implements the -I option.
*/
void
ignore_case_macro(void)
{
	static int did_this;
	int i;

	if (did_this) {
		/* Macros already created */
		return;
	}
	did_this = 1;
	for (i = 1; i <= NSY; i++) {
		if (isalpha(v[i][0])) {
			upper_case_macro(v[i]);
		}
	}

	for (i = 0; builtin_pointers[i].type; i++) {
		if (BYTE(&builtin_pointers[i].name, 0) != ' ') {
			upper_case_macro(builtin_pointers[i].name._Address);
		}
	}
	for (i = 0; builtin[i].type; i++) {
		if (BYTE(&builtin[i].name, 0) != ' ') {
			upper_case_macro(builtin[i].name._Address);
		}
	}
}

/*
**      allocate(int sym, int type, int dim, int array)
**
**	Allocates storage for the identifier at sym in the symbol table.
**	Set dimension to dim
*/
void
allocate(int sym, int type, int dim, int array)
{
	/* We only allocate string space.  Everything else is done by the C compiler */
	if (type == CHRTYPE) {
		syt_disp[sym] = newdsp;
		newdsp += dim;
	}
	syt_dim[sym] = array + dim - 1;
	syt_type[sym] = type;
}

/*
**      tdeclare(dim, array)
**
**      Allocates storage for identifiers in declarations
**
**	<type declaration> ::= <identifier specification> <type>
**	<type declaration> ::= <bound head> <expression> ) <type>
*/
void
tdeclare(int dim, int array)
{
	static STRING declare_static = STR("static ");
	STRING *tdeclare_string;
	int i, j = 0, sym;

	tdeclare_string = get_temp_descriptor();
	newdsp = dsp;
	ps_type[mp] = ps_type[sp];
	itype = ps_type[mp];
	if (proc_nest > 0) {
		/* Declare it static so it can be initialized and will retain its value */
		CAT(tdeclare_string, &declare_static, &dcltypes[itype]);
	} else {
		*tdeclare_string = dcltypes[itype];
	}
	ps_text(mp)._Length = 0;
	dim = dim + 1;		/* Actual number of items  */
	initial_count = dim;
	for(i = ps_code[mp]; i < casep; i++) {
		sym = casestack[i];      /* Symbol table pointer */
		if (syt_mapped(sym)._Length == 0) {
			squirrley_name(&syt_mapped(sym), &syt_name(sym));
		}
		if (sym >= procmark && sym < procmark + parct) {
			/* a parameter */
			; /* Nothing to do here */
		} else
		if (itype != CHRTYPE && itype != FORWARD_LABEL) {
			if (j != 0) {
				CAT(tdeclare_string, tdeclare_string, &comma_x1);
			}
			CAT(tdeclare_string, tdeclare_string, &syt_mapped(sym));
			if (array) {
				CAT(tdeclare_string, tdeclare_string, &left_bracket);
				CAT_INT(tdeclare_string, tdeclare_string, dim);
				CAT(tdeclare_string, tdeclare_string, &right_bracket);
			}
			j++;
			if (itype == CHARFIXED) {
				syt_disp[sym] = ps_value[sp];
				CAT(tdeclare_string, tdeclare_string, &left_bracket);
				CAT_INT(tdeclare_string, tdeclare_string, syt_disp[sym]);
				CAT(tdeclare_string, tdeclare_string, &right_bracket);
			}
		}
		allocate(sym, itype, dim, array);
	}
	if (j != 0 && itype != CHRTYPE && itype != FORWARD_LABEL) {
		static STRING start_initial = STR(" = {"); /* "}" */
		static STRING array_init =
			STR("Initial may not be used with identifier list");

		if (token == initial_token) {
			CAT(tdeclare_string, tdeclare_string, &start_initial);
			emit_declare(tdeclare_string, ps_line[mp]);
			nest_block++;
			if (ps_code[mp] + 1 != casep) {
				error(&array_init, __LINE__, 0);
				itype = 0;
				initial_count = 0;
			}
		} else {
			CAT(tdeclare_string, tdeclare_string, &semicolon);
			emit_declare(tdeclare_string, ps_line[mp]);
		}
	}
	casep = ps_code[mp];
	release_temp_descriptor(1);
}

/*
**	build_string(result, string)
**
**	Convert a string to a string.
**	Return the new descriptor
*/
STRING *
build_string(STRING *outstr, STRING *string)
{
	static STRING double_quote = STR("\"");
	static STRING back_slash = STR("\\");
	STRING *build_string_substr;
	int i, ch;

	build_string_substr = get_temp_descriptor();
	outstr->_Length = double_quote._Length;
	outstr->_Address = double_quote._Address;
	for (i = 0; i < string->_Length; i++) {
		ch = string->_Address[i] & 255;
		if (printing[ch]) {
			SUBSTR3(build_string_substr, string, i, 1);
			CAT(outstr, outstr, build_string_substr);
		} else {
			CAT(outstr, outstr, &back_slash);
			CAT_INT(outstr, outstr, ch >> 6);
			CAT_INT(outstr, outstr, (ch >> 3) & 7);
			CAT_INT(outstr, outstr, ch & 7);
		}
	}
	CAT(outstr, outstr, &double_quote);
	release_temp_descriptor(1);
	return outstr;
}

/*
**	truncate_constant(p, type)
**
**	Truncate the integer constant to match type so the C compiler does not
**	complain about it.
*/
void
truncate_constant(int p, int type)
{
	short s;
	int i;
	XPL_LONG l;

	s = (short) ps_value[p];
	i = (int) ps_value[p];
	l = ps_value[p];
	switch (type) {
	case BOOLEAN:
		ps_value[p] &= 1;
		break;
	case BYTETYPE:
		ps_value[p] &= 255;
		break;
	case HALFWORD:
		ps_value[p] = s;
		break;
	case FIXEDTYPE:
		if (sizeof(int) == 4) {
			ps_value[p] = i;
		}
		break;
	case DOUBLEWORD:
		if (sizeof_long == 4 && sizeof(int) == 4) {
			ps_value[p] = i;
		}
		break;
	default:
		return;
	}
	if (l != ps_value[p]) {
		ps_name(p)._Length = 0;  /* Clear the string value */
	}
}

/*
**	setinit(p, vp)
**
**	Places initial values into data area
*/
void
setinit(int p, int vp)
{
	static STRING bad_constant = STR("Illegal CONSTANT in INITIAL list");
	static STRING too_many = STR("Too many values in INITIAL list");
	STRING *setinit_string;
	int i;

	initial_count--;
	if (initial_count == -1) {
		error(&too_many, __LINE__, 0);
	}
	if (initial_count < 0) {
		return;
	}
	setinit_string = get_temp_descriptor();
	if (ps_type[vp] == SPECIAL && ps_bin[vp] == BIN_INLINE) {
		/* declare xxx fixed initial(inline('TEXT')) */
		if (ps_text(p)._Length > 0 &&
			ps_text(p)._Length + ps_text(vp)._Length > 80) {
			emit_declare(&ps_text(p), ps_line[vp]);
			ps_text(p)._Length = 0;
		}
		/* No type conversion */
		CAT(&ps_text(p), &ps_text(p), &ps_text(vp));
		if (initial_count > 0) {
			CAT(&ps_text(p), &ps_text(p), &comma_x1);
		}
	} else
	if (itype == CHARFIXED) {
		if (ps_type[vp] == CONSTANT ) {
			if (ps_name(vp)._Length == 0) {
				/* Convert to string */
				DECIMAL(&ps_name(vp), ps_value[vp]);
			}
		} else
		if (ps_type[vp] != STRINGCON) {
			error(&bad_constant, __LINE__, 0);
			ps_name(vp) = null_terminator;  /* Place holder */
		}
		build_string(setinit_string, &ps_name(vp));
		if (ps_text(p)._Length + setinit_string->_Length > 80 &&
				ps_text(p)._Length > 0) {
			emit_declare(&ps_text(p), ps_line[vp]);
			ps_text(p)._Length = 0;
		}
		CAT(&ps_text(p), &ps_text(p), setinit_string);
		if (initial_count > 0) {
			CAT(&ps_text(p), &ps_text(p), &comma_x1);
		}
	} else
	if (itype == CHRTYPE) {
		if (ps_type[vp] == CONSTANT ) {
			if (ps_name(vp)._Length == 0) {
				/* Convert to string */
				DECIMAL(&ps_name(vp), ps_value[vp]);
			}
		} else
		if (ps_type[vp] != STRINGCON) {
			error(&bad_constant, __LINE__, 0);
			ps_name(vp) = null_terminator;  /* Place holder */
		}
		i = string_constant(&ps_name(vp));
		enter_init_string(dsp++, i);
	} else
	if (ps_type[vp] != CONSTANT) {
		error(&bad_constant, __LINE__, 0);
	} else {
		/* Truncate the constant to fit the destination */
		truncate_constant(vp, itype);
		if (ps_text(p)._Length > 70) {
			emit_declare(&ps_text(p), ps_line[vp]);
			ps_text(p)._Length = 0;
		}
		DECIMAL(&ps_name(vp), ps_value[vp]);
		CAT(&ps_text(p), &ps_text(p), &ps_name(vp));
		if (initial_count > 0) {
			CAT(&ps_text(p), &ps_text(p), &comma_x1);
		}
	}
	release_temp_descriptor(1);
}

/*
**	movestacks(from, to)
**
**	Move all the compiler stacks down from f to t
*/
void
movestacks(int f, int t)
{
	ps_type[t] = ps_type[f];    ps_name(t) = ps_name(f);
	ps_line[t] = ps_line[f];    ps_value[t] = ps_value[f];
	ps_cnt[t] = ps_cnt[f];      ps_symtab[t] = ps_symtab[f];
	ps_text(t) = ps_text(f);    ps_bin[t] = ps_bin[f];
	ps_code[t] = ps_code[f];
}

/*
**	set_type_of_constant(p)
**
**	Set ps_type[] for the constant at p.
**	The result could be BOOLEAN, BYTETYPE, HALFWORD, FIXEDTYPE, or DOUBLEWORD.
*/
void
set_type_of_constant(int p)
{
	short s;
	int i;

	s = (short) ps_value[p];
	i = (int) ps_value[p];
	if (ps_value[p] == 0 || ps_value[p] == 1) {
		ps_type[p] = BOOLEAN;
	} else
	if (ps_value[p] >= 0 && ps_value[p] < 256) {
		ps_type[p] = BYTETYPE;
	} else
	if (ps_value[p] == s) {
		ps_type[p] = HALFWORD;
	} else
	if (ps_value[p] == i && sizeof(int) == 4) {
		ps_type[p] = FIXEDTYPE;
	} else
	if (sizeof_long == 4) {
		if (sizeof(int) == 4) {
			/* Truncate */
			ps_value[p] = i;
		}
		ps_type[p] = FIXEDTYPE;
	} else {
		ps_type[p] = DOUBLEWORD;
	}
}

/*
**	forceaccumulator(int p, int target_type, int paren)
**
**	paren is set TRUE if MOPAR needs to add parentheses.
**	Traget_type is used for high order truncation of constants.
**
**	Force the object at p into the ps_text[] string array.
*/
void
forceaccumulator(int p, int tt, int paren)
{
	static STRING force_type = STR("forceaccumulator type ");
	static STRING force_error = STR(" error");
	static STRING get_byte = STR("__xpl_get_byte");
	static STRING cast_long = STR("((XPL_LONG) ");
	static STRING proc_void =
		STR("A procedure that does not return a value is used in an expression");
	STRING *forceaccumulator_text;

	count_force = count_force + 1;
	forceaccumulator_text = get_temp_descriptor();

	switch(ps_type[p]) {
	case CONSTANT:
		truncate_constant(p, tt);
		if (ps_value[p] < 0) {
			CAT_INT(forceaccumulator_text, &open_paren, ps_value[p]);
			CAT(&ps_text(p), forceaccumulator_text, &close_paren);
		} else {
			us_decimal(&ps_text(p), ps_value[p]);
		}
		set_type_of_constant(p);
		ps_bin[p] = BIN_ACCUMULATOR;
		break;
	case BOOLEAN:
	case BYTETYPE:
	case HALFWORD:
	case FIXEDTYPE:
	case DOUBLEWORD:
		if (paren && ps_bin[p] == BIN_MOPAR) {
			CAT(forceaccumulator_text, &open_paren, &ps_text(p));
			CAT(&ps_text(p), forceaccumulator_text, &close_paren);
		}
		ps_bin[p] = BIN_ACCUMULATOR;
		break;
	case PROC_VOID:
		error(&proc_void, __LINE__, 0);
		ps_bin[p] = BIN_ACCUMULATOR;
		ps_type[p] = FIXEDTYPE;
		break;
	case PROC_BIT1:
	case PROC_BIT8:
	case PROC_BIT16:
	case PROC_BIT32:
	case PROC_BIT64:
		ps_bin[p] = BIN_ACCUMULATOR;
		ps_type[p] = ps_type[p] - PROC_VOID;
		break;
	case CHAR_PROC_TYPE:
		/* Example: xprintf('%s', hex(<expression>)); */
		CAT_INT(forceaccumulator_text, &wrapper_descriptor, get_temp());
		CAT(forceaccumulator_text, forceaccumulator_text, &bracket_comma_x1);
		CAT(forceaccumulator_text, forceaccumulator_text, &ps_text(p));
		CAT(&ps_text(p), forceaccumulator_text, &close_paren);
		ps_bin[p] = BIN_ACCUMULATOR;
		ps_type[p] = DESCRIPT;
		break;
	case FORWARD_CALL:
		/* Nothing to do */
		break;
	case CHRTYPE:
	case DESCRIPT:
	case CHARFIXED:
		/* Function call parameters might come here */
		break;
	case POINTER_TYPE:
		CAT(forceaccumulator_text, &cast_long, &ps_text(p));
		CAT(&ps_text(p), forceaccumulator_text, &close_paren);
		ps_bin[p] = BIN_ACCUMULATOR;
		if (sizeof_long == 8) {
			ps_type[p] = DOUBLEWORD;
		} else {
			ps_type[p] = FIXEDTYPE;
		}
		break;
	case SPECIAL:
		/* Finish builtin functions on the right of the assignment operator */
		if (ps_bin[p] == BIN_BYTE) {
			CAT(forceaccumulator_text, &get_byte, &ps_text(p));
			CAT(&ps_text(p), forceaccumulator_text, &close_paren);
			ps_type[p] = BYTETYPE;
			ps_bin[p] = BIN_ACCUMULATOR;
		} else
		if (ps_bin[p] == BIN_INLINE) {
			/* x = inline('something'); */
			ps_type[p] = BYTETYPE;
			ps_bin[p] = BIN_ACCUMULATOR;
		} else {
			error(&unimplemented, __LINE__, 1);
			ps_text(p) = oops;
		}
		break;
	default:
		CAT_INT(forceaccumulator_text, &force_type, ps_type[p]);
		CAT(forceaccumulator_text, forceaccumulator_text, &force_error);
		error(forceaccumulator_text, __LINE__, 1);
		ps_text(p) = oops;
		break;
	}
	release_temp_descriptor(1);
}

/*
**	forcedescript(int p, int target_type)
**
**	Get a descriptor for the operand p
*/
void
forcedescript(int p, int tt)
{
	STRING *forcedescript_text;
	int type;

	count_desc = count_desc + 1;
	forcedescript_text = get_temp_descriptor();
	if (ps_type[p] == CONSTANT) {
		if (ps_name(p)._Length == 0) {
			/* Convert an integer constant to a string constant */
			DECIMAL(&ps_name(p), ps_value[p]);
		}
		ps_type[p] = STRINGCON;
	}
	if (ps_type[p] == STRINGCON) {
		/* Convert a string constant to an identifier */
		ps_type[p] = CHRTYPE;
		if (ps_text(p)._Length == 0) {
			temp_name(&ps_text(p), string_constant(&ps_name(p)), 0);
		} else {
			CAT(forcedescript_text, &open_paren, &ps_text(p));
			temp_name(&ps_text(p), string_constant(&ps_name(p)), 0);
			CAT(forcedescript_text, forcedescript_text, &comma_x1);
			if (tt == DESCRIPT) {
				CAT(forcedescript_text, forcedescript_text, &ampersand);
				ps_type[p] = DESCRIPT;
			}
			CAT(forcedescript_text, forcedescript_text, &ps_text(p));
			CAT(&ps_text(p), forcedescript_text, &close_paren);
		}
		ps_bin[p] = BIN_ACCUMULATOR;
	}
	if (ps_type[p] == CHARFIXED) {
		/* Convert to a descriptor */
		/* __xpl_get_charfixed(&descriptor[temp], length(p), address(p)) */
		CAT_INT(forcedescript_text, &get_charfixed_descriptor, get_temp());
		CAT(forcedescript_text, forcedescript_text, &bracket_comma_x1);
		CAT_INT(forcedescript_text, forcedescript_text, syt_disp[ps_symtab[p]]);
		CAT(forcedescript_text, forcedescript_text, &comma_x1);
		CAT(forcedescript_text, forcedescript_text, &ps_text(p));
		CAT(&ps_text(p), forcedescript_text, &close_paren);
		release_temp_descriptor(1);
		ps_type[p] = DESCRIPT;
		ps_bin[p] = BIN_ACCUMULATOR;
		return;
	}
	if (tt == DESCRIPT && ps_type[p] == CHRTYPE) {
		CAT(&ps_text(p), &ampersand, &ps_text(p));
		release_temp_descriptor(1);
		ps_type[p] = DESCRIPT;
		ps_bin[p] = BIN_ACCUMULATOR;
		return;
	}
	type = ps_type[p];
	if (type == DESCRIPT || type == CHRTYPE) {
		release_temp_descriptor(1);
		ps_bin[p] = BIN_ACCUMULATOR;
		return;
	}
	if (type == CHAR_PROC_TYPE) {
		CAT_INT(forcedescript_text, &wrapper_descriptor, get_temp());
		CAT(forcedescript_text, forcedescript_text, &bracket_comma_x1);
		CAT(forcedescript_text, forcedescript_text, &ps_text(p));
		CAT(&ps_text(p), forcedescript_text, &close_paren);
		release_temp_descriptor(1);
		ps_type[p] = DESCRIPT;
		ps_bin[p] = BIN_ACCUMULATOR;
		return;
	}
	/* Convert to a string at runtime */
	forceaccumulator(p, DESCRIPT, FALSE);
	/* __xpl_decimal(&descriptor[temp], ps_text) */
	CAT_INT(forcedescript_text, &decimal_descriptor, get_temp());
	CAT(forcedescript_text, forcedescript_text, &bracket_comma_x1);
	CAT(forcedescript_text, forcedescript_text, &ps_text(p));
	CAT(&ps_text(p), forcedescript_text, &close_paren);
	release_temp_descriptor(1);
	ps_type[p] = DESCRIPT;
	ps_bin[p] = BIN_ACCUMULATOR;
}

/*
**	the_big_easy(p)
**
**	Accept any value, springs or integers.
*/
void
the_big_easy(int p)
{
	int t;

	t = ps_type[p];
	if (t == STRINGCON || t == CHRTYPE || t == DESCRIPT) {
		forcedescript(p, DESCRIPT);
	} else
	if (t != POINTER_TYPE) {
		forceaccumulator(p, DOUBLEWORD, FALSE);
	}
}

/*
**	createdescript(int p)
**
**	Create a descriptor for the operand p.  This function is used with
**	fixed length character strings to create a full sized descriptor that
**	does not derive the length from the current string.
*/
void
createdescript(int p)
{
	STRING *createdescript_text;

	createdescript_text = get_temp_descriptor();
	if (ps_type[p] == CHARFIXED) {	/* Create a descriptor */
		/* __xpl_build_descriptor(&descriptor[temp], length(p), address(p)) */
		CAT_INT(createdescript_text, &build_descriptor_descriptor, get_temp());
		CAT(createdescript_text, createdescript_text, &bracket_comma_x1);
		CAT_INT(createdescript_text, createdescript_text, syt_disp[ps_symtab[p]]);
		CAT(createdescript_text, createdescript_text, &comma_x1);
		CAT(createdescript_text, createdescript_text, &ps_text(p));
		CAT(&ps_text(p), createdescript_text, &close_paren);
		ps_type[p] = DESCRIPT;
		ps_bin[p] = BIN_ACCUMULATOR;
	} else {
		forcedescript(p, DESCRIPT);
	}
	release_temp_descriptor(1);
}

/*
**	stuff_parameter(last_parameter)
**
**	Do type conversion on parameters
*/
void
stuff_parameter(int last_parameter)
{
	static STRING too_many = STR("Too many actual parameters");
	static STRING calling_a_goto =
		STR("<identifier> previously used as target of goto");
	static STRING illegal_string = STR("Illegal use of string descriptor");
	int t, sym;

	sym = ps_symtab[mp];
	if (syt_type[sym] == DEFINED_LABEL) {
		/* Does this code ever get executed? */
		syt_type[sym] = PROC_VOID;
		ps_type[mp] = PROC_VOID;
	}
	if (syt_type[sym] == FORWARD_LABEL) {
		syt_type[sym] = FORWARD_CALL;
		ps_type[mp] = FORWARD_CALL;
	}
	if (syt_type[sym] == FORWARD_GOTO) {
		error(&calling_a_goto, __LINE__, 1);
		return;
	}
	if (syt_type[sym] == FORWARD_CALL) {
		/* No prototype given.  Good Luck. */
		the_big_easy(mpp1);
	} else {
		sym = ps_symtab[mp] + ps_cnt[mp];
		if (syt_name(sym)._Length > 0) {
			error(&too_many, __LINE__, 1);
			ps_cnt[mp]--;
			return;
		}
		t = syt_type[sym];
		if (t == CHRTYPE || t == DESCRIPT || t == CHARFIXED) {
			forcedescript(mpp1, DESCRIPT);
		} else {
			forceaccumulator(mpp1, t, FALSE);
			if (ps_type[mpp1] == CHRTYPE || ps_type[mpp1] == DESCRIPT) {
				/* Assign a string to an integer */
				error(&illegal_string, __LINE__, 0);
			}
			if (t < ps_type[mpp1] && (control['Y'] & 1) == 1) {
				STRING *stuff_parameter_text;

				stuff_parameter_text = get_temp_descriptor();
				CAT(stuff_parameter_text, &truncation, &syt_mapped(sym));
				error(stuff_parameter_text, __LINE__, 0);
				release_temp_descriptor(1);
			}
		}
		if (last_parameter) {
			/* Fill in any remaining parameters with zero */
			for (sym++; syt_name(sym)._Length == 0 && sym <= ndecsy; sym++) {
				CAT(&ps_text(mpp1), &ps_text(mpp1), &comma_zero);
			}
		}
	}
	CAT(&ps_text(mp), &ps_text(mp), &ps_text(mpp1));
}

/*
**	file_builtin(f, b)
**
**	Handle the builtin function file()
**	f is the stack pointer for file().
**	b is the stack pointer for the buffer.
*/
void
file_builtin(int f, int b)
{
	static STRING bad_buffer = STR("Target of file must be an array");
	static STRING comma_long_sizeof = STR(", (XPL_UNSIGNED_LONG) sizeof(");
	STRING *file_builtin_text;
	int sym;

	if (ps_type[b] < BOOLEAN || ps_type[b] > DOUBLEWORD || ps_bin[b] != BIN_ARRAY) {
		error(&bad_buffer, __LINE__, 0);
		return;
	}
	file_builtin_text = get_temp_descriptor();
	sym = ps_symtab[b];
	CAT(file_builtin_text, &ps_text(f), &syt_mapped(sym));
	CAT(file_builtin_text, file_builtin_text, &comma_long_sizeof);
	CAT(file_builtin_text, file_builtin_text, &syt_mapped(sym));
	CAT(&ps_text(f), file_builtin_text, &close_close);
	release_temp_descriptor(1);
}

/*
**	store_direct(f, t)
**
**	Modify the emitted code to store the string directly into the
**	destination rather than a temp descriptor.
*/
int
store_direct(int f, int t)
{
	int i, j, len, value, chr;
	STRING *store_direct_text;

	store_direct_text = get_temp_descriptor();
	len = ps_text(f)._Length;
	for (i = 0; ; i++) {
		j = suproc[i]._Length;
		if (j == 0) {
			release_temp_descriptor(1);
			return FALSE;
		}
		if (j < len) {
			SUBSTR3(store_direct_text, &ps_text(f), 0, j);
			if (COMPARE(&suproc[i], store_direct_text) == 0) {
				break;
			}
		}
	}
	value = 0;
	for ( ; ; j++) {
		chr = BYTE(&ps_text(f), j);
		if (chr == ']') {
			break;
		}
		if (chr >= '0' && chr <= '9') {
			value = (value * 10) + chr - '0';
		} else {
			release_temp_descriptor(1);
			return FALSE;
		}
	}
	if (valid_temp(value) == 0) {
		release_temp_descriptor(1);
		return FALSE;
	}
	SUBSTR3(store_direct_text, &suproc[i], 0, suproc[i]._Length - 11);
	CAT(store_direct_text, store_direct_text, &ps_text(t));
	SUBSTR2(&ps_text(f), &ps_text(f), j + 1);
	CAT(&ps_text(t), store_direct_text, &ps_text(f));
	release_temp_descriptor(1);
	return TRUE;
}

/*
**	genstore(int mp, int sp)
**
**	Emit code for a Store operation
**
**	<assignment> ::= <variable> <replace> <expression>
**	<assignment> ::= <left part> <assignment>
*/
void
genstore(int mp, int sp)
{
	static STRING put_byte = STR("__xpl_put_byte");
	static STRING store_type = STR("genstore type ");
	static STRING store_error = STR(" error");
	static STRING out_call = STR("__xpl_output(");
	static STRING out_zero = STR("__xpl_output(0, ");
	static STRING wrapper = STR("__xpl_wrapper(");
	static STRING wrapper_and = STR("__xpl_wrapper(&");
	static STRING read_file = STR("__xpl_read_file");
	static STRING write_file = STR("__xpl_write_file");
	static STRING length_equals = STR("._Length = ");
	static STRING length_equals_zero = STR("._Length = 0");
	static STRING put_charfixed = STR("__xpl_put_charfixed(");
	STRING *genstore_text;

	count_store++;
	if (ps_type[sp] == SPECIAL && ps_bin[sp] == BIN_FILE) {  /* read */
		/* <variable> = file(i, j) */
		CAT(&ps_text(sp), &read_file, &ps_text(sp));
		file_builtin(sp, mp);
		ps_text(mp) = ps_text(sp);
		ps_type[mp] = VOID_TYPE;
		return;
	}
	genstore_text = get_temp_descriptor();
	switch(ps_type[mp]) {
	case BOOLEAN:
	case BYTETYPE:
	case HALFWORD:
	case FIXEDTYPE:
	case DOUBLEWORD:
		/* Integer <identifier> */
		forceaccumulator(sp, ps_type[mp], FALSE);
		if (ps_type[mp] < ps_type[sp] && (control['Y'] & 1) == 1) {
			CAT(genstore_text, &truncation, &syt_name(ps_symtab[mp]));
			error(genstore_text, __LINE__, 0);
		}
		break;
	case CHRTYPE:
		if (ps_type[sp] == STRINGCON && ps_name(sp)._Length == 0) {
			/* Set the variable to the NULL string */
			if (ps_text(sp)._Length == 0) {
				CAT(&ps_text(mp), &ps_text(mp), &length_equals_zero);
			} else {
				CAT(genstore_text, &ps_text(mp), &length_equals);
				CAT(&ps_text(mp), genstore_text, &ps_text(sp));
			}
			ps_type[mp] = STRINGCON;
			ps_name(mp)._Length = 0;
			release_temp_descriptor(1);
			return;
		}
		forcedescript(sp, DESCRIPT);
		if (store_direct(sp, mp)) {
			ps_type[mp] = DESCRIPT;
			release_temp_descriptor(1);
			return;
		}
		CAT(genstore_text, &wrapper_and, &ps_text(mp));
		CAT(genstore_text, genstore_text, &comma_x1);
		CAT(genstore_text, genstore_text, &ps_text(sp));
		CAT(&ps_text(mp), genstore_text, &close_paren);
		release_temp_descriptor(1);
		ps_type[mp] = DESCRIPT;
		return;
	case CHARFIXED:
		forcedescript(sp, DESCRIPT);

		/* __xpl_put_charfixed(length(mp), ps_text(mp), ps_text(sp)) */
		CAT_INT(genstore_text, &put_charfixed, syt_disp[ps_symtab[mp]]);
		CAT(genstore_text, genstore_text, &comma_x1);
		CAT(genstore_text, genstore_text, &ps_text(mp));
		CAT(genstore_text, genstore_text, &comma_x1);
		CAT(genstore_text, genstore_text, &ps_text(sp));
		CAT(&ps_text(mp), genstore_text, &close_paren);
		release_temp_descriptor(1);
		ps_type[mp] = DESCRIPT;
		return;
	case DESCRIPT:
		forcedescript(sp, DESCRIPT);
		CAT(genstore_text, &wrapper, &ps_text(mp));
		CAT(genstore_text, genstore_text, &comma_x1);
		CAT(genstore_text, genstore_text, &ps_text(sp));
		CAT(&ps_text(mp), genstore_text, &close_paren);
		release_temp_descriptor(1);
		ps_type[mp] = DESCRIPT;
		return;
	case SPECIAL:
		/* Finish builtin functions on the left of the assignment operator */
		if (ps_bin[mp] == BIN_BYTE) {
			forceaccumulator(sp, FIXEDTYPE, FALSE);
			CAT(genstore_text, &put_byte, &ps_text(mp));
			CAT(genstore_text, genstore_text, &comma_x1);
			CAT(genstore_text, genstore_text, &ps_text(sp));
			CAT(&ps_text(mp), genstore_text, &close_paren);
			release_temp_descriptor(1);
			ps_type[mp] = BYTETYPE;
			ps_bin[mp] = BIN_ACCUMULATOR;
			return;
		} else
		if (ps_bin[mp] == BIN_INLINE) {
			/* inline('something') = value; */
			forceaccumulator(sp, DOUBLEWORD, FALSE);
		} else
		if (ps_bin[mp] == BIN_OUTPUT) {
			forcedescript(sp, DESCRIPT);
			if (ps_cnt[mp] == 0) {
				CAT(genstore_text, &out_zero, &ps_text(sp));
			} else {
				CAT(genstore_text, &out_call, &ps_text(mp));
				CAT(genstore_text, genstore_text, &comma_x1);
				CAT(genstore_text, genstore_text, &ps_text(sp));
			}
			CAT(&ps_text(mp), genstore_text, &close_paren);
			release_temp_descriptor(1);
			ps_type[mp] = DESCRIPT;
			return;
		} else
		if (ps_bin[mp] == BIN_FILE) {  /* Write file */
			/* file(i, j) = <variable> */
			CAT(&ps_text(mp), &write_file, &ps_text(mp));
			file_builtin(mp, sp);
			ps_type[mp] = VOID_TYPE;
			release_temp_descriptor(1);
			return;
		} else {
			error(&unimplemented, __LINE__, 1);
			ps_text(mp) = oops;
		}
		break;
	default:
		CAT_INT(genstore_text, &store_type, ps_type[mp]);
		CAT(genstore_text, genstore_text, &store_error);
		error(genstore_text, __LINE__, 1);
		ps_text(mp) = oops;
		break;
	}
	CAT(genstore_text, &ps_text(mp), &x1_equal_x1);
	if (ps_type[sp] == DESCRIPT) {
		CAT(genstore_text, genstore_text, &star);
	}
	CAT(&ps_text(mp), genstore_text, &ps_text(sp));
	ps_type[mp] = ps_type[sp];
	ps_bin[mp] = BIN_ACCUMULATOR;
	release_temp_descriptor(1);
}

/*
**	character_string(p)
**
**	Return TRUE if the value on the parse stack at p is a character string.
*/
int
character_string(int p)
{
	return (ps_type[p] == STRINGCON || ps_type[p] == DESCRIPT ||
		ps_type[p] == CHRTYPE || ps_type[p] == CHARFIXED);
}

enum {AOP_EQ, AOP_NE, AOP_LT, AOP_GE, AOP_GT, AOP_LE,
	AOP_LAST_STRING,   /* AOP_LAST_STRING is the last valid string operation */
	AOP_OR, AOP_XOR, AOP_AND, AOP_ADD, AOP_SUBTRACT,
	AOP_MULTIPLY, AOP_DIVIDE, AOP_MOD, AOP_SHL, AOP_SHR
};

/* String definitions for the AOP enumeration.  These must match */
static STRING aop_string[] = {
	STR(" == "),
	STR(" != "),
	STR(" < "),
	STR(" >= "),
	STR(" > "),
	STR(" <= "),
	STR_NULL,	/* AOP_LAST_STRING */
	STR(" | "),
	STR(" ^ "),
	STR(" & "),
	STR(" + "),
	STR(" - "),
	STR(" * "),
	STR(" / "),
	STR(" % "),
	STR(" << "),
	STR(" >> "),
};

/*
**	arithemit(mp, op, sp)
**
**	Emit code for an Arithmetic operation.
*/
void
arithemit(int mp, int op, int sp)
{
	static STRING bad_op = STR("Arithmetic with a string descriptor");
	static STRING div_zero = STR("Divide by zero");
	STRING *arithemit_text;
	XPL_LONG vsp, vmp;

	count_arith = count_arith + 1;
	arithemit_text = get_temp_descriptor();
	if (ps_type[sp] == CONSTANT && ps_type[mp] == CONSTANT) {
		vsp = ps_value[sp];
		vmp = ps_value[mp];
		switch (op) {
		case AOP_EQ:
			vmp = (vmp == vsp) ? 1 : 0;
			break;
		case AOP_NE:
			vmp = (vmp != vsp) ? 1 : 0;
			break;
		case AOP_LT:
			vmp = (vmp < vsp) ? 1 : 0;
			break;
		case AOP_GE:
			vmp = (vmp >= vsp) ? 1 : 0;
			break;
		case AOP_GT:
			vmp = (vmp > vsp) ? 1 : 0;
			break;
		case AOP_LE:
			vmp = (vmp <= vsp) ? 1 : 0;
			break;
		case AOP_OR:
			vmp = vmp | vsp;
			break;
		case AOP_XOR:
			vmp = vmp ^ vsp;
			break;
		case AOP_AND:
			vmp = vmp & vsp;
			break;
		case AOP_ADD:
			vmp = vmp + vsp;
			break;
		case AOP_SUBTRACT:
			vmp = vmp - vsp;
			break;
		case AOP_MULTIPLY:
			vmp = vmp * vsp;
			break;
		case AOP_DIVIDE:
			if (vsp == 0) {
				error(&div_zero, __LINE__, 0);
			} else {
				vmp = vmp / vsp;
			}
			break;
		case AOP_MOD:
			if (vsp == 0) {
				error(&div_zero, __LINE__, 0);
			} else {
				vmp = vmp % vsp;
			}
			break;
		case AOP_SHL:
			vmp = vmp << vsp;
			break;
		case AOP_SHR:
			set_type_of_constant(mp);
			switch (ps_type[mp]) {
			case BOOLEAN:
			case BYTETYPE:
				vmp = (unsigned char) vmp >> vsp;
				break;
			case HALFWORD:
				vmp = (unsigned short) vmp >> vsp;
				break;
			case FIXEDTYPE:
				if (sizeof(int) == 4) {
					vmp = (unsigned int) vmp >> vsp;
				} else {
					vmp = (XPL_UNSIGNED_LONG) vmp >> vsp;
				}
				break;
			case DOUBLEWORD:
				vmp = (XPL_UNSIGNED_LONG) vmp >> vsp;
				break;
			default:
				break;
			}
			ps_type[mp] = CONSTANT;
			break;
		default:
			error(&oops, __LINE__, 1);
			break;
		}
		ps_value[mp] = vmp;
		ps_name(mp)._Length = 0;  /* Not just housekeeping */
	} else
	if ((op < AOP_LAST_STRING) && (character_string(mp) || character_string(sp))) {
		static STRING compare = STR("__xpl_compare(");  /* ")" */
		static STRING zero = STR("0");

		/* Must be compare */
		forcedescript(mp, DESCRIPT);
		forcedescript(sp, DESCRIPT);
		CAT(arithemit_text, &compare, &ps_text(mp));
		CAT(arithemit_text, arithemit_text, &comma_x1);
		CAT(arithemit_text, arithemit_text, &ps_text(sp));
		CAT(arithemit_text, arithemit_text, &close_paren);
		CAT(arithemit_text, arithemit_text, &aop_string[op]);
		CAT(&ps_text(mp), arithemit_text, &zero);
		ps_type[mp] = BOOLEAN;
		ps_bin[mp] = BIN_MOPAR;
	} else
	if (op == AOP_SHR) {	/* XPL uses a logical right shift */
		static STRING unsigned_x1 = STR("(unsigned ");
		static STRING cast_unsigned = STR("(XPL_UNSIGNED_LONG)");
		static STRING bad_shift = STR("Bad operand to SHR()");

		forceaccumulator(mp, DOUBLEWORD, TRUE);
		forceaccumulator(sp, DOUBLEWORD, TRUE);
		if (character_string(mp) || character_string(sp)) {
			error(&bad_op, __LINE__, 0);
			release_temp_descriptor(1);
			return;
		}
		if (ps_type[mp] < BOOLEAN || ps_type[mp] > DOUBLEWORD) {
			error(&bad_shift, __LINE__, 0);
			release_temp_descriptor(1);
			return;
		}
		/* (unsigned type) ps_text(mp) >> ps_text(sp) */
		if (ps_type[mp] == BYTETYPE) {
			/* This is the only unsigned type */
			arithemit_text->_Length = 0;
		} else
		if (ps_type[mp] == DOUBLEWORD) {
			*arithemit_text = cast_unsigned;
		} else {
			CAT(arithemit_text, &unsigned_x1, &dcltypes[ps_type[mp]]);
			CAT(arithemit_text, arithemit_text, &close_paren);
		}
		CAT(arithemit_text, arithemit_text, &ps_text(mp));
		CAT(arithemit_text, arithemit_text, &aop_string[op]);
		CAT(&ps_text(mp), arithemit_text, &ps_text(sp));
		ps_text(sp)._Length = 0;
		ps_bin[mp] = BIN_MOPAR;
	} else {
		forceaccumulator(mp, DOUBLEWORD, TRUE);
		forceaccumulator(sp, DOUBLEWORD, TRUE);
		if (character_string(mp) || character_string(sp)) {
			error(&bad_op, __LINE__, 0);
			release_temp_descriptor(1);
			return;
		}
		CAT(arithemit_text, &ps_text(mp), &aop_string[op]);
		CAT(&ps_text(mp), arithemit_text, &ps_text(sp));
		ps_text(sp)._Length = 0;
		ps_bin[mp] = BIN_MOPAR;
		if (op < AOP_LAST_STRING) {
			ps_type[mp] = BOOLEAN;
		} else
		if (op == AOP_AND) {
			/* Types get smaller with the AND operator */
			if (ps_type[mp] > ps_type[sp] && ps_type[sp] <= BYTETYPE) {
				ps_type[mp] = ps_type[sp];
			}
		} else
		if (ps_type[mp] < ps_type[sp] && ps_type[sp] <= DOUBLEWORD) {
			ps_type[mp] = ps_type[sp];
		}
	}
	release_temp_descriptor(1);
}

/*
**	set_limit()
**
**	Set the upper limit for DO loops
**
**	<iteration control> ::= to <expression>
**	<iteration control> ::= to <expression> by <expression>
*/
void
set_limit(void)
{
	movestacks(mpp1, mp);
	if (ps_type[mp] != CONSTANT) {
		forceaccumulator(mp, DOUBLEWORD, TRUE);
	}
}

/*
**	subscripted(variable)
**
**	Return TRUE if the variable is subscripted by something other than
**	a constant.
*/
int
subscripted(STRING *var)
{
	int ch, i;

	for (i = 0; BYTE(var, i) != '['; i++) {
		if (i + 1 >= var->_Length) {
			return FALSE;
		}
	}
	while (++i < var->_Length) {
		ch = BYTE(var, i);
		if (ch == ']') {
			i++;
			break;
		}
		if (ch < '0' || ch > '9') {
			return TRUE;
		}
	}
	if (i >= var->_Length) {
		return FALSE;
	}
	return TRUE;
}

/*
**	for_loop()
**
**	Generate the code for the start of a DO loop
**
**	<step definition> ::= <variable> <replace> <expression> <iteration control>
**
**	Parse stack location zero is used for the step.
*/
void
for_loop(void)
{
	static STRING bad_subscript = STR("subscripted DO variable");
	static STRING bad_string = STR("Strings may not be DO variables");
	static STRING for_head = STR("for (");
	static STRING step = STR(" += ");
	STRING *for_loop_string, *for_loop_temp, *for_loop_substr;
	int mpp2, sym, aop;
	char *symtype;

	if (ps_type[mp] < BOOLEAN || ps_type[mp] > DOUBLEWORD) {
		error(&bad_string, __LINE__, 1);
		return;
	}
	for_loop_string = get_temp_descriptor();
	for_loop_temp = get_temp_descriptor();
	for_loop_substr = get_temp_descriptor();
	sym = ps_symtab[mp];
	if (syt_dim[sym] != 0) {
		/* A constant subscript is permitted */
		if (subscripted(&ps_text(mp))) {
			error(&bad_subscript, __LINE__, 0);
		}
	}
	symtype = dcltypes[syt_type[sym]]._Address;
	mpp2 = mp + 2;
	forceaccumulator(mpp2, ps_type[mp], TRUE);
	CAT(for_loop_string, &for_head, &ps_text(mp));
	CAT(for_loop_string, for_loop_string, &x1_equal_x1);
	CAT(for_loop_string, for_loop_string, &ps_text(mpp2));
	if (ps_type[sp] != CONSTANT) {
		temp_name(for_loop_temp, next_temp(), 1);
		SUBSTR3(for_loop_substr, for_loop_temp, 0, for_loop_temp->_Length - 1);
		CAT(for_loop_string, for_loop_string, &comma_x1);
		CAT(for_loop_string, for_loop_string, for_loop_substr);
		CAT(for_loop_string, for_loop_string, &x1_equal_x1);
		CAT(for_loop_string, for_loop_string, &ps_text(sp));
		ps_text(sp) = *for_loop_substr;
		fprintf(string_file, "static %s%s;\n", symtype, for_loop_temp->_Address);
	} else {
		forceaccumulator(sp, ps_type[mp], TRUE);
	}
	aop = AOP_LE;
	if (ps_type[0] != CONSTANT) {
		temp_name(for_loop_temp, next_temp(), 1);
		SUBSTR3(for_loop_substr, for_loop_temp, 0, for_loop_temp->_Length - 1);
		CAT(for_loop_string, for_loop_string, &comma_x1);
		CAT(for_loop_string, for_loop_string, for_loop_substr);
		CAT(for_loop_string, for_loop_string, &x1_equal_x1);
		CAT(for_loop_string, for_loop_string, &ps_text(0));
		ps_text(0) = *for_loop_substr;
		fprintf(string_file, "static %s%s;\n", symtype, for_loop_temp->_Address);
	} else {
		if (ps_value[0] < 0) {
			aop = AOP_GE;
		}
		forceaccumulator(0, ps_type[mp], TRUE);
	}
	CAT(for_loop_string, for_loop_string, &semicolon_x1);
	CAT(for_loop_string, for_loop_string, &ps_text(mp));
	CAT(for_loop_string, for_loop_string, &aop_string[aop]);
	CAT(for_loop_string, for_loop_string, &ps_text(sp));
	CAT(for_loop_string, for_loop_string, &semicolon_x1);

	CAT(for_loop_string, for_loop_string, &ps_text(mp));
	CAT(for_loop_string, for_loop_string, &step);
	CAT(for_loop_string, for_loop_string, &ps_text(0));
	CAT(&ps_text(mp), for_loop_string, &while_tail);

	release_temp_descriptor(3);

	/* Housekeeping */
	ps_name(0)._Length = 0;
	ps_text(0)._Length = 0;
}

/*
**	dump_token()
**
**	Display the token and it's value.
*/
void
dump_token(void)
{
	STRING *dump_token_text;
	int i, ch;
	char *bp;

	printf("%-20s", v[token]);
	dump_token_text = get_temp_descriptor();
	if (token == number_token) {
		OUTPUT(0, CAT_INT(dump_token_text, &x1_equal_x1, number_value));
	} else {
		if (token == string_token || token == ident_token) {
			printf(" = ");
			bp = bcd._Address;
			for (i = 0; i < bcd._Length; i++) {
				ch = bp[i] & 255;
				if (printing[ch]) {
					printf("%c", ch);
				} else {
					printf("\\%03o", ch);
				}
			}
		}
		printf("\n");
	}
	release_temp_descriptor(1);
}

/*
**	initialize()
**
**	Initialize the compiler.
*/
int
initialize(void)
{
	static STRING splash = STR("XPL to C language translator -- version 0.4");
	static STRING line_name = STR("__LINE__");
	static STRING address = STR("address");
	static STRING bit32 = STR("bit(32)");
	static STRING bit64 = STR("bit(64)");
	int i, j, type;
	char *s;

	/* ndescript is bug compatable */
	ndescript = (sizeof(descriptor) / sizeof(descriptor[0])) - 1;
	if (__xpl_runtime_init(0x8000)) {
		return 1;
	}

	/* Announce the compiler */
	OUTPUT(0, &splash);

	input_unit = 0;  /* Default is stdin */
	if (input_filename) {
		static STRING mode = STR("r");
		STRING string_descriptor;

		i = xfopen(c2x_string(&string_descriptor, input_filename), &mode);
		if (i < 0) {
			fprintf(stderr, "File open error: %s\n", input_filename);
			return 1;
		}
		input_unit = i;
	}
	out_file = fopen(output_filename, "w");
	if (!out_file) {
		fprintf(stderr, "File open error: %s\n", output_filename);
		if (input_unit > 0) {
			xfclose(input_unit);
		}
		return 1;
	}
	string_file = fopen(string_filename, "w");
	if (!string_file) {
		fprintf(stderr, "File open error: %s\n", string_filename);
		if (input_unit > 0) {
			xfclose(input_unit);
		}
		fclose(out_file);
		return 1;
	}

	/* String conversion table */
	if ('A' == 65) {	/* ASCII */
		for (i = ' '; i < 127; i++) {
			printing[i] = TRUE;
		}
	} else {	/* EBCDIC */
		for (i = 0; i < 256; i++) {
			printing[i] = (isprint(i) != 0);
		}
	}
	/* Disable the special characters */
	printing['\"'] = printing['\\'] = 0;

	chartype[' '] = 1;
	chartype['\''] = 2;
	chartype['"'] = 3;
	escape_sequence['a'] = '\a';	/* Bell */
	escape_sequence['b'] = '\b';	/* Backspace */
	escape_sequence['f'] = '\f';	/* Formfeed */
	escape_sequence['n'] = '\n';	/* Newline */
	escape_sequence['r'] = '\r';	/* Carriage Return */
	escape_sequence['t'] = '\t';	/* Horizontal Tab */
	escape_sequence['v'] = '\v';	/* Vertical Tab */
	escape_sequence['\\'] = '\\';	/* Backslash */
	escape_sequence['\''] = '\'';	/* Single Quote */
	escape_sequence['\"'] = '\"';	/* Double Quote */
	escape_sequence['\?'] = '\?';	/* Question Mark */
	escape_sequence['e'] = 0x1b;	/* Escape Character */
	for (i = 0; i < 256; i++) {
		not_letter_or_digit[i] = TRUE;
		hex_digit[i] = 16;
	}
	for (i = 0; alphabet[i]; i++) {
		j = alphabet[i];
		not_letter_or_digit[j] = FALSE;
		chartype[j] = 4;
	}
	for (j = '0'; j <= '9'; j++) {
		not_letter_or_digit[j] = FALSE;
		chartype[j] = 5;
		hex_digit[j] = j - '0';
	}
	for (j = 0; j <= 5; j++) {
		hex_digit[j + 'a'] = j + 10;
		hex_digit[j + 'A'] = j + 10;
	}
	for (i = v_index[0]; i < v_index[1]; i++) {
		j = v[i][0];
		tx[j] = i;
		chartype[j] = 7;
	}
	tx['^'] = tx['~'];  chartype['^'] = chartype['~'];  /* For Moscow */
	chartype['|'] = 8;
	chartype['/'] = 6;

	for (i = 1; i <= NSY; i++) {
		s = v[i];
		if (strcasecmp(s, "<number>") == 0) number_token = i;  else
		if (strcasecmp(s, "<identifier>") == 0) ident_token = i;  else
		if (strcasecmp(s, "<string>") == 0) string_token = i;  else
		if (strcasecmp(s, "/") == 0) divide_token = i;  else
		if (strcasecmp(s, "_|_") == 0) {
			stopit[eof_token = i] = TRUE;
			v[eof_token] = "eof";
		} else
		if (strcasecmp(s, ";") == 0) stopit[i] = TRUE;  else
		if (strcasecmp(s, "|") == 0) or_token = i; else
		if (strcasecmp(s, "||") == 0) concatenate_token = i; else
		;
	}
	stopit[do_token] = TRUE;
	stopit[end_token] = TRUE;
	stopit[declare_token] = TRUE;
	stopit[procedure_token] = TRUE;

	control['H'] <<= 1;	/* Push the mangle flag */
	for (i = 0; i < 256; i++) {
		hash[i] = -1;
	}
	ndecsy = -1;
	procmark = 0;

	/* Map C language keywords to XPL identifiers */
	for (i = 0; poison_list[i]; i++) {
		poison[poison_list[i]] = TRUE;
	}
	top_mapper = -1;
	top_global = -1;
	enter_keywords();

	/* Create macro definitions to ignore case.  The -I option */
	if (control['I'] & 1) {
		ignore_case_macro();
	}

	sizeof_long = sizeof(XPL_LONG);
	if ((control['G'] & 1) == 1 && sizeof_long > 4) {
		sizeof_long = 4;
	}

	/* Create a macro to declare an object that will hold an address */
	if (sizeof(void *) <= 4 || (control['G'] & 1) == 1) {
		/* declare address literally 'bit(32)'; */
		address_symbol = enter_macro(&address, &bit32);
		type = FIXEDTYPE;
	} else {
		/* declare address literally 'bit(64)'; */
		address_symbol = enter_macro(&address, &bit64);
		type = DOUBLEWORD;
	}
	line_symbol = enter_macro(&line_name, &null);

	/* Add the pointer variables */
	for (i = 0; builtin_pointers[i].type; i++) {
		j = enter(&builtin_pointers[i].name, type, builtin_pointers[i].location, 0);
		syt_dim[j] = builtin_pointers[i].dimension;
	}

	/* Add the built-in functions to the symbol table */
	for (i = 0; builtin[i].type; i++) {
		if (BYTE(&builtin[i].name, 0) == ' ') {
			enter_parameter(builtin[i].type, builtin[i].location, 0);
		} else {
			j = enter(&builtin[i].name, builtin[i].type, builtin[i].location, 0);
			syt_dim[j] = builtin[i].dimension;
		}
	}
	/* Mark the end of the predefined symbols */
	initmark = procmark = ndecsy + 1;
	parct = 0;			/* Parameter count */
	control['H'] >>= 1;		/* Pop the mangle flag */

	returned_type = FIXEDTYPE;	/* Default return type */

	/* Set up global variables controlling scan */
	cp = 0;  text_limit = -1;
	text = null;
	current_procedure = null;
	compiling = TRUE;
	token = tx[';'];

	/* Initialize the parse stack */
	parse_stack[sp = 1] = eof_token;

	dsp = argv_limit + XPLRT_STRINGS;	/* Start of string space */
	divert_index[0] = 0;

	/* Output the header files for the .c code */
	emit_headers();

	return 0;
}


/*               Symbol and statistics printout                         */

int
string_gt(STRING *a, STRING *b)
{
	/*  Does an honest string compare for a > b.  Uses the
	    knowledge that neither a nor b may contain blanks.
	*/
	STRING *string_gt_string;
	int la, lb, v;

	string_gt_string = get_temp_descriptor();
	la = a->_Length;
	lb = b->_Length;
	if (la == lb) {
		/*  XCOM is honest in this case  */
		v = COMPARE(a, b) > 0;
	} else
	if (la > lb) {
		v = COMPARE(SUBSTR3(string_gt_string, a, 0, lb), b) >= 0;
	} else {
		v = COMPARE(a, SUBSTR3(string_gt_string, b, 0, la)) > 0;
	}
	release_temp_descriptor(1);
	return v;
}

STRING *
outline(STRING *outstr, STRING *name, int sym)
{
	static STRING at = STR(" at ");

	CAT(outstr, pad(name, 23), &colon_x1);
	if (syt_type[sym] == CHARFIXED) {
		CAT(outstr, outstr, &typename[CHARFIXED]);
		CAT(outstr, outstr, &open_paren);
		CAT_INT(outstr, outstr, syt_disp[sym]);
		CAT(outstr, outstr, &close_paren);
	} else {
		CAT(outstr, outstr, &typename[syt_type[sym]]);
	}
	if (syt_type[sym] == CHRTYPE || syt_type[sym] == SPECIAL) {
		CAT(outstr, outstr, &at);
		CAT_INT(outstr, outstr, syt_disp[sym]);
	}
	CAT_INT(outstr, pad(outstr, 48), declared_on_line[sym]);
	CAT_INT(outstr, pad(outstr, 55), syt_count[sym]);
#if 0
	/* Add the symbol table index to the display */
	CAT_INT(outstr, pad(outstr, 62), sym);
	__xpl_wrapper(outstr, pad(outstr, 69));
#endif
	if (COMPARE(&syt_name(sym), &syt_mapped(sym)) != 0) {
		CAT(outstr, pad(outstr, 62), &syt_mapped(sym));
	}

	return outstr;
}

/*
**	symboldump()
**
**	Lists the symbols in the procedure that has just been
**	compiled if dollar S or dollar D is enabled
*/
void
symboldump(void)
{
	int i, j, k, l, m;
	short sytsort[SYTSIZE + 1];
	static STRING symbol_table_dump = STR("Symbol  table  dump");
	static STRING param = STR("  parameter  ");
	STRING *symboldump_string, *symboldump_text;

	symboldump_string = get_temp_descriptor();
	symboldump_text = get_temp_descriptor();
	if (procmark <= ndecsy) {
		double_space;
		OUTPUT(0, &symbol_table_dump);
		blank_line;
		for (i = procmark; i <= ndecsy; i++) {
			sytsort[i] = i;
		}
		k = ndecsy - procmark;
		m = SHR(k + 1, 1);
		while(m > 0) {
			int limit = procmark + k - m;
			for(j = procmark; j <= limit; j++) {
				i = j;
				while(string_gt(&syt_name(sytsort[i]), &syt_name(sytsort[i+m]))) {
					l = sytsort[i];
					sytsort[i] = sytsort[i + m];
					sytsort[i + m] = l;
					i = i - m;
					if (i < procmark) break;
				}
			}
			m = SHR(m, 1);
		}
		i = procmark;
		while(syt_name(sytsort[i])._Length == 0) {
			i = i + 1;  /* Ignore null names */
		}
		for( ; i <= ndecsy; i++) {
			k = sytsort[i];
			if (syt_dim[k]) {
				CAT(symboldump_string, &syt_name(k), &open_paren);
				CAT_INT(symboldump_string, symboldump_string,
					syt_dim[k] - 1);
				CAT(symboldump_string, symboldump_string, &close_paren);
			} else {
				symboldump_string->_Length = syt_name(k)._Length;
				symboldump_string->_Address = syt_name(k)._Address;
			}
			OUTPUT(0, outline(symboldump_text, symboldump_string, k));

			k = k + 1;
			while((syt_name(k)._Length == 0) && (k <= ndecsy)) {
				j = k - sytsort[i];
				CAT_INT(symboldump_string, &param, j);
				OUTPUT(0, outline(symboldump_text, symboldump_string, k));
				k = k + 1;
			}
		}
		eject_page;
	}
	release_temp_descriptor(2);
}

/*
**	dumpit()
**
**	Dump statistics and other information gathered during the compilation.
*/
void
dumpit(void)
{
	static STRING global_info = STR("Identifiers promoted to Global Scope:");
	static STRING mapper_info = STR("Identifier mapper definitions:");
	static STRING literal = STR(" literally: ");
	STRING *dumpit_string;
	int i;

	procmark = initmark;
	symboldump();

	/* Dump out the pre-defined symbols */
	ndecsy = procmark - 1;
	procmark = 0;
	symboldump();

	if (top_global >= 0) {
		OUTPUT(0, &global_info);
		blank_line;
		for (i = 0; i <= top_global; i++) {
			OUTPUT(0, &global_name(i));
		}
		double_space;
	}

	OUTPUT(0, &mapper_info);
	blank_line;
	dumpit_string = get_temp_descriptor();
	for (i = 0; i <= top_mapper; i++) {
		CAT(dumpit_string, pad(&mapper_name(i), 20), &literal);
		CAT(dumpit_string, dumpit_string, &mapper_text(i));
		OUTPUT(0, dumpit_string);
	}
	release_temp_descriptor(1);
	double_space;

	/*  Put out the entry count for important procedures */
	printf("Function call and usage statistics:\n");
	blank_line;
	printf("scan()            = %d\n", count_scan);
	printf("id compares       = %d\n", idcompares);
	printf("emit()            = %d\n", count_emit);
	printf("forceaccumulator()= %d\n", count_force);
	printf("forcedescript()   = %d\n", count_desc);
	printf("arithemit()       = %d\n", count_arith);
	printf("genstore()        = %d\n", count_store);
	printf("stacking decisions= %d\n", count_stack);
	printf("String cache hits = %d\n", string_cache_hits);
	printf("Free string area  = %s\n", __c_decimal(freelimit - freebase));

	printf("Max. Stack depth  = %d (of %d)\n", stackmax, STACKSIZE + 1);
	printf("Symbol table size = %d (of %d)\n", maxndecsy, SYTSIZE + 1);
	printf("Mapper definitions= %d (of %d)\n", top_mapper + 1, MAPPER_LIMIT + 1);
	printf("Global remaps     = %d (of %d)\n", top_global + 1, GLOBAL_LIMIT + 1);
	printf("String temp stack = %d (of %d)\n", max_temp_descriptor - DESCRIPTOR_STACK,
		SYTSIZE_BASE - DESCRIPTOR_STACK);

	/* Runtime statistics */
	blank_line;
	__xpl_info();
}

void
stack_dump(void)
{
	static STRING partial_parse = STR("Partial parse to this point is: ");
	STRING *stack_dump_line, *stack_dump_string;
	int i;

	stack_dump_line = get_temp_descriptor();
	stack_dump_string = get_temp_descriptor();
	*stack_dump_line = partial_parse;
	for (i = 2; i <= sp; i++) {
		if (stack_dump_line->_Length > 105) {
			OUTPUT(0, stack_dump_line);
			*stack_dump_line = x4;
		}
		CAT(stack_dump_line, stack_dump_line, &x1);
		CAT(stack_dump_line, stack_dump_line,
			c2x_string(stack_dump_string, v[parse_stack[i]]));
	}
	OUTPUT(0, stack_dump_line);
	release_temp_descriptor(2);
}


/*                  The synthesis algorithm for XPL                      */

/*
**	illegal_argument(p, line)
**
**	Call error() with the "Illegal argument" message.
*/
void
illegal_argument(int p, int line)
{
	static STRING illarg = STR("Illegal argument of: ");
	STRING *illegal_argument_text;

	illegal_argument_text = get_temp_descriptor();
	CAT(illegal_argument_text, &illarg, &syt_name(ps_symtab[p]));
	error(illegal_argument_text, line, 0);
	release_temp_descriptor(1);
}

/*
**	too_many_arguments(p, line)
**
**	Call error() with the "Too many arguments" message.
*/
void
too_many_arguments(int p, int line)
{
	static STRING too_many = STR("Too many arguments for: ");
	STRING *too_many_arguments_text;

	too_many_arguments_text = get_temp_descriptor();
	CAT(too_many_arguments_text, &too_many, &syt_name(ps_symtab[p]));
	error(too_many_arguments_text, line, 0);
	release_temp_descriptor(1);
}

/*
**	too_few_arguments(p, line)
**
**	Call error() with the "Too few arguments" message.
*/
void
too_few_arguments(int p, int line)
{
	static STRING too_few = STR("Too few arguments for: ");
	STRING *too_few_arguments_text;

	too_few_arguments_text = get_temp_descriptor();
	CAT(too_few_arguments_text, &too_few, &syt_name(ps_symtab[p]));
	error(too_few_arguments_text, line, 0);
	release_temp_descriptor(1);
}

enum {PN_DUMMY = 0, PN_PROGRAM,
	PN_STATEMENT_LIST_1, PN_STATEMENT_LIST_2,
	PN_STATEMENT_1, PN_STATEMENT_2,
	PN_BASIC_STATEMENT_1, PN_BASIC_STATEMENT_2, PN_BASIC_STATEMENT_3,
	PN_BASIC_STATEMENT_4, PN_BASIC_STATEMENT_5, PN_BASIC_STATEMENT_6,
	PN_BASIC_STATEMENT_7, PN_BASIC_STATEMENT_8, PN_BASIC_STATEMENT_9,
	PN_IF_STATEMENT_1, PN_IF_STATEMENT_2, PN_IF_STATEMENT_3,
	PN_IF_CLAUSE, PN_TRUE_PART, PN_GROUP,
	PN_GROUP_HEAD_1, PN_GROUP_HEAD_2, PN_GROUP_HEAD_3,
	PN_GROUP_HEAD_4, PN_GROUP_HEAD_5,
	PN_STEP_DEFINITION,
	PN_ITERATION_CONTROL_1, PN_ITERATION_CONTROL_2,
	PN_WHILE_CLAUSE, PN_CASE_SELECTOR,
	PN_PROCEDURE_DEFINITION,
	PN_PROCEDURE_STATEMENT_1, PN_PROCEDURE_STATEMENT_2,
	PN_PROCEDURE_STATEMENT_3,
	PN_PROCEDURE_HEAD_1, PN_PROCEDURE_HEAD_2, PN_PROCEDURE_HEAD_3,
	PN_PROCEDURE_HEAD_4,
	PN_PROCEDURE_NAME, PN_PARAMETER_LIST,
	PN_PARAMETER_HEAD_1, PN_PARAMETER_HEAD_2,
	PN_ENDING_1, PN_ENDING_2, PN_ENDING_3,
	PN_LABEL_DEFINITION,
	PN_RETURN_STATEMENT_1, PN_RETURN_STATEMENT_2,
	PN_CALL_STATEMENT,
	PN_GOTO_STATEMENT, PN_GOTO_1, PN_GOTO_2,
	PN_DECLARATION_STATEMENT_1, PN_DECLARATION_STATEMENT_2,
	PN_DECLARATION_ELEMENT_1, PN_DECLARATION_ELEMENT_2,
	PN_TYPE_DECLARATION_1, PN_TYPE_DECLARATION_2, PN_TYPE_DECLARATION_3,
	PN_TYPE_1, PN_TYPE_2, PN_TYPE_3, PN_TYPE_4, PN_TYPE_5,
	PN_BIT_HEAD, PN_CHARACTER_HEAD, PN_BOUND_HEAD,
	PN_IDENTIFIER_SPECIFICATION_1, PN_IDENTIFIER_SPECIFICATION_2,
	PN_IDENTIFIER_LIST_1, PN_IDENTIFIER_LIST_2, PN_INITIAL_LIST,
	PN_INITIAL_HEAD_1, PN_INITIAL_HEAD_2,
	PN_ASSIGNMENT_1, PN_ASSIGNMENT_2,
	PN_REPLACE, PN_LEFT_PART,
	PN_EXPRESSION_1, PN_EXPRESSION_2, PN_EXPRESSION_3,
	PN_LOGICAL_FACTOR_1, PN_LOGICAL_FACTOR_2,
	PN_LOGICAL_SECONDARY_1, PN_LOGICAL_SECONDARY_2,
	PN_LOGICAL_PRIMARY_1, PN_LOGICAL_PRIMARY_2,
	PN_RELATION_1, PN_RELATION_2, PN_RELATION_3, PN_RELATION_4,
	PN_RELATION_5, PN_RELATION_6, PN_RELATION_7, PN_RELATION_8,
	PN_STRING_EXPRESSION_1, PN_STRING_EXPRESSION_2,
	PN_ARITHMETIC_EXPRESSION_1, PN_ARITHMETIC_EXPRESSION_2,
	PN_ARITHMETIC_EXPRESSION_3, PN_ARITHMETIC_EXPRESSION_4,
	PN_ARITHMETIC_EXPRESSION_5,
	PN_TERM_1, PN_TERM_2, PN_TERM_3, PN_TERM_4,
	PN_PRIMARY_1, PN_PRIMARY_2, PN_PRIMARY_3,
	PN_VARIABLE_1, PN_VARIABLE_2,
	PN_SUBSCRIPT_HEAD_1, PN_SUBSCRIPT_HEAD_2,
	PN_CONSTANT_1,
	PN_CONSTANT_2};

void
synthesize(int production_number)
{
	/* One statement for each production of the grammar */
	static STRING bad_point = STR("EOF at invalid point");
	static STRING undef_lab = STR("undefined label or procedure: ");
	static STRING no_parameters = STR("(void)");
	static STRING desc = STR("descriptor[");
	static STRING return_x1 = STR("return ");
	static STRING return_zero = STR("return 0;");
	static STRING if_clause = STR("if (");
	static STRING else_statement = STR("else");
	static STRING while_head = STR("while(");
	static STRING switch_start = STR("switch(");
	static STRING case_statement = STR("case ");
	static STRING break_statement = STR("break;");
	static STRING input_tail = STR("], 0)");
	static STRING open_close_paren = STR("()");
	static STRING bad_addr = STR("Illegal target of ADDR function.  Type: ");
	static STRING bad_sub = STR("Illegal subscript");
	static STRING byte_head = STR("corebyte[");
	static STRING halfword_head = STR("corehalfword[(XPL_ADDRESS)(");
	static STRING halfword_tail = STR(") >> 1]");
	static STRING word_head = STR("coreword[(XPL_ADDRESS)(");
	static STRING word_tail = STR(") >> 2]");
	static STRING longword_head = STR("corelongword[(XPL_ADDRESS)(");
	static STRING longword_tail = STR(") >> 3]");
	static STRING ampersand_ampersand = STR("&&");
	static STRING dot_address = STR("._Address");
	static STRING point_address = STR("->_Address");
	static STRING and_one = STR(" & 1");
	static STRING not_constant = STR("Value must be a constant");
	static STRING cast_void_star = STR("(void *)(XPL_ADDRESS)(");
	STRING *synthesize_string;
	int i, j, k, sym;

	synthesize_string = get_temp_descriptor();
	switch(production_number) {
	case PN_DUMMY:
		/* Case 0 is a dummy, because we number productions from 1 */
		break;
	case PN_PROGRAM:
		/*  <program>  ::=  <statement list>    */
		if (mp != 2) {
			error(&bad_point, __LINE__, 1);
			stack_dump();
		}
		if ((control['W'] & 1) == 0) {
			for (i = 1; i <= ndecsy; i++) {
				if ((syt_type[i] == FORWARD_LABEL || syt_type[i] == FORWARD_CALL ||
					syt_type[i] == FORWARD_GOTO) && syt_count[i] > 0) {
					CAT(synthesize_string, &undef_lab, &syt_name(i));
					error(synthesize_string, __LINE__, 0);
				}
			}
		}
		if (control['U'] & 1) {
			for (i = procmark; i <= ndecsy; i++) {
				if (syt_count[i] == 0) {
					CAT(synthesize_string, &unused, &syt_name(i));
					error(synthesize_string, __LINE__, 0);
				}
			}
		}
		compiling = FALSE;
		break;
	case PN_STATEMENT_LIST_1:
		/*  <statement list> ::= <statement>    */
   		break;
	case PN_STATEMENT_LIST_2:
		/*  <statement list> ::= <statement list> <statement>    */
		break;
	case PN_STATEMENT_1:
		/*  <statement> ::= <basic statement>    */
		statement_count = statement_count + 1;
		if (ps_text(sp)._Length) {
			emit_code(&ps_text(sp), ps_line[sp]);
			ps_text(sp)._Length = 0;
		}
		reset_temp();
		break;
	case PN_STATEMENT_2:
		/*  <statement> ::= <if statement>    */
		if (ps_text(sp)._Length) {
			emit_code(&ps_text(sp), ps_line[sp]);
			ps_text(sp)._Length = 0;
		}
		break;
	case PN_BASIC_STATEMENT_1:
		/*  <basic statement> ::= <assignment> ;    */
		CAT(&ps_text(mp), &ps_text(mp), &semicolon);
		break;
	case PN_BASIC_STATEMENT_2:
		/*  <basic statement> ::= <group> ;    */
		break;
	case PN_BASIC_STATEMENT_3:
		/*  <basic statement> ::= <procedure definition> ;    */
		break;
	case PN_BASIC_STATEMENT_4:
		/*  <basic statement> ::= <return statement> ;    */
		CAT(&ps_text(mp), &ps_text(mp), &semicolon);
		return_statement = statement_count;
		break;
	case PN_BASIC_STATEMENT_5:
		/*  <basic statement> ::= <call statement> ;    */
		if (ps_type[mp] == SPECIAL) {	/* BIN_INLINE */

			if (BYTE(&ps_text(mp), 0) == '#') {
				emit_preprocessor(&ps_text(mp), ps_line[mp]);
			} else {
				emit_declare(&ps_text(mp), ps_line[mp]);
			}
			ps_text(mp)._Length = 0;
		} else {
			CAT(&ps_text(mp), &ps_text(mp), &semicolon);
		}
		break;
	case PN_BASIC_STATEMENT_6:
		/*  <basic statement> ::= <go to statement> ;    */
		CAT(&ps_text(mp), &ps_text(mp), &semicolon);
		break;
	case PN_BASIC_STATEMENT_7:
		/*  <basic statement> ::= <declaration statement> ;    */
		break;
	case PN_BASIC_STATEMENT_8:
		/*  <basic statement> ::= ;    */
		emit_declare(&semicolon, ps_line[mp]);
		ps_text(mp)._Length = 0;
		break;
	case PN_BASIC_STATEMENT_9:
		/*  <basic statement> ::= <label definition> <basic statement>    */
		if (ps_text(sp)._Length) {
			emit_code(&ps_text(sp), ps_line[sp]);
			ps_text(sp)._Length = 0;
		}
		reset_temp();
		break;
	case PN_IF_STATEMENT_1:
		/*  <if statement> ::= <if clause> <statement>    */
		if (ps_code[mp]) {
			CAT(&ps_text(mp), &ps_text(mp), &close_brace);
			nest_block--;
		}
		ps_code[mp] = 0;
		break;
	case PN_IF_STATEMENT_2:
		/*  <if statement> ::= <if clause> <true part> <statement>    */
		if (ps_code[mp]) {
			CAT(&ps_text(mp), &ps_text(mp), &close_brace);
			nest_block--;
		}
		ps_code[mp] = 0;
		break;
	case PN_IF_STATEMENT_3:
		/*  <if statement> ::= <label definition> <if statement>    */
		if (ps_text(sp)._Length) {
			/* Emit the closing brace */
			emit_code(&ps_text(sp), ps_line[sp]);
			ps_text(sp)._Length = 0;
		}
		break;
	case PN_IF_CLAUSE:
		/*  <if clause> ::= if <expression> then    */
		if (ps_type[mpp1] != BOOLEAN && ps_type[mpp1] != PROC_BIT1) {
			forceaccumulator(mpp1, DOUBLEWORD, TRUE);
			CAT(&ps_text(mpp1), &ps_text(mpp1), &and_one);
		}
		CAT(synthesize_string, &if_clause, &ps_text(mpp1));
		if (token == if_token) {
			/* The C compiler complains about the dreaded Algol-60
			   dangling else problem.  Somebody needs to get a life. */
			CAT(synthesize_string, synthesize_string, &while_tail);
			ps_code[mp] = 1;
		} else {
			CAT(synthesize_string, synthesize_string, &close_paren);
			ps_code[mp] = 0;
		}
		emit_code(synthesize_string, ps_line[mpp1]);
		if (ps_code[mp]) {
			nest_block++;
		}
		break;
	case PN_TRUE_PART:
		/*  <true part> ::= <basic statement> else   */
		if (ps_text(mp)._Length) {
			emit_code(&ps_text(mp), ps_line[mp]);
			ps_text(mp)._Length = 0;
		}
		reset_temp();
		emit_code(&else_statement, ps_line[sp]);
		break;
	case PN_GROUP:
		/*  <group> ::= <group head> <ending>    */
		if (ps_code[mp] == DO_CASE) {
			emit_code(&break_statement, ps_line[sp]);
		}
		nest_block--;
		emit_code(&close_brace, ps_line[sp]);
		break;
	case PN_GROUP_HEAD_1:
		/*  <group head> ::= do ;    */
		emit_code(&open_brace, ps_line[mp]);
		nest_block++;
		ps_code[mp] = DO_GROUP;
		break;
	case PN_GROUP_HEAD_2:
		/*  <group head> ::= do <step definition> ;    */
		emit_code(&ps_text(mpp1), ps_line[mpp1]);
		nest_block++;
		ps_code[mp] = DO_STEP;
		break;
	case PN_GROUP_HEAD_3:
		/*  <group head> ::= do <while clause> ;    */
		CAT(synthesize_string, &while_head, &ps_text(mpp1));
		CAT(synthesize_string, synthesize_string, &while_tail);
		emit_code(synthesize_string, ps_line[mpp1]);
		nest_block++;
		ps_symtab[mp] = ps_symtab[mpp1];
		ps_code[mp] = DO_WHILE;
		break;
	case PN_GROUP_HEAD_4:
		/*  <group head> ::= do <case selector> ;    */
		emit_code(&ps_text(mpp1), ps_line[mpp1]);
		ps_code[mp] = DO_CASE;
		ps_cnt[mp] = 0;
		CAT_INT(synthesize_string, &case_statement, ps_cnt[mp]);
		CAT(synthesize_string, synthesize_string, &colon);
		emit_code(synthesize_string, ps_line[mpp1]);
		{
			static STRING case_zero = STR(" case 0.");

			CAT(&information, &information, &case_zero);
		}
		nest_block++;
		break;
	case PN_GROUP_HEAD_5:
		/*  <group head> ::= <group head> <statement>    */
		if (ps_code[mp] == DO_CASE) {
			emit_code(&break_statement, ps_line[mpp1]);
			ps_cnt[mp]++;
			CAT_INT(synthesize_string, &case_statement, ps_cnt[mp]);
			CAT(synthesize_string, synthesize_string, &colon);
			nest_block--;
			emit_code(synthesize_string, ps_line[mpp1]);
			nest_block++;
			if (token != end_token) {
				static STRING case_info = STR(" case ");

				CAT(&information, &information, &case_info);
				CAT_INT(&information, &information, ps_cnt[mp]);
				CAT(&information, &information, &period);
			}
		}
		break;
	case PN_STEP_DEFINITION:
		/*  <step definition> ::= <variable> <replace> <expression> <iteration control> */
		for_loop();
		break;
	case PN_ITERATION_CONTROL_1:
		/*  <iteration control> ::= to <expression>    */
		ps_type[0] = CONSTANT;
		ps_value[0] = 1;	/* By 1 */
		set_limit();
		break;
	case PN_ITERATION_CONTROL_2:
		/*  <iteration control> ::= to <expression> by <expression>    */
		if (ps_type[sp] == CONSTANT) {
			/* By <constant> */
			ps_type[0] = CONSTANT;
			ps_value[0] = ps_value[sp];
		} else {
			/* By <expression> */
			/* Fix this $$$ */
			forceaccumulator(sp, DOUBLEWORD, TRUE);
			movestacks(sp, 0);
		}
		set_limit();
		break;
	case PN_WHILE_CLAUSE:
		/*  <while clause> ::= while <expression>    */
		forceaccumulator(sp, DOUBLEWORD, FALSE);
		ps_text(mp) = ps_text(sp);
		break;
	case PN_CASE_SELECTOR:
		/*  <case selector> ::= case <expression>    */
		forceaccumulator(sp, DOUBLEWORD, FALSE);
		CAT(synthesize_string, &switch_start, &ps_text(sp));
		CAT(&ps_text(mp), synthesize_string, &while_tail);
		break;
	case PN_PROCEDURE_DEFINITION:
		/*  <procedure definition> ::= <procedure statement> <statement list> <ending>    */
		/* Procedure is defined, restore symbol table */
		if (returned_type != 0 && return_statement != statement_count - 1) {
			/* Add a return to make the compiler happy */
			emit_code(&return_zero, ps_line[sp]);
		}
		nest_block--;
		emit_code(&close_brace, ps_line[sp]);
		pop_divert();
		if (ps_name(sp)._Length > 0) {
			SUBSTR2(synthesize_string, &current_procedure, 1);
			if (COMPARE(synthesize_string, &ps_name(sp)) != 0) {
				static STRING proc = STR("Procedure");
				static STRING closed_by_end = STR(" closed by END ");

				CAT(synthesize_string, &proc, &current_procedure);
				CAT(synthesize_string, synthesize_string, &closed_by_end);
				CAT(synthesize_string, synthesize_string, &ps_name(sp));
				error(synthesize_string, __LINE__, 0);
			}
		}
		if (control['S'] & 1) symboldump();
		for (i = procmark; i <= ndecsy; i++) {
			if ((syt_type[i] == FORWARD_LABEL || syt_type[i] == FORWARD_CALL ||
				syt_type[i] == FORWARD_GOTO) && syt_count[i] > 0) {
				CAT(synthesize_string, &undef_lab, &syt_name(i));
				error(synthesize_string, __LINE__, 0);
			}
			if (syt_count[i] == 0 && (control['U'] & 1) == 1) {
				CAT(synthesize_string, &unused, &syt_name(i));
				error(synthesize_string, __LINE__, 0);
			}
			j = ndecsy - i + procmark;
			if (syt_name(j)._Length > 0) {
				/*  Unwind hash pointers  */
				hash[hasher(&syt_name(j))] = ptr[j];
			}
		}
		/* Discard parameter names */
		for (i = procmark; i < procmark + parct; i++) {
			syt_name(i)._Length = 0;
		}
		/* Mark the end of parameters and do some housekeeping */
		for (i = procmark + parct; i <= ndecsy + 1; i++) {
			syt_name(i) = x1;
			syt_mapped(i)._Length = 0;
		}
		ndecsy = procmark + parct - 1;
		current_procedure = ps_name(mp);
		procmark = ps_symtab[mp];
		parct = ps_cnt[mp];
		returned_type = ps_type[mp];
		ps_text(mp)._Length = 0;	/* Housekeeping */
		clear_temp();
		no_body = 0;
		break;
	case PN_PROCEDURE_STATEMENT_1:
		/*  <procedure statement> ::= <procedure head> ;    */
		CAT(synthesize_string, &ps_text(mp), &x1);
		CAT(synthesize_string, synthesize_string, &open_brace);
		emit_declare(synthesize_string, ps_line[mp]);
		nest_block++;
		if (no_body) {
			static STRING bad_nesting = STR("Illegal procedure nesting");
			error(&bad_nesting, __LINE__, 1);
		}
		break;
	case PN_PROCEDURE_STATEMENT_2:
		/*  <procedure statement> ::= <procedure head> external    */
		CAT(synthesize_string, &ps_text(mp), &semicolon);
		emit_declare(synthesize_string, ps_line[mp]);
		nest_block++;
		no_body = 1;
		break;
	case PN_PROCEDURE_STATEMENT_3:
		/*  <procedure statement> ::= <procedure head> transparent    */
		nest_block++;
		no_body = 1;
		break;
	case PN_PROCEDURE_HEAD_1:
		/*  <procedure head> ::= <procedure name>    */
		push_procmark(mp);
		returned_type = 0;
		push_divert(mp);
		CAT(synthesize_string, &dcltypes[returned_type], &ps_text(mp));
		CAT(&ps_text(mp), synthesize_string, &no_parameters);
		break;
	case PN_PROCEDURE_HEAD_2:
		/*  <procedure head> ::= <procedure name> <type>    */
		/* Set the type before the call to push_procmark() */
		if (ps_type[mpp1] == CHARFIXED) {
			/* Functions must return a descriptor */
			ps_type[mpp1] = CHRTYPE;
		}
		syt_type[ps_symtab[mp]] = ps_type[mpp1] + 8;
		push_procmark(mp);
		returned_type = ps_type[mpp1];
		push_divert(mp);
		CAT(synthesize_string, &dcltypes[returned_type], &ps_text(mp));
		CAT(&ps_text(mp), synthesize_string, &no_parameters);
		break;
	case PN_PROCEDURE_HEAD_3:
		/*  <procedure head> ::= <procedure name> <parameter list>    */
		push_divert(mp);
		syt_disp[ps_symtab[mp]] = temp_variable_name - parct + 1;
		ps_symtab[mp] = ps_symtab[mpp1];
		returned_type = 0;
		CAT(synthesize_string, &dcltypes[returned_type], &ps_text(mp));
		CAT(&ps_text(mp), synthesize_string, &ps_text(mpp1));
		break;
	case PN_PROCEDURE_HEAD_4:
		/*  <procedure head> ::= <procedure name> <parameter list> <type>       */
		if (ps_type[sp] == CHARFIXED) {
			/* Functions must return a descriptor */
			ps_type[sp] = CHRTYPE;
		}
		push_divert(mp);
		syt_disp[ps_symtab[mp]] = temp_variable_name - parct + 1;
		returned_type = ps_type[sp];
		syt_type[ps_symtab[mp]] = returned_type + 8;
		ps_symtab[mp] = ps_symtab[mpp1];
		CAT(synthesize_string, &dcltypes[returned_type], &ps_text(mp));
		CAT(&ps_text(mp), synthesize_string, &ps_text(mpp1));
		break;
	case PN_PROCEDURE_NAME:
		/*  <procedure name> ::= <label definition> procedure    */
		syt_type[ps_symtab[mp]] = PROC_VOID;
		ps_text(mp) = syt_mapped(ps_symtab[mp]);
		CAT(synthesize_string, &x1, &ps_name(mp));
		ps_name(mp) = current_procedure;
		current_procedure = *synthesize_string;
		ps_type[mp] = returned_type;
		ps_cnt[mp] = parct;
		parct = 0;
		break;
	case PN_PARAMETER_LIST:
		/*  <parameter list> ::= <parameter head> <identifier> )    */
		enter(&ps_name(mpp1), UNDEF_ARG, 0, 0);
		temp_name(synthesize_string, next_temp(), 0);	/* __xpl_999 */
		CAT(synthesize_string, &ps_text(mp), synthesize_string);
		CAT(&ps_text(mp), synthesize_string, &close_paren);
		parct = parct + 1;
		break;
	case PN_PARAMETER_HEAD_1:
		/*  <parameter head> ::= (   */
		/* Point at the first parameter for symbol table */
		push_procmark(mp);
		ps_text(mp) = open_paren;
		break;
	case PN_PARAMETER_HEAD_2:
		/*  <parameter head> ::= <parameter head> <identifier> ,    */
		enter(&ps_name(mpp1), UNDEF_ARG, 0, 0);
		temp_name(synthesize_string, next_temp(), 0);	/* __xpl_999 */
		CAT(synthesize_string, &ps_text(mp), synthesize_string);
		CAT(&ps_text(mp), synthesize_string, &comma_x1);
		parct = parct + 1;
		break;
	case PN_ENDING_1:
		/*  <ending> ::= end    */
		ps_name(mp)._Length = 0;
		ps_text(mp)._Length = 0;	/* Housekeeping */
		break;
	case PN_ENDING_2:
		/*  <ending> ::= end <identifier>    */
		ps_name(mp) = ps_name(sp);
		ps_text(mp)._Length = 0;	/* Housekeeping */
		break;
	case PN_ENDING_3:
		/*  <ending> ::= <label definition> <ending>    */
		emit_code(&semicolon, ps_line[sp]);
		ps_name(mp) = ps_name(sp);
		ps_text(mp) = ps_text(sp);
		break;
	case PN_LABEL_DEFINITION:
		/*  <label definition> ::= <identifier> :    */
		if (token == procedure_token) {
			ps_symtab[mp] = enter(&ps_name(mp), PROC_VOID, 0, ps_line[mp]);
			ps_text(mp) = syt_mapped(ps_symtab[mp]);
		} else {
			ps_symtab[mp] = enter(&ps_name(mp), DEFINED_LABEL, 0, ps_line[mp]);
			ps_text(mp) = syt_mapped(ps_symtab[mp]);
			CAT(synthesize_string, &ps_text(mp), &colon_x1);
			emit_code(synthesize_string, ps_line[sp]);
		}
		ps_text(mp)._Length = 0;	/* Must be cleared or it gets emitted */
		break;
	case PN_RETURN_STATEMENT_1:
		/*  <return statement> ::= return    */
		{
			static STRING return_text = STR("return");

			ps_text(mp) = return_text;
		}
		break;
	case PN_RETURN_STATEMENT_2:
		/*  <return statement> ::= return <expression>    */
		if (returned_type == CHRTYPE) {
			forcedescript(mpp1, DESCRIPT);
		} else {
			forceaccumulator(mpp1, returned_type, FALSE);
		}
		CAT(&ps_text(mp), &return_x1, &ps_text(mpp1));
		break;
	case PN_CALL_STATEMENT:
		/*  <call statement> ::= call <variable>    */
		ps_text(mp) = ps_text(sp);
		ps_type[mp] = ps_type[sp];
		break;
	case PN_GOTO_STATEMENT:
		/*  <go to statement> ::= <go to> <identifier>    */
		id_lookup(sp, FORWARD_LABEL);
		sym = ps_symtab[sp];
		if (sym < 0) {
			sym = enter(&ps_name(sp), FORWARD_LABEL, 0, ps_line[sp]);
			syt_count[sym] = 1;
		}
		if (syt_type[sym] == FORWARD_LABEL) {
			/* We now know its a goto label */
			syt_type[sym] = FORWARD_GOTO;
		}
		if (syt_type[sym] == DEFINED_LABEL || syt_type[sym] == FORWARD_GOTO) {
			static STRING goto_x1 = STR("goto ");

			CAT(&ps_text(mp), &goto_x1, &syt_mapped(sym));
		} else {
			static STRING bad_target = STR("Target of go to is not a label");

			error(&bad_target, __LINE__, 1);
			ps_text(mp)._Length = 0;	/* Housekeeping */
		}
		ps_text(sp)._Length = 0;	/* Housekeeping */
		ps_name(sp)._Length = 0;	/* Housekeeping */
		break;
	case PN_GOTO_1:
		/*  <go to> ::= go to    */
		break;
	case PN_GOTO_2:
		/*  <go to> ::= goto    */
		break;
	case PN_DECLARATION_STATEMENT_1:
		/*  <declaration statement> ::=  declare <declaration element>    */
		break;
	case PN_DECLARATION_STATEMENT_2:
		/*  <declaration statement> ::= <declaration statement> , <declaration element> */
		break;
	case PN_DECLARATION_ELEMENT_1:
		/*  <declaration element> ::= <type declaration>    */
		if (ps_type[mp] == CHRTYPE) {
			dsp = newdsp;
		}
		break;
	case PN_DECLARATION_ELEMENT_2:
		/*  <declaration element> ::= <identifier> literally <string>    */
		enter_macro(&ps_name(mp), &ps_name(sp));
		break;
	case PN_TYPE_DECLARATION_1:
		/*  <type declaration> ::= <identifier specification> <type>    */
		tdeclare(0, 0);
		break;
	case PN_TYPE_DECLARATION_2:
		/*  <type declaration> ::= <bound head> <expression> ) <type>    */
		if (ps_type[mpp1] != CONSTANT) {
			error(&not_constant, __LINE__, 0);
			ps_value[mpp1] = 1;
		}
		tdeclare(ps_value[mpp1], 1);
		break;
	case PN_TYPE_DECLARATION_3:
		/*  <type declaration> ::= <type declaration> <initial list>    */
		break;
	case PN_TYPE_1:
		/*  <type> ::= fixed    */
		ps_type[mp] = FIXEDTYPE;
		break;
	case PN_TYPE_2:
		/*  <type> ::= character    */
		ps_type[mp] = CHRTYPE;
		break;
	case PN_TYPE_3:
		/*  <type> ::= label    */
		ps_type[mp] = FORWARD_LABEL;
		break;
	case PN_TYPE_4:
		/*  <type> ::= <bit head> <expression> )    */
		if (ps_type[mpp1] != CONSTANT) {
			error(&not_constant, __LINE__, 0);
			ps_value[mpp1] = 32;
		}
		if (ps_value[mpp1] <= 1) ps_type[mp] = BOOLEAN;
		else if (ps_value[mpp1] <= 8) ps_type[mp] = BYTETYPE;
		else if (ps_value[mpp1] <= 16) ps_type[mp] = HALFWORD;
		else if (ps_value[mpp1] <= 32) ps_type[mp] = FIXEDTYPE;
		else if (ps_value[mpp1] <= 64 && sizeof_long == 8) {
			ps_type[mp] = DOUBLEWORD;
		} else ps_type[mp] = CHRTYPE;
		break;
	case PN_TYPE_5:
		/*  <type> ::= <character head> <expression> )    */
		if (ps_type[mpp1] != CONSTANT) {
			error(&not_constant, __LINE__, 0);
			ps_value[mpp1] = 2;
		}
		ps_type[mp] = CHARFIXED;
		ps_value[mp] = ps_value[mpp1];
		break;
	case PN_BIT_HEAD:
		/*  <bit head> ::= bit (   */
		break;
	case PN_CHARACTER_HEAD:
		/*  <character head> ::= character (   */
		break;
	case PN_BOUND_HEAD:
		/*  <bound head> ::= <identifier specification> (   */
		break;
	case PN_IDENTIFIER_SPECIFICATION_1:
		/*  <identifier specification> ::= <identifier>    */
		ps_code[mp] = casep;
		if (casep > CASELIMIT) {
			error(&dclrm, __LINE__, 1);
		} else {
			casestack[casep] = enter(&ps_name(mp), 0, 0, ps_line[mp]);
			casep = casep + 1;	/* Reset by tdeclare() */
		}
		break;
	case PN_IDENTIFIER_SPECIFICATION_2:
		/*  <identifier specification> ::= <identifier list> <identifier> )    */
		if (casep > CASELIMIT) {
			error(&dclrm, __LINE__, 1);
		} else {
			casestack[casep] = enter(&ps_name(mpp1), 0, 0, ps_line[mpp1]);
			casep = casep + 1;	/* Reset by tdeclare() */
		}
		break;
	case PN_IDENTIFIER_LIST_1:
		/*  <identifier list> ::= (   */
		ps_code[mp] = casep;
		break;
	case PN_IDENTIFIER_LIST_2:
		/*  <identifier list> ::= <identifier list> <identifier> ,    */
		if (casep > CASELIMIT) {
			error(&dclrm, __LINE__, 1);
		} else {
			casestack[casep] = enter(&ps_name(mpp1), 0, 0, ps_line[mpp1]);
			casep = casep + 1;
		}
		break;
	case PN_INITIAL_LIST:
		/*  <initial list> ::= <initial head> <expression> )    */
		setinit(mp, mpp1);
		if (ps_text(mp)._Length) {
			CAT(&ps_text(mp), &ps_text(mp), &close_brace_semicolon);
			emit_declare(&ps_text(mp), ps_line[mp]);
			nest_block--;
		}
		break;
	case PN_INITIAL_HEAD_1:
		/*  <initial head> ::= initial (   */
		ps_text(mp)._Length = 0;
		break;
	case PN_INITIAL_HEAD_2:
		/*  <initial head> ::= <initial head> <expression> ,    */
		setinit(mp, mpp1);
		break;
	case PN_ASSIGNMENT_1:
		/*  <assignment> ::= <variable> <replace> <expression>    */
		genstore(mp, sp);
		break;
	case PN_ASSIGNMENT_2:
		/*  <assignment> ::= <left part> <assignment>    */
		genstore(mp, sp);
		break;
	case PN_REPLACE:
		/*  <replace> ::= =    */
		break;
	case PN_LEFT_PART:
		/*  <left part> ::= <variable> ,    */
		break;
	case PN_EXPRESSION_1:
		/*  <expression> ::= <logical factor>    */
		break;
	case PN_EXPRESSION_2:
		/*  <expression> ::= <expression> | <logical factor>    */
		arithemit(mp, AOP_OR, sp);
		break;
	case PN_EXPRESSION_3:
		/*  <expression> ::= <expression> xor <logical factor>    */
		arithemit(mp, AOP_XOR, sp);
		break;
	case PN_LOGICAL_FACTOR_1:
		/*  <logical factor> ::= <logical secondary>    */
		break;
	case PN_LOGICAL_FACTOR_2:
		/*  <logical factor> ::= <logical factor> & <logical secondary>    */
		arithemit(mp, AOP_AND, sp);
		break;
	case PN_LOGICAL_SECONDARY_1:
		/*  <logical secondary> ::= <logical primary>    */
		break;
	case PN_LOGICAL_SECONDARY_2:
		/*  <logical secondary> ::= ~ <logical primary>    */
		movestacks(sp, mp);
		if (ps_type[mp] == CONSTANT) {
			ps_value[mp] = ~ ps_value[mp];
			ps_name(mp)._Length = 0;  /* Not just housekeeping */
		} else {
			static STRING complement_bool = STR("!");
			static STRING complement_numeric = STR("~");

			forceaccumulator(mp, DOUBLEWORD, TRUE);
			if (ps_type[mp] == BOOLEAN || ps_type[mp] == BYTETYPE) {
				CAT(&ps_text(mp), &complement_bool, &ps_text(mp));
			} else {
				CAT(&ps_text(mp), &complement_numeric, &ps_text(mp));
			}
			ps_bin[mp] = BIN_MOPAR;
		}
		break;
	case PN_LOGICAL_PRIMARY_1:
		/*  <logical primary> ::= <string expression>    */
		break;
	case PN_LOGICAL_PRIMARY_2:
		/*  <logical primary> ::= <string expression> <relation> <string expression> */
		arithemit(mp, ps_code[mpp1], sp);
		break;
	case PN_RELATION_1:
		/*  <relation> ::= =    */
		ps_code[mp] = AOP_EQ;
		break;
	case PN_RELATION_2:
		/*  <relation> ::= <    */
		ps_code[mp] = AOP_LT;
		break;
	case PN_RELATION_3:
		/*  <relation> ::= >    */
		ps_code[mp] = AOP_GT;
		break;
	case PN_RELATION_4:
		/*  <relation> ::= ~ =    */
		ps_code[mp] = AOP_NE;
		break;
	case PN_RELATION_5:
		/*  <relation> ::= ~ <    */
		ps_code[mp] = AOP_GE;
		break;
	case PN_RELATION_6:
		/*  <relation> ::= ~ >    */
		ps_code[mp] = AOP_LE;
		break;
	case PN_RELATION_7:
		/*  <relation> ::= < =    */
		ps_code[mp] = AOP_LE;
		break;
	case PN_RELATION_8:
		/*  <relation> ::= > =    */
		ps_code[mp] = AOP_GE;
		break;
	case PN_STRING_EXPRESSION_1:
		/*  <string expression> ::= <arithmetic expression>    */
		break;
	case PN_STRING_EXPRESSION_2:
		/*  <string expression> ::= <string expression> || <arithmetic expression> */
		if (ps_type[mp] == CONSTANT) {
			if (ps_name(mp)._Length == 0) {
				/* Convert an integer constant to a string constant */
				DECIMAL(&ps_name(mp), ps_value[mp]);
			}
			ps_type[mp] = STRINGCON;
		}
		if (ps_type[sp] == CONSTANT) {
			if (ps_name(sp)._Length == 0) {
				/* Convert an integer constant to a string constant */
				DECIMAL(&ps_name(sp), ps_value[sp]);
			}
			ps_type[sp] = STRINGCON;
		}
		if (ps_type[mp] == STRINGCON && ps_type[sp] == STRINGCON) {
			/* Concatenate the two constant strings at compile time */
			CAT(&ps_name(mp), &ps_name(mp), &ps_name(sp));
			break;
		}
		forcedescript(mp, DESCRIPT);
		if (ps_type[sp] >= PROC_BIT1 && ps_type[sp] <= PROC_BIT64) {
			forceaccumulator(sp, DOUBLEWORD, TRUE);
		}
		i = get_temp();
		if (ps_type[sp] >= BOOLEAN && ps_type[sp] <= DOUBLEWORD) {
			/* __xpl_cat_int(&descriptor[temp], ps_text, value) */
			forceaccumulator(sp, DOUBLEWORD, TRUE);
			CAT_INT(synthesize_string, &cat_int_descriptor, i);
		} else {
			/* __xpl_cat(&descriptor[temp], ps_text, ps_text) */
			forcedescript(sp, DESCRIPT);
			CAT_INT(synthesize_string, &cat_descriptor, i);
		}
		CAT(synthesize_string, synthesize_string, &bracket_comma_x1);
		CAT(synthesize_string, synthesize_string, &ps_text(mp));
		CAT(synthesize_string, synthesize_string, &comma_x1);
		CAT(synthesize_string, synthesize_string, &ps_text(sp));
		CAT(&ps_text(mp), synthesize_string, &close_paren);
		ps_type[mp] = DESCRIPT;
		break;
	case PN_ARITHMETIC_EXPRESSION_1:
		/*  <arithmetic expression> ::= <term>    */
		break;
	case PN_ARITHMETIC_EXPRESSION_2:
		/*  <arithmetic expression> ::= <arithmetic expression> + <term>    */
		arithemit(mp, AOP_ADD, sp);
		break;
	case PN_ARITHMETIC_EXPRESSION_3:
		/*  <arithmetic expression> ::= <arithmetic expression> - <term>    */
		arithemit(mp, AOP_SUBTRACT, sp);
		break;
	case PN_ARITHMETIC_EXPRESSION_4:
		/*  <arithmetic expression> ::= + <term>    */
		movestacks(mpp1, mp);
		break;
	case PN_ARITHMETIC_EXPRESSION_5:
		/*  <arithmetic expression> ::= - <term>    */
		movestacks(mpp1, mp);
		if (ps_type[mp] == CONSTANT) {
			ps_value[mp] = - ps_value[mp];
			ps_name(mp)._Length = 0;  /* Not just housekeeping */
		} else {
			static STRING negate = STR("-");

			forceaccumulator(mp, DOUBLEWORD, TRUE);
			CAT(&ps_text(mp), &negate, &ps_text(mp));
		}
		break;
	case PN_TERM_1:
		/*  <term> ::= <primary>    */
		break;
	case PN_TERM_2:
		/*  <term> ::= <term> * <primary>    */
		arithemit(mp, AOP_MULTIPLY, sp);
		break;
	case PN_TERM_3:
		/*  <term> ::= <term> / <primary>    */
		arithemit(mp, AOP_DIVIDE, sp);
		break;
	case PN_TERM_4:
		/*  <term> ::= <term> mod <primary>    */
		arithemit(mp, AOP_MOD, sp);
		break;
	case PN_PRIMARY_1:
		/*  <primary> ::= <constant>    */
		break;
	case PN_PRIMARY_2:
		/*  <primary> ::= <variable>    */
		break;
	case PN_PRIMARY_3:
		/*  <primary> ::= ( <expression> )    */
		movestacks(mpp1, mp);
		if (ps_type[mp] != CONSTANT && ps_type[mp] != STRINGCON) {
			forceaccumulator(mp, DOUBLEWORD, FALSE);
			CAT(synthesize_string, &open_paren, &ps_text(mp));
			CAT(&ps_text(mp), synthesize_string, &close_paren);
		}
		break;
	case PN_VARIABLE_1:
		/*  <variable> ::= <identifier>    */
		/* Find the identifier in the symbol table */
		id_lookup(mp, FIXEDTYPE);
		sym = ps_symtab[mp];
		switch (ps_bin[mp]) {
		case BIN_CALL:
			CAT(&ps_text(mp), &ps_text(mp), &open_close_paren);
			break;
		case BIN_SCALAR:
			break;
		case BIN_COREBYTE:
		case BIN_COREHALFWORD:
		case BIN_COREWORD:
		case BIN_CORELONGWORD:
		case BIN_ARRAY:
			/* Add [0] to any unsubscripted arrays */
			CAT(&ps_text(mp), &ps_text(mp), &index_zero);
			break;
		case BIN_STRING:
			CAT_INT(synthesize_string, &desc, syt_disp[sym]);
			CAT(&ps_text(mp), synthesize_string, &right_bracket);
			ps_type[mp] = CHRTYPE;
			break;
		case BIN_CHAR_POINTER:
			/* These can never be subscripted */
			ps_type[mp] = DESCRIPT;
			break;
		case BIN_INPUT:
			j = get_temp();
			CAT_INT(synthesize_string, &input_descriptor, j);
			CAT(&ps_text(mp), synthesize_string, &input_tail);
			ps_type[mp] = DESCRIPT;
			break;
		default:
			break;
		}
		break;
	case PN_VARIABLE_2:
		/*  <variable> ::= <subscript head> <expression> )    */
		/*  This production is called for both the left and the right of
		    an assignment. */
		ps_cnt[mp] = ps_cnt[mp] + 1;
		sym = ps_symtab[mp];

		/*  Either a procedure call, array or built in function */
		switch (ps_bin[mp]) {
		case BIN_SCALAR:
			{
				static STRING not_array =
					STR("Identifier is not an array");

				error(&not_array, __LINE__, 0);
			}
			break;
		case BIN_ARRAY:
		case BIN_STRING:
			/* Subscripted variable */
			if (ps_cnt[mp] > 1) {
				static STRING multisub =
					STR("Multiple subscripts not allowed");

				error(&multisub, __LINE__, 0);
			}
			forceaccumulator(mpp1, DOUBLEWORD, TRUE);
			if (ps_type[mpp1] < BOOLEAN || ps_type[mpp1] > DOUBLEWORD) {
				static STRING bad_string =
					STR("String not allowed as a subscript");

				error(&bad_string, __LINE__, 0);
			}
			CAT(&ps_text(mp), &ps_text(mp), &ps_text(mpp1));
			CAT(&ps_text(mp), &ps_text(mp), &right_bracket);
			break;
		case BIN_CALL:
			stuff_parameter(TRUE);
			CAT(&ps_text(mp), &ps_text(mp), &close_paren);
			break;
		case BIN_CHAR_POINTER:
			/* These can never be subscripted */
			error(&bad_sub, __LINE__, 0);
			ps_type[mp] = DESCRIPT;
			break;
		case BIN_LENGTH:
			if (ps_cnt[mp] > 1) {
				too_many_arguments(mp, __LINE__);
				ps_type[mp] = VOID_TYPE;
				break;
			}
			if (ps_type[mpp1] == CONSTANT) {
				if (ps_name(mpp1)._Length == 0) {
					/* Convert an integer constant to a string constant */
					DECIMAL(&ps_name(mpp1), ps_value[mpp1]);
				}
				ps_type[mpp1] = STRINGCON;
			}
			if (ps_type[mpp1] == STRINGCON) {
				/* Get the length of the string constant */
				ps_value[mp] = ps_name(mpp1)._Length;
				ps_name(mp)._Length = 0;  /* Not just housekeeping */
				ps_type[mp] = CONSTANT;
				break;
			}
			movestacks(mpp1, mp);
			forcedescript(mp, 0);
			if (ps_type[mp] == CHRTYPE) {
				CAT(&ps_text(mp), &ps_text(mp), &dot_length);
			} else
			if (ps_type[mp] == DESCRIPT) {
				CAT(&ps_text(mp), &ps_text(mp), &point_length);
			} else {
				error(&oops, __LINE__, 1);
			}
			ps_type[mp] = FIXEDTYPE;
			ps_bin[mp] = BIN_ACCUMULATOR;
			break;
		case BIN_SUBSTR:
			if (ps_cnt[mp] == 2) {
				/* Two arguments:  SUBSTR(string, position) */
				static STRING substr2 = STR("__xpl_substr2");

				forceaccumulator(mpp1, FIXEDTYPE, FALSE);
				CAT(synthesize_string, &substr2, &ps_text(mp));
				CAT(synthesize_string, synthesize_string, &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &close_paren);
			} else
			if (ps_cnt[mp] == 3) {
				/* Three arguments:  SUBSTR(string, position, length) */
				static STRING substr3 = STR("__xpl_substr3");

				forceaccumulator(mpp1, FIXEDTYPE, FALSE);
				CAT(synthesize_string, &substr3, &ps_text(mp));
				CAT(synthesize_string, synthesize_string, &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &close_paren);
			} else {
				too_many_arguments(mp, __LINE__);
			}
			ps_type[mp] = DESCRIPT;
			break;
		case BIN_BYTE:
			/* Built in function byte */
			if (ps_cnt[mp] == 1) {
				if (ps_type[mpp1] == CONSTANT) {
					if (ps_name(mpp1)._Length == 0) {
						/* Convert an integer constant to a string constant */
						DECIMAL(&ps_name(mpp1), ps_value[mpp1]);
					}
					ps_type[mpp1] = STRINGCON;
				}
				if (ps_type[mpp1] == STRINGCON) {
					/* Get the first byte of the string constant */
					ps_value[mp] = BYTE(&ps_name(mpp1), 0);
					/* Wipe out all traces of the builtin function */
					ps_name(mp)._Length = 0;  /* Not just housekeeping */
					ps_text(mp)._Length = 0;  /* Not just housekeeping */
					ps_type[mp] = CONSTANT;
					ps_bin[mp] = BIN_ACCUMULATOR;
					break;
				}
				createdescript(mpp1);
				/* Might be load or store.  Omit function name. */
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_zero);
				ps_type[mp] = SPECIAL;
			} else
			if (ps_cnt[mp] == 2) {
				forceaccumulator(mpp1, FIXEDTYPE, FALSE);
				CAT(&ps_text(mp), &ps_text(mp), &ps_text(mpp1));
				ps_type[mp] = SPECIAL;
			} else {
				too_many_arguments(mp, __LINE__);
			}
			break;
		case BIN_SHL:
			if (ps_cnt[mp] > 2) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			if (ps_cnt[mp] < 2) {
				too_few_arguments(mp, __LINE__);
				break;
			}
			arithemit(mp, AOP_SHL, mpp1);
			break;
		case BIN_SHR:
			if (ps_cnt[mp] > 2) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			if (ps_cnt[mp] < 2) {
				too_few_arguments(mp, __LINE__);
				break;
			}
			arithemit(mp, AOP_SHR, mpp1);
			break;
		case BIN_INPUT:
			if (ps_cnt[mp] > 1) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			forceaccumulator(mpp1, FIXEDTYPE, TRUE);
			j = get_temp();
			CAT_INT(synthesize_string, &input_descriptor, j);
			CAT(synthesize_string, synthesize_string, &bracket_comma_x1);
			CAT(synthesize_string, synthesize_string, &ps_text(mpp1));
			CAT(&ps_text(mp), synthesize_string, &close_paren);
			ps_type[mp] = DESCRIPT;
			break;
		case BIN_OUTPUT:
			if (ps_cnt[mp] > 1) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			forceaccumulator(mpp1, FIXEDTYPE, FALSE);
			ps_text(mp) = ps_text(mpp1);
			ps_type[mp] = SPECIAL;
			break;
		case BIN_FILE:
			if (ps_cnt[mp] < 2) {
				too_few_arguments(mp, __LINE__);
			} else
			if (ps_cnt[mp] == 2) {
				forceaccumulator(mpp1, DOUBLEWORD, FALSE);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_x1);
				ps_type[mp] = SPECIAL;
			} else {
				too_many_arguments(mp, __LINE__);
			}
			break;
		case BIN_INLINE:
			if (ps_type[mpp1] == CONSTANT) {
				if (ps_name(mpp1)._Length == 0) {
					/* Convert an integer constant to a string constant */
					__xpl_decimal(&ps_name(mpp1), ps_value[mpp1]);
				}
				ps_type[mpp1] = STRINGCON;
			}
			if (ps_type[mpp1] == STRINGCON) {
				CAT(&ps_text(mp), &ps_text(mp), &ps_name(mpp1));
			} else {
				CAT(&ps_text(mp), &ps_text(mp), &ps_text(mpp1));
			}
			ps_type[mp] = SPECIAL;
			break;
		case BIN_ADDR:
			if (ps_cnt[mp] > 1) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			switch (ps_type[mpp1]) {
			case BOOLEAN:
			case BYTETYPE:
			case HALFWORD:
			case FIXEDTYPE:
			case DOUBLEWORD:
			case CHRTYPE:
				CAT(&ps_text(mp), &ampersand, &ps_text(mpp1));
				ps_type[mp] = POINTER_TYPE;
				break;
			case DESCRIPT:
				ps_text(mp)._Length = ps_text(mpp1)._Length;
				ps_text(mp)._Address = ps_text(mpp1)._Address;
				ps_type[mp] = POINTER_TYPE;
				break;
			case PROC_VOID:
			case PROC_BIT1:
			case PROC_BIT8:
			case PROC_BIT16:
			case PROC_BIT32:
			case PROC_BIT64:
			case CHAR_PROC_TYPE:
				sym = ps_symtab[mpp1];
				CAT(&ps_text(mp), &ampersand, &syt_mapped(sym));
				ps_type[mp] = POINTER_TYPE;
				break;
			case DEFINED_LABEL:
			case FORWARD_LABEL:
			case FORWARD_CALL:
				sym = ps_symtab[mpp1];
				/* This is a non-standard C extension */
				CAT(&ps_text(mp), &ampersand_ampersand, &syt_mapped(sym));
				ps_type[mp] = POINTER_TYPE;
				break;
			case CHARFIXED:
				ps_text(mp) = ps_text(mpp1);
				ps_type[mp] = POINTER_TYPE;
				break;
			default:
				CAT_INT(synthesize_string, &bad_addr, ps_type[mpp1]);
				error(synthesize_string, __LINE__, 0);
				break;
			}
			break;
		case BIN_SADDR:
			if (ps_cnt[mp] > 1) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			if (ps_type[mpp1] == CHARFIXED) {
				ps_text(mp) = ps_text(mpp1);
				ps_type[mp] = POINTER_TYPE;
				break;
			}
			forcedescript(mpp1, 0);
			if (ps_type[mpp1] == CHRTYPE) {
				CAT(&ps_text(mp), &ps_text(mpp1), &dot_address);
			} else
			if (ps_type[mpp1] == DESCRIPT) {
				CAT(&ps_text(mp), &ps_text(mpp1), &point_address);
			} else {
				error(&oops, __LINE__, 1);
			}
			ps_type[mp] = POINTER_TYPE;
			break;
		case BIN_COREBYTE:
			if (ps_cnt[mp] > 1) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			forceaccumulator(mpp1, DOUBLEWORD, FALSE);
			CAT(synthesize_string, &byte_head, &ps_text(mpp1));
			CAT(&ps_text(mp), synthesize_string, &right_bracket);
			ps_type[mp] = BYTETYPE;
			ps_bin[mp] = BIN_ACCUMULATOR;
			break;
		case BIN_COREHALFWORD:
			if (ps_cnt[mp] > 1) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			forceaccumulator(mpp1, DOUBLEWORD, FALSE);
			CAT(synthesize_string, &halfword_head, &ps_text(mpp1));
			CAT(&ps_text(mp), synthesize_string, &halfword_tail);
			ps_type[mp] = HALFWORD;
			ps_bin[mp] = BIN_ACCUMULATOR;
			break;
		case BIN_COREWORD:
			if (ps_cnt[mp] > 1) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			forceaccumulator(mpp1, DOUBLEWORD, FALSE);
			CAT(synthesize_string, &word_head, &ps_text(mpp1));
			CAT(&ps_text(mp), synthesize_string, &word_tail);
			ps_type[mp] = FIXEDTYPE;
			ps_bin[mp] = BIN_ACCUMULATOR;
			break;
		case BIN_CORELONGWORD:
			if (ps_cnt[mp] > 1) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			forceaccumulator(mpp1, DOUBLEWORD, FALSE);
			CAT(synthesize_string, &longword_head, &ps_text(mpp1));
			if (sizeof_long == 4) {
				CAT(&ps_text(mp), synthesize_string, &word_tail);
			} else {
				CAT(&ps_text(mp), synthesize_string, &longword_tail);
			}
			ps_type[mp] = DOUBLEWORD;
			ps_bin[mp] = BIN_ACCUMULATOR;
			break;
		case BIN_BUILD_DESCRIPTOR:
			if (ps_cnt[mp] != 2) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			if (ps_type[mpp1] == POINTER_TYPE) {
				CAT(synthesize_string, &ps_text(mp),  &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &close_paren);
			} else {
				forceaccumulator(mpp1, FIXEDTYPE, FALSE);
				if (ps_bin[mpp1] != BIN_ACCUMULATOR) {
					static STRING ill_build =
						STR("Illegal argument to build_descriptor");

					error(&ill_build, __LINE__, 0);
					break;
				}
				CAT(synthesize_string, &ps_text(mp), &cast_void_star);
				CAT(synthesize_string, synthesize_string, &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &close_close);
			}
			ps_type[mp] = DESCRIPT;
			ps_bin[mp] = BIN_ACCUMULATOR;
			break;
		case BIN_XPRINTF:
			the_big_easy(mpp1);
			CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
			CAT(&ps_text(mp), synthesize_string, &close_paren);
			ps_type[mp] = FIXEDTYPE;
			ps_bin[mp] = BIN_ACCUMULATOR;
			break;
		case BIN_XSPRINTF:
		case BIN_XFPRINTF:
			if (ps_cnt[mp] < 2) {
				too_few_arguments(mp, __LINE__);
			}
			the_big_easy(mpp1);
			CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
			CAT(&ps_text(mp), synthesize_string, &close_paren);
			ps_type[mp] = FIXEDTYPE;
			ps_bin[mp] = BIN_ACCUMULATOR;
			break;
		default:
			break;
		}
		break;
	case PN_SUBSCRIPT_HEAD_1:
		/*  <subscript head> ::= <identifier> (   */
		/* Find the identifier in the symbol table */
		id_lookup(mp, FIXEDTYPE);
		sym = ps_symtab[mp];
		switch (ps_bin[mp]) {
		case BIN_CALL:
			CAT(&ps_text(mp), &ps_text(mp), &open_paren);
			break;
		case BIN_SCALAR:
			break;
		case BIN_ARRAY:
			/* Subscripted variable */
			CAT(&ps_text(mp), &ps_text(mp), &left_bracket);
			break;
		case BIN_STRING:
			CAT_INT(synthesize_string, &desc, syt_disp[sym]);
			CAT(&ps_text(mp), synthesize_string,
				&aop_string[AOP_ADD]);
			break;
		case BIN_SUBSTR:
			/* Add the temp descriptor for the substr() call */
			{
				static STRING sub = STR("(&descriptor[");

				i = get_temp();
				CAT_INT(synthesize_string, &sub, i);
				CAT(&ps_text(mp), synthesize_string, &bracket_comma_x1);
			}
			break;
		case BIN_BYTE:
		case BIN_FILE:
			ps_text(mp) = open_paren;
			break;
		case BIN_INLINE:
			ps_text(mp)._Length = 0;  /* Clear the function name */
			break;
		case BIN_XPRINTF:
			{
				static STRING bin_xprintf = STR("xprintf(0, 0, ");

				ps_text(mp) = bin_xprintf;
			}
			break;
		case BIN_XFPRINTF:
			{
				static STRING bin_xfprintf = STR("xprintf(1, (void *)(");

				ps_text(mp) = bin_xfprintf;
			}
			break;
		case BIN_XSPRINTF:
			{
				static STRING bin_xsprintf = STR("xprintf(");

				ps_text(mp) = bin_xsprintf;
			}
			break;
		default:
			break;
		}
		break;
	case PN_SUBSCRIPT_HEAD_2:
		/*  <subscript head> ::= <subscript head> <expression> ,    */
		ps_cnt[mp] = ps_cnt[mp] + 1;
		sym = ps_symtab[mp];

		/* Built in function or procedure call */
		switch (ps_bin[mp]) {
		case BIN_CALL:
			k = syt_type[sym];
			if (k >= PROC_VOID && k <= FORWARD_LABEL) {
				stuff_parameter(FALSE);
				CAT(&ps_text(mp), &ps_text(mp), &comma_x1);
			} else {
				static STRING no_fun =
					STR("IDENTIFIER(arg...) Is not a function!");

				error(&no_fun, __LINE__, 1);
			}
			break;
		case BIN_SCALAR:
			break;
		case BIN_ARRAY:
			break;
		case BIN_STRING:
			break;
		case BIN_BYTE:
			if (ps_cnt[mp] == 1) {
				createdescript(mpp1);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_x1);
				break;
			}
			/* Fall through */
		case BIN_SUBSTR:
			if (ps_cnt[mp] == 1) {
				forcedescript(mpp1, DESCRIPT);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_x1);
			} else
			if (ps_cnt[mp] == 2) {
				forceaccumulator(mpp1, FIXEDTYPE, FALSE);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_x1);
			} else {
				too_many_arguments(mp, __LINE__);
			}
			break;
		case BIN_SHL:
		case BIN_SHR:
			if (ps_type[mpp1] == CONSTANT) {
				ps_value[mp] = ps_value[mpp1];
				ps_type[mp] = ps_type[mpp1];
				ps_name(mp)._Length = 0;  /* Not just housekeeping */
				break;
			}
			forceaccumulator(mpp1, DOUBLEWORD, TRUE);
			ps_text(mp) = ps_text(mpp1);
			ps_type[mp] = ps_type[mpp1];
			break;
		case BIN_INLINE:
			if (ps_type[mpp1] == CONSTANT) {
				if (ps_name(mpp1)._Length == 0) {
					/* Convert an integer constant to a string constant */
					__xpl_decimal(&ps_name(mpp1), ps_value[mpp1]);
				}
				ps_type[mpp1] = STRINGCON;
			}
			if (ps_type[mpp1] == STRINGCON) {
				CAT(&ps_text(mp), &ps_text(mp), &ps_name(mpp1));
			} else {
				CAT(&ps_text(mp), &ps_text(mp), &ps_text(mpp1));
			}
			break;
		case BIN_FILE:
			if (ps_cnt[mp] == 1) {
				forceaccumulator(mpp1, FIXEDTYPE, FALSE);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_x1);
			} else {
				too_many_arguments(mp, __LINE__);
			}
			break;
		case BIN_BUILD_DESCRIPTOR:
			if (ps_cnt[mp] != 1) {
				too_many_arguments(mp, __LINE__);
				break;
			}
			i = get_temp();
			CAT_INT(synthesize_string, &build_descriptor_descriptor, i);
			CAT(synthesize_string, synthesize_string, &bracket_comma_x1);
			forceaccumulator(mpp1, FIXEDTYPE, FALSE);
			CAT(synthesize_string, synthesize_string, &ps_text(mpp1));
			CAT(&ps_text(mp), synthesize_string, &comma_x1);
			break;
		case BIN_XPRINTF:
			if (ps_cnt[mp] == 1) {
				forcedescript(mpp1, DESCRIPT);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_x1);
			} else {
				the_big_easy(mpp1);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_x1);
			}
			break;
		case BIN_XFPRINTF:
			if (ps_cnt[mp] == 2) {
				forcedescript(mpp1, DESCRIPT);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_x1);
			} else {
				the_big_easy(mpp1);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				if (ps_cnt[mp] == 1) {
					CAT(&ps_text(mp), synthesize_string, &close_comma_x1);
				} else {
					CAT(&ps_text(mp), synthesize_string, &comma_x1);
				}
			}
			break;
		case BIN_XSPRINTF:
			if (ps_cnt[mp] > 1) {
				the_big_easy(mpp1);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_x1);
				break;
			}
			if (ps_type[mpp1] == CHRTYPE) {
				static STRING two_comma_x1 = STR("2, ");
				CAT(&ps_text(mp), &ps_text(mp), &two_comma_x1);
			} else
			if (ps_type[mpp1] == CHARFIXED) {
				static STRING three_comma_x1 = STR("3, ");
				CAT(&ps_text(mp), &ps_text(mp), &three_comma_x1);
				createdescript(mpp1);
				CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
				CAT(&ps_text(mp), synthesize_string, &comma_x1);
				break;
			} else {
				illegal_argument(mp, __LINE__);
				break;
			}
			forcedescript(mpp1, DESCRIPT);
			CAT(synthesize_string, &ps_text(mp), &ps_text(mpp1));
			CAT(&ps_text(mp), synthesize_string, &comma_x1);
			break;
		default:
			too_many_arguments(mp, __LINE__);
			break;
		}
		break;
	case PN_CONSTANT_1:
		/*  <constant> ::= <string>    */
		ps_type[mp] = STRINGCON;
		break;
	case PN_CONSTANT_2:
		/*  <constant> ::= <number>    */
		ps_type[mp] = CONSTANT;
		break;
	default:
		error(&oops, __LINE__, 1);
		break;
	} /* Of case selection on production number */
#if 0 /* HUH */
	if (control['E'] & 1) {
		static STRING ps_text_head = STR("ps_text(");
		static STRING ps_text_tail = STR("): ");

		CAT_INT(synthesize_string, &ps_text_head, mp);
		CAT(synthesize_string, synthesize_string, &ps_text_tail);
		CAT(synthesize_string, synthesize_string, &ps_text(mp));
		OUTPUT(0, synthesize_string);
	}
#endif
	release_temp_descriptor(1);
}

/*              Syntactic parsing functions                              */

/*
**	dump_production(left_part)
**
**	Output the production about to be applied.
*/
void
dump_production(int left_part)
{
	static STRING replace = STR("  ::= ");
	STRING *dump_production_string, *dump_production_text;
	int i;

	dump_production_string = get_temp_descriptor();
	dump_production_text = get_temp_descriptor();
	CAT(dump_production_string, c2x_string(dump_production_text, v[left_part]),
		&replace);
	for (i = mp; i <= sp; i++) {
		CAT(dump_production_string, dump_production_string, &x1);
		CAT(dump_production_string, dump_production_string,
			c2x_string(dump_production_text, v[parse_stack[i]]));
	}
	OUTPUT(0, dump_production_string);
	release_temp_descriptor(2);
}

int
right_conflict(int left)
{
	/* This procedure is TRUE if token is not a legal right context of left */
	return (0xc0 & SHL(c1[left][SHR(token, 2)], SHL(token, 1) & 6)) == 0;
}


void
recover(void)
{
	static STRING resume = STR("Resume:");
	STRING *recover_string;

	/* If this is the second successive call to recover, discard one symbol */
	recover_string = get_temp_descriptor();
	if (!failsoft) scan();
	failsoft = FALSE;
	while (!stopit[token]) {
		scan();  /* To find something solid in the text */
	}
	while (right_conflict(parse_stack[sp])) {
		if (sp > 2) sp = sp - 1;  /* And in the stack */
		else scan();  /* But don't go too far */
	}
	CAT(recover_string, pad(&resume, text_used + 6), &vbar);
	OUTPUT(0, recover_string);
	release_temp_descriptor(1);
}

int
stacking(void)
{	/* Stacking decision function */
	static STRING ill_symbol = STR("Illegal symbol pair: ");
	STRING *stacking_string, *stacking_text;
	int i, j, k, l;

	count_stack = count_stack + 1;
	while(1) {
		switch (SHR(c1[parse_stack[sp]][SHR(token,2)],SHL(3-token,1)&6)&3) {
		case 0:
			stacking_string = get_temp_descriptor();
			stacking_text = get_temp_descriptor();
			CAT(stacking_string, &ill_symbol,
				c2x_string(stacking_text, v[parse_stack[sp]]));
			CAT(stacking_string, stacking_string, &x1);
			CAT(stacking_string, stacking_string,
				c2x_string(stacking_text, v[token]));
			error(stacking_string, __LINE__, 1);
			release_temp_descriptor(2);
			stack_dump();
			recover();
			break;
		case 1:
			return TRUE;  /* Stack token */
		case 2:
			return FALSE;  /* Don't stack it yet */
		case 3:
			/* Must check triples  */
			j = SHL(parse_stack[sp-1], 16) + SHL(parse_stack[sp], 8) + token;
			i = -1;
			k = NC1TRIPLES + 1;  /* Binary search of triples */
			while(i + 1 < k) {
				l = SHR(i + k, 1);
				if (c1triples[l] > j) k = l;
				else
				if (c1triples[l] < j) i = l;
				else return TRUE;  /* It is a valid triple */
			}
			return FALSE;
		} /* Of do case  */
	} /*  Of do forever */
}

int
pr_ok(int prd)
{
	/* Decision procedure for context check of equal or imbedded right parts */
	int h, i, j;

	switch (context_case[prd]) {
	case 0:
		/*  Case 0 -- no check required  */
		return TRUE;
	case 1:
		/*  Case 1 -- right context check  */
		return ! right_conflict(hdtb[prd]);
	case 2:
		/*  Case 2 -- left context check  */
		h = hdtb[prd] - NT;
		i = parse_stack[sp - prlength[prd]];
		for (j = left_index[h-1]; j < left_index[h]; j++) {
			if (left_context[j] == i) return TRUE;
		}
		return FALSE;
	case 3:
		/*  Case 3 -- check triples  */
		h = hdtb[prd] - NT;
		i = SHL(parse_stack[sp - prlength[prd]], 8) + token;
		for (j = triple_index[h-1]; j < triple_index[h]; j++) {
			if (context_triple[j] == i) return TRUE;
		}
		return FALSE;
	}  /* Of do case  */
	return FALSE;
}


/*                     Analysis algorithm                                  */

void
reduce(void)
{
	static STRING no_production = STR("No production is applicable");
	int i, j, prd;

	/* Pack the top four tokens on the parse stack into one word (j)  */
	for (i = sp - 4; i < sp; i++) {
		j = SHL(j, 8) + parse_stack[i];
	}
	for (prd = pr_index[parse_stack[sp]-1]; prd < pr_index[parse_stack[sp]]; prd++) {
		if ((prmask[prlength[prd]] & j) == prtb[prd]) {
			if (pr_ok(prd)) {
				/* An allowed reduction */
				mp = sp - prlength[prd] + 1;
				mpp1 = mp + 1;
				if (control['T'] & 1) {
					/* Used to help debug code generation */
					dump_production(hdtb[prd]);
				}
				synthesize(prdtb[prd]);

				sp = mp;
				parse_stack[sp] = hdtb[prd];
				return;
			}
		}
	}

	/* Look up has failed, error condition */
	error(&no_production, __LINE__, 1);
	stack_dump();
	failsoft = FALSE;
	recover();
}

void
compilation_loop(void)
{
	static STRING stack_overflow = STR("Stack overflow *** compilation aborted ***");

	stackmax = 0;
	while (compiling) {     /* Once around for each production (reduction)  */
		while(stacking()) {
			sp = sp + 1;
			if (sp > stackmax) stackmax = sp;
			if (sp == STACKSIZE) {
				error(&stack_overflow, __LINE__, 2);
				return;   /* Thus aborting compilation */
			}
			parse_stack[sp] = token;
			ps_name(sp) = bcd;
			ps_text(sp)._Length = 0;
			ps_value[sp] = number_value;
			ps_line[sp] = card_count;
			ps_type[sp] = 0;
			scan();
		}
		reduce();
	} /* Of do while compiling  */
}

/*
**	print_summary()
**
**	Summerize the compilation
*/
void
print_summary(void)
{
	static STRING cards = STR(" cards containing ");
	static STRING statements = STR(" statements were compiled.");
	static STRING no_errors = STR("No errors were detected.");
	static STRING errors = STR(" errors (");
	static STRING severe_detected = STR(" severe) were detected.");
	static STRING one_severe = STR("One severe error was detected.");
	static STRING one_error = STR("One error was detected.");
	static STRING last_detected = STR("The last detected error was on line ");
	STRING *print_summary_text;

	blank_line;
	print_summary_text = get_temp_descriptor();
	DECIMAL(print_summary_text, card_count);
	CAT(print_summary_text, print_summary_text, &cards);
	CAT_INT(print_summary_text, print_summary_text, statement_count);
	CAT(print_summary_text, print_summary_text, &statements);
	OUTPUT(0, print_summary_text);

	if (error_count == 0) {
		OUTPUT(0, &no_errors);
	} else
	if (error_count > 1) {
		DECIMAL(print_summary_text, error_count);
		CAT(print_summary_text, print_summary_text, &errors);
		CAT_INT(print_summary_text, print_summary_text, severe_errors);
		CAT(print_summary_text, print_summary_text, &severe_detected);
		OUTPUT(0, print_summary_text);
	} else
	if (severe_errors == 1) {
		OUTPUT(0, &one_severe);
	} else {
		OUTPUT(0, &one_error);
	}
	if (previous_error > 0) {
		CAT_INT(print_summary_text, &last_detected, previous_error);
		CAT(print_summary_text, print_summary_text, &period);
		OUTPUT(0, print_summary_text);
	}
	release_temp_descriptor(1);

	if (control['D'] & 1) {
		dumpit();
	}
}

/*
**	usage()
**
**	Display the usage message
*/
void
usage(void)
{
	printf("Usage:\n   xpl [-DGHIKLmMSTUWXY] [-v number] [-o outfile] [-s stringfile] file\n");
	printf("      -o outfile - Output file name for C code\n");
	printf("      -s stringfile - Output string header file name\n");
	printf("      -v number - Number of entries reserved for the argv array\n");
	printf("      file - XPL input source file name\n\n");
	printf("   Uppercase letters set initial values for compiler toggles\n");
	printf("      D - Dump stats at the end of compilation\n");
	printf("      G - 32 bit machine (2 Gig address space)\n");
	printf("      H - Hide identifiers from the C compiler\n");
	printf("      I - Ignore case for keywords and builtin functions\n");
	printf("      K - Emit Linemarkers for the C compiler\n");
	printf("      L - List source\n");
	printf("      m - Do not generate code for main()\n");
	printf("      M - Minimal source listing; takes precedence over -L\n");
	printf("      S - Dump symbol table at end of each procedure\n");
	printf("      T - Trace compiler productions\n");
	printf("      U - Give warning for unused variables\n");
	printf("      W - Inhibit warnings for undeclared variables and procedures\n");
	printf("      X - Show macro expansions\n");
	printf("      Y - Give warning for high order truncation\n");
}

int
main(int argv, char **argc)
{
	int i, j, k, show_usage = 0;
	char *kp;

	argv_limit = 32;
	temp_descriptor = DESCRIPTOR_STACK;
	for (i = 1; i < argv; i++) {
		if (argc[i][0] != '-') {
			/* Input file */
			input_filename = argc[i];
			continue;
		}
		if (i + 1 < argv && argc[i][2] == '\0') {
			if (argc[i][1] == 'o') {
				strncpy(output_filename, argc[i + 1], sizeof(output_filename));
				output_filename[sizeof(output_filename) - 1] = '\0';
				i++;
				continue;
			}
			if (argc[i][1] == 's') {
				strncpy(string_filename, argc[i + 1], sizeof(string_filename));
				string_filename[sizeof(string_filename) - 1] = '\0';
				i++;
				continue;
			}
			if (argc[i][1] == 'v') {
				j = atoi(argc[i + 1]);
				if (j >= 0 && j < 16384) {
					argv_limit = j;
				}
				i++;
				continue;
			}
		}
		for (j = 1; argc[i][j]; j++) {
			k = argc[i][j] & 255;
			if (isupper(k)) {
				control[k] = 1;
			} else
			if (k == 'm') {
				control[k] = 1;
			} else {
				show_usage = 1;
			}
		}
	}
	if (show_usage) {
		usage();
		return 1;
	}
	if (output_filename[0] == '\0') {
		if (!input_filename) {
			fprintf(stderr, "Output filename required\n");
			return 1;
		}
		/* Create the object filename from the input filename */
		if ((kp = strrchr(input_filename, '/'))) {	/* Should be \ on DOS */
			strcpy(output_filename, kp + 1);
		} else {
			strcpy(output_filename, input_filename);
		}
		i = strlen(output_filename);
		if (i > sizeof(output_filename) - 4) {
			fprintf(stderr, "Filename too long: %s\n", output_filename);
			return 1;
		}
		if ((kp = strrchr(output_filename, '.')) == (char *) 0) {
			kp = output_filename + i;
		}
		strcpy(kp, ".c");
	}
	if (string_filename[0] == '\0') {
		/* Create the string filename from the object filename */
		strcpy(string_filename, output_filename);
		i = strlen(string_filename);
		if (i > sizeof(string_filename) - 4) {
			fprintf(stderr, "Filename too long: %s\n", string_filename);
			return 1;
		}
		if ((kp = strrchr(string_filename, '.')) == (char *) 0) {
			kp = string_filename + i;
		}
		strcpy(kp, ".xh");	/* XPL header file */
	}
#if DEBUG_HASHER
	debug_file = fopen("hasher.txt", "w");
#endif
	if (initialize()) {
		return 1;
	}
	compilation_loop();
	emit_main_program();
	print_summary();
	/* Close any files that are still open */
	fclose(out_file);
	fclose(string_file);
	if (input_unit > 0) {
		xfclose(input_unit);
	}
	return (severe_errors > 0);
}

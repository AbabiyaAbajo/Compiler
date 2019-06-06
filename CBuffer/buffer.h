#pragma once
/*
* File Name:		buffer.h
* Compiler:			MS Visual Studio 2015
* Author:			Ababiya Abajobir 040873720
* Course:			CST 8152 - Compilers, Lab Section: 001
* Assignment:		1
* Date:				02 October 2018
* Professor:		Sv. Ranev
* Purpose:			To house my function declations, macro and constant definitions that are
					required to run my assignment 1
* Function list:	b_allocate(), b_addc(), b_clear(), b_free(), b_isfull(), b_limit(),
					b_capacity(), b_mark(), b_mode(), b_incfactor(), b_load(),
					b_isempty(), b_getc(), b_eob(), b_print(), b_compact(),
					b_rflag(), b_retract(), b_reset(), b_getcoffset(), b_rewind(),
					b_location(),
*/
#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

							/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

														   /* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

														   /* constant definitions */
#define RT_FAIL_1 -1			/* fail return value */
#define RT_FAIL_2 -2			/* fail return value */
#define LOAD_FAIL -2			/* load fail return value */
#define ADD_MODE 1				/* named constant for additive mode */
#define MULTI_MODE -1			/* named constant for multiplicative mode */
#define FIX_MODE 0				/* named constant for additive mode */
#define VALUE_1 1				/* generic named constant value of 1 */
#define VALUE_0 0				/* generic named constant value of 0 */
#define MAX_CAP SHRT_MAX-1		/* named constant for the maximum capacity */
#define NEW_FULL 0x100			/* named constant for 256 represented in hex value */
#define SUCCESS 0				/* Successfully completes task(s) */
#define DEFAULT_FALGS 0xFFFC	/* default flags value */
#define SET_EOB 0xFFFD          /* set eob mask */
#define RESET_EOB 0xFFFE		/* reset eob mask */
#define CHECK_EOB 0xFFFD		/* check eob mask */
#define SET_R_FLAG 0xFFFE		/* set r_flag mask */
#define RESET_R_FLAG 0xFFFD		/* reset r_flag mask */
#define CHECK_R_FLAG 0xFFFE		/* check r_flag mask */

#define IS_FULL(pBD) (((pBD) == NULL) ? (-1) : (((pBD->addc_offset) == pBD->capacity) ? (1) : (0)))

														   /* user data type declarations */
typedef struct BufferDescriptor {
	char *cb_head;				/* pointer to the beginning of character array (character buffer) */
	short capacity;				/* current dynamic memory size (in bytes) allocated to character buffer */
	short addc_offset;			/* the offset (in chars) to the add-character location */
	short getc_offset;			/* the offset (in chars) to the get-character location */
	short markc_offset;			/* the offset (in chars) to the mark location */
	char  inc_factor;			/* character array increment factor */
	char  mode;					/* operational mode indicator*/
	unsigned short flags;		/* contains character array reallocation flag and end-of-buffer flag */
} Buffer, *pBuffer;


/* function declarations */
Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode);
pBuffer b_addc(pBuffer const pBD, char symbol);
int b_clear(Buffer * const pBD);
void b_free(Buffer * const pBD);
int b_isfull(Buffer * const pBD);
short b_limit(Buffer * const pBD);
short b_capacity(Buffer * const pBD);
short b_mark(pBuffer const pBD, short mark);
int b_mode(Buffer * const pBD);
size_t b_incfactor(Buffer * const pBD);
int b_load(FILE * const fi, Buffer * const pBD);
int b_isempty(Buffer * const pBD);
char b_getc(Buffer * const pBD);
int b_eob(Buffer * const pBD);
int b_print(Buffer * const pBD);
Buffer * b_compact(Buffer * const pBD, char symbol);
char b_rflag(Buffer * const pBD);
short b_retract(Buffer * const pBD);
short b_reset(Buffer * const pBD);
short b_getcoffset(Buffer * const pBD);
int b_rewind(Buffer * const pBD);
char * b_location(Buffer * const pBD, short loc_offset);

#endif


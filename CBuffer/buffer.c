/*
* File Name: buffer.c
* Compiler: MS Visual Studio 2015
* Author: Ababiya Abajobir 040873720
* Course: CST 8152 - Compilers, Lab Section: 001
* Assignment: 1
* Date: 02 October 2018
* Professor: Sv. Ranev
* Purpose:	Function definitions that were declared in header file
* Function list:	b_allocate(), b_addc(), b_clear(), b_free(), b_isfull(), b_limit(),
					b_capacity(), b_mark(), b_mode(), b_incfactor(), b_load(),
					b_isempty(), b_getc(), b_eob(), b_print(), b_compact(),
					b_rflag(), b_retract(), b_reset(), b_getcoffset(), b_rewind(),
					b_location(),
*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "buffer.h"

/******************************************************************************************************************
Purpose:
Author: Ababiya Abajobir
History/Versions: v1 10/02/2018
Called functions: calloc, malloc, free and sizeof.
Parameters: short init_capacity with a range of 1 to SHRT_MAX - 1. char inc_factor with a range that is equal or
greater than 0. char o_mode can only be 'a' 'f' or 'm'
Return value: If successfull, the function return a pointer to the buffer struct. If inc_factor has a value lower than
0, init_capacity is equal or less than 0, init_capacity is greater than SHRT_MAX -1m, the calloc or the malloc was
not able to allocate memories
Algorithm:
******************************************************************************************************************/
Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode)
{

	Buffer *buff;

	/*Checks for invalid parameters*/
	if (init_capacity <= VALUE_0 || init_capacity > MAX_CAP || (unsigned char)inc_factor < VALUE_0 || o_mode != 'a' && o_mode != 'm' && o_mode != 'f')
		return NULL;

	/*Allocating memory for buff struct*/
	buff = (Buffer *)calloc(1, sizeof(Buffer));

	if (!buff)
	{
		/*Verifying if allocation was successful*/
		return NULL;
	}

	/*Allocating memory for buff struct*/
	buff->cb_head = (char *)malloc(init_capacity);
	if (!buff->cb_head)
	{
		/*Memory that was just assigned is freed*/
		free(buff);
		return NULL;
	}

	/*Sets buffer increment factor to 0 and the operation mode to FIX_MODE(0) if using operation 'f' and inc factor is equal to 0*/
	if (o_mode == 'f' || (unsigned char)inc_factor == VALUE_0) {
		buff->mode = FIX_MODE;
		buff->inc_factor = 0;

	}

	/*Sets buffer increment factor to 0 and the operation mode to FIX_MODE(0) if using operation 'f' and inc factor is not equal to 0*/
	else if (o_mode == 'f' && inc_factor != VALUE_0)
	{
		buff->mode = FIX_MODE;
		buff->inc_factor = 0;

	}

	/*Sets buffer increment factor to inc_factor and the operation mode to ADD_MODE(1) if using operation 'a' and inc factor is  equal or between 0 - 255*/
	else if (o_mode == 'a' && ((unsigned char)(inc_factor < NEW_FULL))) {
		buff->mode = ADD_MODE;
		buff->inc_factor = (unsigned char)inc_factor;
	}

	/*Sets buffer increment factor to inc_factor and the operation mode to MULTI_MODE(1) if using operation 'm' and inc factor is equal or between 0 - 100*/
	else if (o_mode == 'm' && (unsigned char)inc_factor <= 100) {
		buff->mode = MULTI_MODE;
		buff->inc_factor = (unsigned char)inc_factor;
	}
	else
	{
		/*Memory that was just assigned is freed*/
		free(buff->cb_head);
		free(buff);
		return NULL;
	}


	buff->capacity = init_capacity;
	buff->flags = DEFAULT_FALGS;
	return buff;
}

/******************************************************************************************************************
Purpose: Is to add 'symbol' to the buffer
Author: Ababiya Abajobir
History/Versions: v2 10/02/2018
Called functions: realloc()
Parameters: Constant buffer pointer of char* cb_head that is not NULL; char symbol that will be added to the char
buffer
Return value: If the size of addc_offset is euqal to the capacity, newCap is equal or smaller than zero, if capacity
is equal or great than MAX_CAP or if the realloc was not allocate, a NULL will be returned. If that isn't the case,
the function will return the buffer pointer.
Algorithm: Verifies operation buffer, invalid parameters or inability to increase on capacity, resizing buffer or
mode errors. If any of these are true, the function returns NULL. Otherwise, the function adds symbol to the char
buffer and calculates the new capacity.
******************************************************************************************************************/
pBuffer b_addc(pBuffer const pBD, char symbol)
{
	char *tempBuff;
	short newCap = 0, aSpace = 0, newInc = 0;
	tempBuff = pBD->cb_head;

	/*Statements checks for run-time errors and returns NULL if an error is found*/
	if (!pBD || !tempBuff)
		return NULL;

	pBD->flags &= RESET_R_FLAG;

	/*Verifies if there is space. If so, it adds the symbol, increments and returns the buffer*/
#ifdef IS_FULL
	if (IS_FULL(pBD) == SUCCESS) {
		pBD->cb_head[pBD->addc_offset++] = symbol;
		return pBD;
	}
#else /* !IS_FULL */
	if ((short)(pBD->addc_offset * sizeof(char)) < pBD->capacity) {
		pBD->cb_head[pBD->addc_offset++] = symbol;
		return pBD;
	}
#endif


	/*If mode is set to 1, it calculate new capacity of the buffer*/
	if (pBD->mode == ADD_MODE) {
		newCap = pBD->capacity + (unsigned char)pBD->inc_factor;
		/*Checking to see if the cap is greater than 0. If not, it returns NULL*/
		if (newCap <= 0)
			return NULL;

		pBD->capacity = newCap;
	}

	else if (pBD->mode == MULTI_MODE) {

		aSpace = MAX_CAP - pBD->capacity;
		newInc = ((short)aSpace * (unsigned char)((short)pBD->inc_factor)) / 100;
		newCap = pBD->capacity + newInc;
		/*Setting capacity to the max cap if it outside of parameters or to newCap in all other cases*/
		if (newCap >= MAX_CAP || pBD->capacity > newCap)
			pBD->capacity = MAX_CAP;
		else
			pBD->capacity = newCap;

	}
	else
		return NULL; /*Returns NULL if there's an error with the buffer mode*/

	/*Increasing the size of buffer*/
	tempBuff = (char *)realloc(pBD->cb_head, pBD->capacity);
	if (!tempBuff)
		return NULL; 	/*Verifying if allocation was successful*/

	/*Verifying that the memory location has changed*/
	if (tempBuff != pBD->cb_head)
	{
		pBD->flags |= SET_R_FLAG;
		pBD->cb_head = tempBuff;
	}

	/*Adding symbol into the buffer*/
	pBD->cb_head[pBD->addc_offset++] = symbol;
	tempBuff = NULL;

	return pBD;
}

/******************************************************************************************************************
Purpose:			Is to clear the buffer members so that its char buffer appears
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		Checks for run-time errors and returns -1 if found. Otherwise, 1 is returned.
Algorithm:
******************************************************************************************************************/
int b_clear(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;

	pBD->capacity = 0;
	pBD->addc_offset = 0;
	pBD->getc_offset = 0;
	pBD->markc_offset = 0;
	pBD->flags = DEFAULT_FALGS;

	return VALUE_1;
}

/******************************************************************************************************************
Purpose:			Is to free the memory allocted for buffer
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	free()
Parameters:			Constant buffer pointer
Return value:		This void function does not return anything
Algorithm:			If the buffer isn't empty/NULL, it frees the memory
******************************************************************************************************************/
void b_free(Buffer * const pBD)
{
	if (pBD) {
		free(pBD->cb_head);
		free(pBD);
	}
}

/******************************************************************************************************************
Purpose:			Is to check if the buffer has no more capacity to add a character
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer
Return value:		Function returns -1 in the even of run-time errors, 1 if there is no more
					room to add or 0 otherwise
Algorithm:
******************************************************************************************************************/
#ifndef IS_FULL

int b_isfull(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;
	/*This statement checks to see if the buffer is full. If it is, the function returns one*/
	if ((short)(pBD->addc_offset * sizeof(char)) == pBD->capacity)
		return VALUE_1;
	/*If the buffer isn't full, it returns 0*/
	return SUCCESS;
}

#endif /* !IS_FULL */


/******************************************************************************************************************
Purpose:			Is to obtain the buffer occupied size
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		The function returns -1 in the event of run-time errors. Otherwise, a (short)the number
					of char that occupy the buffer.
Algorithm:
******************************************************************************************************************/
short b_limit(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;

	return pBD->addc_offset;
}

/******************************************************************************************************************
Purpose:			Is to return the capacity of the buffer in bytes
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		The function returns the (short)number of bytes allocated to the buffer. If a run-time error
					is found, the function returns -1.
Algorithm:
******************************************************************************************************************/
short b_capacity(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;

	return pBD->capacity;
}

/******************************************************************************************************************
Purpose:			Is to set the mark location of the buffer
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL. A short that is used to set as the mark location within
					the range size in chars of the buffer.
Return value:		The function returns -1 if there is a run-time error or if the mark is outside the ranges of 0
					or greater than the total number of chars in the buffer.
Algorithm:
******************************************************************************************************************/
short b_mark(pBuffer const pBD, short mark)
{
	/*Statements checks if it's a run-time errors or if mark isn't within the limits of the buffer. If this is true, -1 is returned*/
	if (!pBD || mark < VALUE_0 || mark > pBD->addc_offset)
		return RT_FAIL_1;

	pBD->markc_offset = mark;
	return pBD->markc_offset;
}

/******************************************************************************************************************
Purpose:			Is to get the operational mode
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL.
Return value:		The function returns -2 if there is a run-time error. Otherwise, the mode is returned.
Algorithm:
******************************************************************************************************************/
int b_mode(Buffer * const pBD)
{
	/*This statements checks for run-tim errors. If this is true, -2 is returned*/
	if (!pBD)
		return RT_FAIL_2;

	return pBD->mode;
}

/******************************************************************************************************************
Purpose:			Is to get the increment factor
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL.
Return value:		The function returns 0x100 (256) if a run-time error is found. Otherwise, it returns a short
					value between 0 - 255.
Algorithm:
******************************************************************************************************************/
size_t b_incfactor(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns 0x100 if an error is found*/
	if (!pBD)
		return NEW_FULL;

	return (unsigned char)pBD->inc_factor;
}

/******************************************************************************************************************
Purpose:			Is to load the file into the buffer
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	b_addc(), fgetc(), feof(), ungetc() and printf().
Parameters:			Constant file pointer to open the file. Constant buffer pointer that is not NULL.
Return value:		The function returns (int) count of the number of chars if everything was successful. The
					function returns -1 in the event of run-time error. If there are no more chars to add, -2 is
					returned.
Algorithm:			Characters are read one at a time from the file  up to the end of file. When there are no more
					chars, a message is displayed that provides the last character as a char and int. Returns count
					of chars.
******************************************************************************************************************/
int b_load(FILE * const fi, Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD || !fi)
		return RT_FAIL_1;

	int input = (char)fgetc(fi); /*Variable to hold read character from file*/

	int count = 0; /*Counter of the amount of characters read into the buffer*/

				   /*Loops that breaks at end of file and increments count*/
	for (; !feof(fi); count++)
	{
		/*Displays a message when the end of file is attained along with the last char in int and char*/
		if ((b_addc(pBD, (unsigned char)input)) == NULL) {
			input = ungetc(input, fi);
			printf("The last character read from the file is: %c %d\n", input, (int)input);
			return LOAD_FAIL;
		}
		input = (char)fgetc(fi);
	}
	return count;
}

/******************************************************************************************************************
Purpose:			Is to check if the buffer is empty
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		The functions returns -1 in the event of run-time error, 1 if it empty or 0 if it's not empty.
Algorithm:
******************************************************************************************************************/
int b_isempty(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;

	/*Checks to see if buffer is not empty and returns 1 if this is true*/
	if (pBD->addc_offset == 0)
		return VALUE_1;

	return SUCCESS;
}

/******************************************************************************************************************
Purpose:			Is to get the char at position getc_offset
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		The function returns -2 in case of run-time error. If getc_offset and addc_offset are equal,
					sets the flag for EOB and returns 0. Otherwise, EOB is reset and the function returns the char
					located at get_offset
Algorithm:
******************************************************************************************************************/
char b_getc(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -2 if an error is found*/
	if (!pBD)
		return RT_FAIL_2;

	/*If the character location is the same addc_offset, the flags are reset using &(and) and then set for the end of bit using |(or)*/
	if (pBD->getc_offset == pBD->addc_offset) {
		pBD->flags &= RESET_EOB;
		pBD->flags |= SET_EOB;
		return SUCCESS;
	}
	/*Otherwise, the EOB is reset using & bitwise operations*/
	else {

		pBD->flags &= RESET_EOB;

	}
	return pBD->cb_head[(pBD->getc_offset)++];
}

/******************************************************************************************************************
Purpose:			Is to provide the value at the end of the buffer flag
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		If run-tim error, function returns int -1. Otherwise, function returns the eob bit value.
Algorithm:
******************************************************************************************************************/
int b_eob(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;

	return (pBD->flags & CHECK_EOB);
}

/******************************************************************************************************************
Purpose:			Is to print the chars in the buffer
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	b_isempty(), printf(), b_eob() and b_getc()
Parameters:			Constant buffer pointer that is not NULL
Return value:		If run-time error, function returns int -1. Otherwise, it returns the number of
					chars in the buffer.
Algorithm:			Checks for run-time errors and returns a value if found. Otherwise, it runs through the file
					and adds each char to the buffer while counting the number of chars. Once it gets to the end
					of file, a message is displayed and it prints out the number of characters that were added.
******************************************************************************************************************/
int b_print(Buffer * const pBD)
{
	int num_chars = 0;
	char symbol;

	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;

	/* Checks if the buffer is empty. If so, it prints out a message and returns the number of chars*/
	if (b_isempty(pBD)) {
		printf("Empty buffer!\n");

		return num_chars;
	}
	else {

		printf("%c", b_getc(pBD));

	}

	/*Loops through the file while the eob isn't 1 and displays the content in the buffer one char at a time as long as the value isn't 0. Number of char are counted*/
	while (b_eob(pBD) != CHECK_EOB && (symbol = b_getc(pBD)) != VALUE_0) {
		printf("%c", symbol);

		++num_chars;
	}
	printf("\n");

	return num_chars;
}

/******************************************************************************************************************
Purpose:			Is to modify the buffer capacity to the size of (char) buffer plus 1
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	free() and realloc()
Parameters:			Constant buffer pointer of char* cb_head that is not NULL; char symbol that will be added to the char
					buffer
Return value:		Function returns NULL in the event of run-time errors, the realloc did not work or if the
					capacity is outside of parameters. Otherwise, the buffer is returned
Algorithm:			Checks for run-time errors, capacity is not within parameters and if realloc was not able to
					successfully allocate memory. Otherwise, capacity is set to addc_offset in bytes, cb_head is
					resized, flag is set, symbol is added to the buffer and addc_offset is incremented
******************************************************************************************************************/
Buffer * b_compact(Buffer * const pBD, char symbol)
{
	char *tempBuff;
	/*Statements checks for run-time errors and returns NULL if an error is found*/
	if (!pBD)
		return NULL;

	pBD->capacity = (pBD->addc_offset + sizeof(char)) * sizeof(char);

	if (pBD->capacity < 0)
	{
		free(pBD->cb_head);
		free(pBD);
		return NULL;
	}

	tempBuff = pBD->cb_head;

	tempBuff = (char *)realloc(pBD->cb_head, (unsigned short)pBD->capacity);

	if (tempBuff == NULL)
		return NULL;
	/*Checks if tempBuff is no longer the same as cb_head. If so, it sets the flags using bitwise operations and updates cb_head*/
	if (tempBuff != pBD->cb_head)
	{
		pBD->flags &= RESET_R_FLAG;
		pBD->flags |= SET_R_FLAG;
		pBD->cb_head = tempBuff;
	}

	pBD->cb_head[pBD->addc_offset] = symbol;
	pBD->addc_offset++;

	return pBD;
}

/******************************************************************************************************************
Purpose:			Is to provide the value of the buffer realloc flag
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		Function returns the flag bit value to the bitwise & operations to the calling function
Algorithm:
******************************************************************************************************************/
char b_rflag(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;

	return (char)(pBD->flags & CHECK_R_FLAG);
}

/******************************************************************************************************************
Purpose:			Is to decrement getc_offset by 1
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		Function returns -1 in the event of a run-time error. Otherwise, it returns getc_offset
Algorithm:
******************************************************************************************************************/
short b_retract(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;

	pBD->getc_offset--;
	return pBD->getc_offset;
}

/******************************************************************************************************************
Purpose:			Is to set getc_offset to the value of markc_offset
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		If a run-time error is identified, -1 is returned. Otherwise, getc_offset is returned.
Algorithm:
******************************************************************************************************************/
short b_reset(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;

	pBD->getc_offset = pBD->markc_offset;
	return pBD->getc_offset;
}

/******************************************************************************************************************
Purpose:			Is to return getc_offset to the calling function
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		Function returns -1 in the event of run-time errors. Otherwise, it returns the getc_offset
Algorithm:
******************************************************************************************************************/
short b_getcoffset(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;

	return pBD->getc_offset;
}

/******************************************************************************************************************
Purpose:			Is to reset the get character location and the mark location to 0 in order to read the buffer
					again
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL
Return value:		Function returns -1 if there is a run-time error. Otherwise, it sets getc_offset and markc_offset
					to 0 and returns 0.
Algorithm:
******************************************************************************************************************/
int b_rewind(Buffer * const pBD)
{
	/*Statements checks for run-time errors and returns -1 if an error is found*/
	if (!pBD)
		return RT_FAIL_1;


	pBD->getc_offset = 0;
	pBD->markc_offset = 0;
	return SUCCESS;
}

/******************************************************************************************************************
Purpose:			Is to return the location of the character buffer
Author:				Ababiya Abajobir
History/Versions:	v1 10/02/2018
Called functions:	None
Parameters:			Constant buffer pointer that is not NULL. A short that is the distance from the char array
					cb_head
Return value:		Returns char location of buffer indicated by loc_offset. Otherwise, it returns NULL in the event
					of run-time error
Algorithm:
******************************************************************************************************************/
char * b_location(Buffer * const pBD, short loc_offset)
{
	/*Statements checks for run-time errors and returns NULL if an error is found*/
	if (!pBD)
		return NULL;

	/*Checks to see if the location is at the end or past the end of the buffer capacity*/
	if (loc_offset >= pBD->capacity)
		return NULL;

	return &(pBD->cb_head[loc_offset]);
}
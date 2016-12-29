/*File name: stable.h
 *Compiler: MS Visual Studio 2012
 *Authors:	Danilo Rodrigo, 040 808 827, 
 *			Lucas Pazelo Vargas de Oliveira, 040 799 663
 *Course: CST 8152 – Compilers, Labs Sections: 011 and 012
 *Assignment: 3
 *Date: March 27
 *Professor: Sv. Ranev
 *Purpose: This is the file header for stable.c
 *Function list: st_create(), st_install(), st_lookup(), st_update_type(),
 *st_update_value(), st_get_type(), st_destroy(), st_setsize(), st_incoffset(),
 *st_store(), st_print(), st_sort().
 */
#ifndef STABLE_H_
#define STABLE_H_

#include "buffer.h"

#define SF_ZERO    0x0000 /*0000 0000 0000 0000*/

#define SF_GET_LSB 0x0001 /*0000 0000 0000 0001*/

#define SF_DEFAULT 0xFFF8 /*1111 1111 1111 1000*/
#define SF_FPT     0xFFFA /*1111 1111 1111 1010*/
#define SF_IT      0xFFFC /*1111 1111 1111 1100*/
#define SF_STR     0xFFFE /*1111 1111 1111 1110*/

#define FAIL_FLAG -1  /*value returned when occurs some error in the function*/

typedef union InitialValue
{
	/* integer variable initial value */
	int int_val;
	/* floating-point variable initial value */
	float fpl_val;
	/* string variable initial value (offset) */
	int str_offset;
} InitialValue;

typedef struct SymbolTableVidRecord
{
	/* variable record status field*/
	unsigned short status_field;
	/* pointer to lexeme (VID name) in CA */
	char * plex;
	/* line of first occurrence */
	int o_line;
	/* variable initial value */
	InitialValue i_value;
	/*offset from the beginning of data segment*/
	size_t reserved;
} STVR;

typedef struct SymbolTableDescriptor
{
	/* pointer to array of STVR */
	STVR *pstvr;
	/* size in number of STVR elements */
	int st_size;
	/* offset in number of STVR elements */
	int st_offset;
	/* pointer to the lexeme storage buffer descriptor */
	Buffer *plsBD;
} STD;

/* Function prototypes */
STD st_create(int);
int st_install(STD, char *, char, int);
int st_lookup(STD, char *);
int st_update_type(STD, int, char);
int st_update_value(STD, int, InitialValue);
char st_get_type(STD, int);
void st_destroy(STD);
int st_print(STD);
int st_store(STD);
int st_sort(STD, char);

#endif /* STABLE_H_ */

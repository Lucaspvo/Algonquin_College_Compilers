/*File name: stable.c
 *Compiler: MS Visual Studio 2012
 *Authors:	Danilo Rodrigo, 040 808 827, 
 *			Lucas Pazelo Vargas de Oliveira, 040 799 663
 *Course: CST 8152 – Compilers, Labs Sections: 011 and 012
 *Assignment: 3
 *Date: March 27
 *Professor: Sv. Ranev
 *Purpose: This program will receive a lexeme from the scanner and will install it 
 *in a symbol table database structure.
 *Function list: st_create(), st_install(), st_lookup(), st_update_type(),
 *st_update_value(), st_get_type(), st_destroy(), st_setsize(), st_incoffset(),
 *st_store(), st_print(), st_sort().
 */
#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/* project header files */
#include "buffer.h"
#include "stable.h"

/* Access to global sym_table */
extern STD sym_table;

static void st_setsize(void);
static void st_incoffset(void);

/*Purpose: Function that creates the symbol table
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: malloc(), free()
Parameters: int st_size: 0/+32767
Return value: STD std : a copy to a symbol table descriptor
Algorithm:
*/
STD st_create(int st_size){
	int i;
	STD std;

	std.st_offset = 0;

	std.pstvr = (STVR*) malloc(sizeof(STVR)*st_size);
	if(std.pstvr == NULL){
		std.st_size = 0;
		return std;
	}
	std.plsBD = b_create(1, 1, 'a');
	if(std.plsBD == NULL){
		free(std.pstvr);
		std.st_size = 0;
		return std;
	}

	std.st_size = st_size;

	return std;
}

/*Purpose: Function that installs a lexeme into the symbol table
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: strlen(), st_lookup(), b_size(), b_addc(), b_rflag(), strcmp(), st_incoffset()
Parameters: STD sym_table : copy to a symbol table
			char * lexeme : [a-z], #
			chat type : S, I, F
			int line : [1-9]*
Return value: int : -1, [1-9]*
Algorithm: The function will receive a lexeme with its type. This lexeme will be transfered to a buffer
and then the pointer stvr.plex will point to the beginning of this lexeme in the buffer. After that will
be checked which type the lexeme is and then will be realized bitmask operations to set the status_field.
At the end will be checked the r_flag of the buffer to see if the buffer moved. If so, the plex pointers
of each STVR struct will be moved to the new place in memory.
*/
int st_install(STD sym_table, char *lexeme, char type, int line){
	STVR stvr;
	STVR* pstvr;
	Buffer* buffer;
	int strlen_lexeme = strlen(lexeme), i, r_flag = 0;
	int st_offset, indexBuffer, indexSTVR;

	if(sym_table.st_size == 0)
		return FAIL_FLAG;

	st_offset = st_lookup(sym_table, lexeme);

	if(st_offset != FAIL_FLAG)
		return st_offset;

	if(sym_table.st_offset >= sym_table.st_size)
		return FAIL_FLAG;

	stvr = sym_table.pstvr[sym_table.st_offset];

	buffer = sym_table.plsBD;

	if(b_size(buffer) == 0)
		stvr.plex = buffer->ca_head;
	else
		stvr.plex = &buffer->ca_head[b_size(buffer)];

	for(i = 0; i < strlen_lexeme; i++){
		
		buffer = b_addc(buffer, lexeme[i]);

		if(b_rflag(buffer))
			r_flag = 1;
	}
	 buffer = b_addc(buffer, '\0');

	 if(b_rflag(buffer))
			r_flag = 1;

	stvr.o_line = line;
	stvr.status_field = stvr.status_field & SF_ZERO;
	stvr.status_field = stvr.status_field | SF_DEFAULT;

	if(strcmp(&type, "I") == 0){
		stvr.status_field = stvr.status_field | SF_IT;
		stvr.i_value.int_val = 0;
	}
	else if(strcmp(&type, "F") == 0){
		stvr.status_field = stvr.status_field | SF_FPT;
		stvr.i_value.fpl_val = 0.0f;
	}
	else if(strcmp(&type, "S") == 0){
		stvr.status_field = stvr.status_field | SF_STR;
		stvr.status_field = stvr.status_field | SF_GET_LSB;
		stvr.i_value.str_offset = -1;
	}

	sym_table.pstvr[sym_table.st_offset] = stvr;

	pstvr = sym_table.pstvr;

	if(r_flag == 1){
		indexSTVR = 0;
		indexBuffer = 0;
		pstvr[indexSTVR].plex = buffer->ca_head;
		indexSTVR++;
		while(indexSTVR <= sym_table.st_offset){
			if(buffer->ca_head[indexBuffer] == '\0'){
				pstvr[indexSTVR].plex = &buffer->ca_head[indexBuffer+1];
				indexSTVR++;
			}
			indexBuffer++;
		}
	}

	sym_table.plsBD = buffer;

	st_offset = sym_table.st_offset;
	st_incoffset();
	return st_offset;
}

/*Purpose: Function that looks for a lexeme in the symbol table
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: strcmp()
Parameters: STD sym_table : copy to a symbol table
			char * lexeme : [1-9]*
Return value: int : -1, [1-9]*
Algorithm:
*/
int st_lookup(STD table, char * lexeme){
	
	int currentOffset = table.st_offset - 1;

	if(table.st_size == 0)
		return FAIL_FLAG;

	while(currentOffset > -1){
		STVR * stvr = &table.pstvr[currentOffset];

		char * vid = stvr->plex;

		if(strcmp(vid, lexeme) == 0)
			return currentOffset;
		--currentOffset;
	}
	return FAIL_FLAG;
}

/*Purpose: Function that update the type of the variable identifier
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: 
Parameters: STD sym_table : copy to a symbol table
			int vid_offset : [1-9]*
			char v_type : S, I, F
Return value: int : [1-9]*
Algorithm:
*/
int st_update_type(STD sym_table,int vid_offset,char v_type){
	if(sym_table.st_size == 0)
		return FAIL_FLAG;

	if(sym_table.pstvr[vid_offset].status_field & SF_GET_LSB)
		return FAIL_FLAG;

	sym_table.pstvr[vid_offset].status_field &= SF_ZERO;

	switch (v_type)
	{
	case 'I':
		sym_table.pstvr[vid_offset].status_field |= SF_IT;
		break;

	case 'F':
		sym_table.pstvr[vid_offset].status_field |= SF_FPT;
		break;
	default:
		return FAIL_FLAG;
	}

	sym_table.pstvr[vid_offset].status_field |= SF_GET_LSB;
	return vid_offset;

}

/*Purpose: Function update the value of an variable indentify
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: None
Parameters: STD table, int vidOffset, InitialValue initialValue
Return value: -1 (fail), vidOffset (int)
Algorithm: Function get the type of and variable indentify
*/
int st_update_value(STD table, int vidOffset, InitialValue initialValue){
	if(table.st_size == 0)
		return FAIL_FLAG;

	table.pstvr[vidOffset].i_value = initialValue;
	return vidOffset;
}

/*Purpose: Function get tipe of a variable indentify
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: None
Parameters: STD sym_table, int vid_offset
Return value: void
Algorithm: Function get the type of and variable indentify
*/
char st_get_type(STD sym_table, int vid_offset){

	STVR * stvr;

	unsigned short mask;

	if(sym_table.st_size == 0)
		return FAIL_FLAG;

	stvr = &sym_table.pstvr[vid_offset];

	mask = stvr->status_field & SF_STR;

	if(mask == SF_IT)
		return 'I';
	if(mask == SF_FPT)
		return 'F';
	if(mask == SF_STR)
		return 'S';
	return FAIL_FLAG;
}

/*Purpose: Function release the memory of symbol table
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: free, b_destroy, st_setsize
Parameters: STD sym_table
Return value: void
Algorithm: Function call the b_destroy funtion to buffer plsBD,
free to pstvr if they are not null, and call the funtion st_setsize 
*/
void st_destroy(STD sym_table){
	
	if(sym_table.plsBD)
		b_destroy(sym_table.plsBD);

	if(sym_table.pstvr)
		free(sym_table.pstvr);

	st_setsize();
}

/*Purpose: Function set the st_offset to 0
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: None
Parameters: void
Return value: void
Algorithm: Function set the st_offset to 0
*/
void st_setsize(void){
	sym_table.st_size = 0;
}

/*Purpose: Function increase by one the st_offset
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: None
Parameters: void
Return value: void
Algorithm: Function increase the st_offset by one
*/
static void st_incoffset(void){
	sym_table.st_offset++;
}

/*Purpose: Function store the symbol table content on a file
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: fopen,fprintf, st_get_type
Parameters: STD sym_table
Return value: -1 (fail) or i (index)
Algorithm: Fuction opens a file and put the content of the 
symbol table if it is not empty
*/
int st_store(STD sym_table){
	FILE * outFile;
    int i;

    if (sym_table.st_size == 0)
        return FAIL_FLAG;


    outFile = fopen("$stable.ste", "w");

    if (!outFile)
        return FAIL_FLAG;

	fprintf(outFile, "%d\n", sym_table.st_size);

	for  (i = 0; i < sym_table.st_size ; i++)
	{
		fprintf(outFile, "%4x", sym_table.pstvr[i].status_field);

		fprintf(outFile, "%d", (int) strlen(sym_table.pstvr[i].plex));

		fprintf(outFile, "%s", sym_table.pstvr[i].plex);

		fprintf(outFile, "%d", sym_table.pstvr[i].o_line);
		
		switch (st_get_type(sym_table, i))
		{
		case 'I':
			fprintf(outFile, "%d", sym_table.pstvr[i].i_value.int_val);
			break;

		case 'F':
			fprintf(outFile, "%d", sym_table.pstvr[i].i_value.fpl_val);
			break;

		case 'S':
			fprintf(outFile, "%d", sym_table.pstvr[i].i_value.str_offset);
			break;
		}
	}

	fclose(outFile);
	printf( "\nSymbol Table stored.\n");
	return i;
}

/*Purpose: Function prints the symbol table content
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: printf
Parameters: STD sym_table
Return value: int
Algorithm: The function receive an STD variable and prints its content
if it is not empty
*/
int st_print(STD sym_table){
	Buffer* buffer;
	STVR* stvr;
	int indexSTVR, i;
	char c;

	if(sym_table.st_size == 0)
		return FAIL_FLAG;

	buffer = sym_table.plsBD;
	stvr = sym_table.pstvr;

	printf("\nSymbol Table\n");
	printf("____________\n\n");
	printf("Line Number Variable Identifier\n");
	for(indexSTVR = 0; indexSTVR < sym_table.st_offset; indexSTVR++)
		printf("%2d          %s\n", stvr[indexSTVR].o_line, stvr[indexSTVR].plex);
}

/*Purpose: Function returns 0
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 March 27
Called functions: None
Parameters: STD sym_table, char s_order
Return value: 0
Algorithm: Function just return 0
*/
int st_sort(STD sym_table, char s_order){
	return 0;
}

/*File name: table.h
 *Compiler: MS Visual Studio 2012
 *Authors:	Danilo Rodrigo, 040 808 827, 
 *			Lucas Pazelo Vargas de Oliveira, 040 799 663
 *Course: CST 8152 – Compilers, Labs Sections: 011 and 012
 *Assignment: 2
 *Date: February 05
 *Professor: Sv. Ranev
 *Purpose: This is the file header for scanner.c
 *Function list: aa_func02(), aa_func03(), aa_func05(), aa_func08(), aa_func10(), aa_func12().
 */

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif
#define IS_N_KW -3			/*Variable used for when the lexeme is not a keyword*/
//#define ERR_MALLOC  -10        /*Variable returned in case a malloc doesnt allocate memory*/
#define ERR_LEN_W_DOT 17

/*   Source end-of-file (SEOF) sentinel symbol
 *    '\0' or only one of the folowing constants: 255, 0xFF , EOF
 */
#define SEOF 255
/*  Single-lexeme tokens processed separately one by one
 *  in the token-driven part of the scanner
 *  '=' , ' ' , '(' , ')' , '{' , '}' , == , <> , '>' , '<' ,
 *       space
 *  !<comment , ',' , '"' , ';' , '-' , '+' , '*' , '/', << ,
 *  .AND., .OR. , SEOF, 'wrong symbol',
 */
 

//REPLACE *ESN* WITH YOUR ERROR STATE NUMBER 
#define ES  12 /* Error state */
#define IS -1    /* Inavalid state */

/* State transition table definition */

//REPLACE *CN* WITH YOUR COLUMN NUMBER  

#define TABLE_COLUMNS 7
/*transition table - type of states defined in separate table */
int  st_table[ ][TABLE_COLUMNS] = {
/* State 0 */   {1,6,4,4,IS,IS,IS},
/* State 1 */   {1,1,1,1,2,3,2},
/* State 2 */   {IS,IS,IS,IS,IS,IS,IS},
/* State 3 */   {IS,IS,IS,IS,IS,IS,IS},
/* State 4 */   {ES,4,4,4,7,5,5},
/* State 5 */   {IS,IS,IS,IS,IS,IS,IS},
/* State 6 */   {ES,11,9,ES,7,ES,5},
/* State 7 */   {8,7,7,7,8,8,8},
/* State 8 */   {IS,IS,IS,IS,IS,IS,IS},
/* State 9 */   {ES,9,9,ES,ES,ES,10},
/* State 10 */  {IS,IS,IS,IS,IS,IS,IS},
/* State 11 */  {ES,ES,ES,ES,ES,ES,10},
/* State 12 */  {IS,IS,IS,IS,IS,IS,IS},
/* State 13 */  {IS,IS,IS,IS,IS,IS,IS}
};
 
/* Accepting state table definition */
#define ASWR     2  /* accepting state with retract */
#define ASNR     3  /* accepting state with no retract */
#define NOAS     4  /* not accepting state */

int as_table[ ] = { NOAS, NOAS, ASWR, ASNR, NOAS, ASWR, NOAS, NOAS, ASWR, NOAS, ASWR, NOAS, ASNR, ASWR };

/* Accepting action function declarations */

Token aa_func02(char *lexeme); 
Token aa_func03(char *lexeme); 
Token aa_func05(char *lexeme); 
Token aa_func08(char *lexeme); 
Token aa_func10(char *lexeme); 
Token aa_func12(char *lexeme); 
//Token aa_func13(char *lexeme); 


/* defining a new type: pointer to function (of one char * argument) 
   returning Token
*/  

typedef Token (*PTR_AAF)(char *lexeme);


/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
 * Token (*aa_table[])(char lexeme[]) = {
 */

PTR_AAF aa_table[ ] ={
	NULL,
	NULL,
	aa_func02,
	aa_func03,
	NULL,
	aa_func05,
	NULL,
	NULL,
	aa_func08,
	NULL,
	aa_func10,
	NULL,
	aa_func12
	//aa_func13

};

/* Keyword lookup table (.AND. and .OR. are not keywords) */

#define KWT_SIZE  8

char * kw_table []= {
                      "ELSE",
                      "IF",
                      "INPUT",
                      "OUTPUT",
                      "PLATYPUS",
                      "REPEAT",
                      "THEN",
                      "USING"   
                     };

#endif

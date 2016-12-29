/*File name: scanner.c
 *Compiler: MS Visual Studio 2012
 *Authors:	Danilo Rodrigo, 040 808 827, 
 *			Lucas Pazelo Vargas de Oliveira, 040 799 663
 *Course: CST 8152 – Compilers, Labs Sections: 011 and 012
 *Assignment: 2
 *Date: February 05
 *Professor: Sv. Ranev
 *Purpose: This program is a scanner that reads the content from a file loaded to our buffer
 *and from character to character determines if it goes to a switch for special characters
 *or if it goes to a transition table in which will choose a final state for a lexeme.
 *Function list: scanner_init(), mlwpar_next_token(), get_next_state(), char_class(),
 *aa_func02(), aa_func03(), aa_func05(), aa_func08(), aa_func10(), aa_func12(),
 *atool(), iskeyword().
 */

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
 * to suppress the warnings about using "unsafe" functions like fopen()
 * and standard sting library functions defined in string.h.
 * The define does not have any effect in Borland compiler projects.
 */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */
#include <math.h>

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"
#include "stable.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
   It is defined in platy_st.c */
/* Access to global sym_table */
extern STD sym_table;
extern Buffer * str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

/* Local(file) global objects - variables */
static Buffer *lex_buf;/*pointer to temporary lexeme buffer*/

/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */ 
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */
static long atool(char * lexeme); /* converts octal string to decimal value */

/*Purpose: Function that initializes the scanner
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: b_isempty(), b_setmark(), b_retract_to_mark(), b_reset()
Parameters: Buffer * sc_buf: pointer to the buffer
Return value: int: 0
Algorithm:
*/
int scanner_init(Buffer * sc_buf) {
  	if(b_isempty(sc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	b_setmark(sc_buf, 0);
	b_retract_to_mark(sc_buf);
	b_reset(str_LTBL);
	line = 1;
	return EXIT_SUCCESS;/*0*/
/*   scerrnum = 0;  *//*no need - global ANSI C */
}

/*Purpose: Function that reads each char in the buffer and finds the appropriate final state for each lexeme
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: isalnum(), b_getc(), b_getc_offset(), get_next_state(),
b_retract(), malloc(), free(), aa_table[](), b_size(), b_addc()
Parameters: Buffer * sc_buf: pointer to the buffer
Return value: Token: reference to a token
Algorithm: In this function, a char is read from the buffer and if it is a digit or a letter,
it goes to the first if of this function, where it will be determined to which final state
the lexeme starting in that character will go, otherwise the char goes to a switch that "treats"
special chars cases.
*/
Token mlwpar_next_token(Buffer * sc_buf)
{
   Token t; /* token to return after recognition */
   unsigned char c; /* input symbol */
   int state = 0; /* initial state of the FSM */
   short lexstart;  /*start offset of a lexeme in the input buffer */
   short lexend;    /*end   offset of a lexeme in the input buffer */
   int accept = NOAS; /* type of state - initially not accepting */                                     
/* 
lexstart is the offset from the beginning of the char buffer of the
input buffer (sc_buf) to the first character of the current lexeme,
which is being processed by the scanner.
lexend is the offset from the beginning of the char buffer of the
input buffer (sc_buf) to the last character of the current lexeme,
which is being processed by the scanner.
*/ 
        
        
        //DECLARE YOUR VARIABLES HERE IF NEEDED 
		char * lexeme = NULL;
        int lexsize, i = 0, j = 0;
		char aux;
                
        while (1){ /* endless loop broken by token returns it will generate a warning */
                
        //GET THE NEXT SYMBOL FROM THE INPUT BUFFER 
        
        

        c = b_getc(sc_buf); 

/* special cases or token driven processing */

//WRITE YOUR CODE FOR PROCESSING THE SPECIAL CASES HERE. 
//COMMENTS AND STRING LITERALS ARE ALSO PROCESSED HERE.
//
//WHAT FOLLOWS IS A PSEUDO CODE. YOU CAN USE switch STATEMENT
//INSTEAD OF if-else TO PROCESS THE SPECIAL CASES
//DO NOT FORGET TO COUNT THE PROGRAM LINES
//   
		if(isalnum(c)){
			lexstart = b_getc_offset(sc_buf) - 1;

			//b_setmark(sc_buf, lexstart);
			state = get_next_state(state, c, &accept);

			while(accept == NOAS){
				c = b_getc(sc_buf);
				state = get_next_state(state, c, &accept);
			}

			if(c == '#' || state == 12)
				b_getc(sc_buf);
			
			lexend = b_getc_offset(sc_buf) - 1;

			lexsize = (int) (lexend - lexstart);
			
			for(j = 0; j < lexsize+1; ++j)
				b_retract(sc_buf);


			lexeme = (char *) malloc(sizeof(char) * (size_t) lexsize + 1);

			for (i = 0; i < lexsize; ++i)
				lexeme[i] = b_getc(sc_buf);

			lexeme[i] = '\0';

			t = aa_table[state](lexeme);

			free(lexeme);
			return t;
		}
		
		/*Switch for special character cases*/
		switch (c)
		{

		case '=':

			c = b_getc(sc_buf);
			if(c == '='){
				t.code = REL_OP_T; t.attribute.rel_op = EQ; return t;	
			}
			b_retract(sc_buf);
			t.code = ASS_OP_T; return t;

		case ' ':
			break;

		case '\t':
			break;

		case '!':
			c = b_getc(sc_buf);
			if (c != '<'){
				t.code = ERR_T; 
				t.attribute.err_lex[0] = '!';
				t.attribute.err_lex[1] = c;	
				t.attribute.err_lex[2] = '\0';
				
				while(c != '\n'){
					c = b_getc(sc_buf);	
				}
				b_retract(sc_buf);
				return t;
			}else{
				while(c != '\n'){
					c = b_getc(sc_buf);	
				}
				b_retract(sc_buf);
			}
			break;
			
		case '(':
			t.code = LPR_T; return t; 

		case ')':
			t.code = RPR_T; return t; 

		case '{':
			t.code = LBR_T; return t; 

		case '}':
			t.code = RBR_T; return t; 

		case '>':
			t.code = REL_OP_T; t.attribute.rel_op = GT; return t;

		case '<':
			c = b_getc(sc_buf);
			if(c == '>'){
				t.code = REL_OP_T; t.attribute.rel_op = NE; return t;	
			}else if (c == '<'){
				t.code = SCC_OP_T; return t;
			}
			b_retract(sc_buf);
			t.code = REL_OP_T; t.attribute.rel_op = LT; return t;

		case ',':
			t.code = COM_T; return t; 

		case '"':
			{

				lexstart = b_getc_offset(sc_buf) - 1;

				c = b_getc(sc_buf);
				while(c != '"'){

					if(c == '\n')
						++line;
					else if(c == SEOF || c == '\0'){

						lexend = b_getc_offset(sc_buf) - 1;

						lexsize = (int) (lexend - lexstart);

						
						for(j = 0; j < lexsize+1; ++j)
							b_retract(sc_buf);
						
						if(lexsize > ERR_LEN){
							for(i = 0; i < lexsize; ++i){
								c = b_getc(sc_buf);
								if(i < ERR_LEN_W_DOT)
									t.attribute.err_lex[i] = c;
								else if(i < ERR_LEN)
									t.attribute.err_lex[i] = '.';
								else if(i == ERR_LEN)
									t.attribute.err_lex[i] = '\0';
						
							}
						}else{
							for(i = 0; i < lexsize; ++i){
								c = b_getc(sc_buf);
								t.attribute.err_lex[i] = c;
							}
							t.attribute.err_lex[i] = '\0';
						}
						

						t.code = ERR_T;
						return t;
					}
					
					c = b_getc(sc_buf);
			
			}

			
			lexend = b_getc_offset(sc_buf) - 1;

			lexsize = (int) lexend - lexstart;

			t.attribute.str_offset = b_size(str_LTBL); 

			for(j = 0; j < lexsize+1; ++j)
				b_retract(sc_buf);

			for(i = 0; i < lexsize+1; ++i){
				c = b_getc(sc_buf);
				if(c == '"')
					continue;

				b_addc(str_LTBL, c);
			}

			b_addc(str_LTBL, '\0');
			t.code = STR_T;
			return t;
			}

		case ';':
			t.code = EOS_T; return t; 

		case '-':
			t.code = ART_OP_T; t.attribute.arr_op = MINUS; return t; 

		case '+':
			t.code = ART_OP_T; t.attribute.arr_op = PLUS; return t; 

		case '*':
			t.code = ART_OP_T; t.attribute.arr_op = MULT; return t; 

		case '/':
			t.code = ART_OP_T; t.attribute.arr_op = DIV; return t;

		case '.':
			aux = c;
			c = b_getc(sc_buf);

			lexstart = b_getc_offset(sc_buf) - 1;
			lexend = lexstart;
			
			if(c == 'O'){
				c = b_getc(sc_buf);
				if(c == 'R'){
					c = b_getc(sc_buf);
					if(c == '.'){ /*Checks to see if it is the logical operator OR*/
						t.code = LOG_OP_T; t.attribute.log_op = OR; return t;
					}else
						lexend = b_getc_offset(sc_buf) - 1;
				}else
					lexend = b_getc_offset(sc_buf) - 1;
			}else if(c == 'A'){
				c = b_getc(sc_buf);
				if(c == 'N'){
					c = b_getc(sc_buf);
					if(c == 'D'){
						c = b_getc(sc_buf);
						if(c == '.'){ /*Checks to see if it is the logical operator AND*/
							t.code = LOG_OP_T; t.attribute.log_op = AND; return t;
						}else
							lexend = b_getc_offset(sc_buf) - 1;
					}else
						lexend = b_getc_offset(sc_buf) - 1;
				}else
					lexend = b_getc_offset(sc_buf) - 1;
			}
			/*In the case that is not both logical operator, the lexstart and lexend are set to the begining and end of OR or AND to retract to their begining
			and treat them as Variable Identifiers*/
			lexsize = (int) (lexend - lexstart);
			for(j = 0; j < lexsize+1; ++j)
				b_retract(sc_buf);
			t.code = ERR_T; t.attribute.err_lex[0] = aux; t.attribute.err_lex[1] = '\0'; return t;

		case '\n':
			++line;
			continue;

		case SEOF:
			t.code = SEOF_T; return t;

		case '\0':
			//printf("%d\n",line);
			t.code = SEOF_T; return t;

		default:
			t.code = ERR_T; t.attribute.err_lex[0] = c; t.attribute.err_lex[1] = '\0'; return t;
		}            
              
   }//end while(1)
}

/*Purpose: Returns the next state in the transition table
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: char_class(), assert()
Parameters: int: [0-12]
			char: [a-z], ., #, [0-9], other characters
			int: 2, 3, 4
Return value: int: [0-12]
Algorithm:
*/
int get_next_state(int state, char c, int *accept)
{
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
printf("Input symbol: %c Row: %d Column: %d Next: %d \n",c,state,col,next);
#endif

       assert(next != IS);

#ifdef DEBUG
	if(next == IS){
	  printf("Scanner Error: Illegal state:\n");
	  printf("Input symbol: %c Row: %d Column: %d\n",c,state,col);
	  exit(1);
	}
#endif
	*accept = as_table[next];
	//printf("Input symbol: %c Row: %d Column: %d Next: %d \n",c,state,col,next);
	return next;
}

/*Purpose: This Function will return the column of which the char belongs in the transition table
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: isdigit(), isalpha()
Parameters: char: [a-z], [0-9]
Return value: int: 1,2,3,4,5,6
Algorithm:
*/
int char_class (char c)
{
        /*int val = 0;*/

	if(isdigit( (int) c )){
			int cv = c - '0';
			return (cv == 0) ? 1 : (cv <= 7) ? 2 : 3;
		}	
	
	if(isalpha( (int) c )){
			return 0;
		}
		
		
		return (c == '.') ? 4 : (c == '#') ? 5 : 6;
}

/*Purpose: Accepting function 02 - This will check if the lexeme is a keyword or an arithmetic variable identifier
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: iskeyword(), strlen(), strcpy()
Parameters: char: 0/+127
Return value: Token
Algorithm: Checks to see if the lexeme is a keyword,
if it is not, returns the lexeme as a variable identifier
*/
Token aa_func02(char lexeme[]){

	Token t;
	int index, i, offset;
	char* cAux = NULL;
	
	index = (iskeyword(lexeme));
    if(index != IS_N_KW){
	   t.code = KW_T;
	   t.attribute.kwt_idx = index;
	   return t;
   }

   t.code = AVID_T;

   if(strlen(lexeme) > VID_LEN){
	   cAux = (char*) malloc(sizeof(char)*(VID_LEN +1));
	   strncpy(cAux, lexeme, VID_LEN);
	   cAux[VID_LEN] = '\0';
	   lexeme = cAux;
   }

   if(cAux)
	   free(cAux);
   
   offset = st_install(sym_table, lexeme, 'S', line);
   
   if(offset == FAIL_FLAG){
		printf("\nError: The Symbol Table is full - install failed.\n");
		st_store(sym_table);
		exit(EXIT_FAILURE);
    }

   t.attribute.vid_offset = offset;

  return t;
}

/*Purpose: Accepting function 03 - String Variable Identifier
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: strlen(), strncpy()
Parameters: char: 0/+127
Return value: Token
Algorithm:
*/
Token aa_func03(char lexeme[]){

	Token t;
	int i, offset;
	char* cAux = NULL;

	t.code = SVID_T;
	
	if(strlen(lexeme) >= VID_LEN){
		cAux = (char*) malloc(sizeof(char)*(VID_LEN+2));
		strncpy(cAux, lexeme, VID_LEN);
		cAux[VID_LEN-1] = '#';
		cAux[VID_LEN] = '\0';
		lexeme = cAux;
	}

	if(cAux)
	   free(cAux);

	offset = st_install(sym_table, lexeme, 'S', line);

	if(offset == FAIL_FLAG){
		printf("\nError: The Symbol Table is full - install failed.\n");
		st_store(sym_table);
		exit(EXIT_FAILURE);
    }

	t.attribute.vid_offset = offset;
	//free(cAux);

  return t;
}

/*Purpose: Accepting function 08 - Floating Point Literal
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: atof(), strncpy()
Parameters: char: 0/+127
Return value: Token
Algorithm:
*/
Token aa_func08(char lexeme[]){

	Token t;
	double lexem;

	lexem = strtod(lexeme, NULL);

	if(lexem > FLT_MAX || lexem < 0.0f || (lexem < FLT_MIN && lexem != 0.0f)){
			
			t.code = ERR_T;
			strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
			t.attribute.err_lex[ERR_LEN] = '\0';
			return t;
	}

	t.code = FPL_T;
	t.attribute.flt_value = lexem;

  return t;
}

/*Purpose: Accepting function 05 - Decimal Constant
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: atoi(), strncpy()
Parameters: char: 0/+127
Return value: Token
Algorithm:
*/
Token aa_func05(char lexeme[]){

	Token t;
	long integer;

	integer = (long)atoi(lexeme);

	//printf("%d", integer);
	//getchar();

	if(integer > SHRT_MAX || strlen(lexeme) > INL_LEN){
		t.code = ERR_T;
		strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
		t.attribute.err_lex[ERR_LEN] = '\0';
		return t;
	}

	t.code = INL_T;
	t.attribute.int_value = (int)integer;

  return t;
}

/*Purpose: Accepting function 10 - Octal Constant
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: atool(), strncpy()
Parameters: char: 0/+127
Return value: Token
Algorithm:
*/
Token aa_func10(char lexeme[]){

	Token t;
	long val;

	val = atool(lexeme);
	if(val > SHRT_MAX){
		t.code = ERR_T;
		strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
		t.attribute.err_lex[ERR_LEN] = '\0';
		return t;
	}
	t.code = INL_T;
	t.attribute.int_value = (int)val;
	return t;
}

/*Purpose: Accepting function 12 - Error State
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: strlen(), strcpy()
Parameters: char: 0/+127
Return value: Token
Algorithm:
*/
Token aa_func12(char lexeme[]){

	Token t;

	t.code = ERR_T;

	if(strlen(lexeme) > ERR_LEN){
		strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
		t.attribute.err_lex[ERR_LEN] = '\0';
		return t;
	}

	strcpy(t.attribute.err_lex, lexeme);

  return t;
}

/*Purpose: Function responsible for converting an octal representing string in an integer
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: strtol()
Parameters: char: 0/+127
Return value: long: 0/+2147483647
Algorithm:
*/
long atool(char * lexeme){
	char * ptc;
	long intOctal;

	intOctal = strtol(lexeme, &ptc, 8);
	
	return intOctal; 
}

/*Purpose: Function returns if it is or not a keyword
Authors: Danilo Rodrigo, Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 February 06
Called functions: strcmp()
Parameters: char: 0/+127
Return value: int: [0-7], -3
Algorithm: Function responsible for returning the index of the keyword table if the lexeme is a keyword or
return IS_N_KW otherwise
*/
int iskeyword(char * kw_lexeme){
	int i;
	if(kw_lexeme != NULL){
		for(i = 0; i < KWT_SIZE; i++){
			if(strcmp(kw_lexeme, kw_table[i]) == 0)
				return i;
		}
		return IS_N_KW;
	}
}
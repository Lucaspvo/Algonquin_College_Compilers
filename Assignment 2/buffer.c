/**************************************************************************
File name:          buffer.c
Compiler:           MS Visual Studio 2012
Author:             Danilo Rodrigo Cavalcante Bandeira, 040 808 827
Course:             CST 8152 – Compilers,
Lab Section:        011
Assignment:         1
Date:               January 29, 2015
Professor:          Sv. Ranev
Purpose:            Definition of functions that are used to control the buffer
Function list: 		b_create(), b_addc(), b_reset(), b_destroy(), 
					b_isfull(),	b_size(), b_capacity(), b_mark(), 
					b_setmark(), b_mode(), b_inc_factor(), b_load(), 
					b_isempty(), b_eob(), b_getc(), b_print(), b_pack(), 
					b_rflag(), b_retract(), b_retract_to_mark(), 
					b_getc_offset()
**************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include "buffer.h"

/**************************************************************************
Purpose:			Initialize the Buffer  
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   malloc, calloc
Parameters:         short init_capacity,char inc_factor,char o_mode
Return value:       Buffer *
Algorithm:			After test for wrong parameter, the function will 
					allocate memory for the buffer and your descriptor.
					After that, will set the operation mode according
					with the parameter, and set inc_factor as well.
					Finnally, will set the capacity and return a pointer 
					for the buffer descriptor.
**************************************************************************/
Buffer * b_create(short init_capacity,char inc_factor,char o_mode){

	/* create 2 pBuffer variables, the buffer pointer and one temporary pointer */
	pBuffer pBD, tPointer;
	char * tPointerChar;
	
	/* check for a invalid parameters */
	if(inc_factor > CHAR_MAX || inc_factor < CHAR_MIN)
		return NULL;
	else if(o_mode > CHAR_MAX || o_mode < CHAR_MIN)
		return NULL;
	else if(init_capacity > SHRT_MAX || init_capacity < SHRT_MIN)
		return NULL;

	tPointer = (pBuffer) calloc(1,sizeof (Buffer));
	if(tPointer == 0 || tPointer == NULL){ /* check if the memory was allocated */
		/*printf("No memory enough to create a new Buffer Handle\n");*/
		return NULL;
	}else{
		pBD = tPointer;/* set the buffer descriptor pointer with the temporary buffer value */
	}
	tPointerChar = (char*) malloc(init_capacity * sizeof(char));
	if(tPointerChar == 0 || tPointerChar == NULL){/* check if the memory was allocated */
		/*printf("No memory enough to create a new Buffer\n");*/
		return NULL;
	}else{
		pBD->ca_head = tPointerChar;/* set the buffer pointer with the temporary buffer value */
	}

	/* set the inc_factor and mode according the parameters */
	if(o_mode == 'f' || inc_factor == 0){
		pBD->mode = 0;
		pBD->inc_factor = 0;
	}else if(o_mode == 'f' && inc_factor != 0){
		pBD->mode = 0;
		pBD->inc_factor = 0;
	}else if(o_mode == 'a' && (inc_factor >= 1 && inc_factor <= 255)){
		pBD->mode = 1;
		pBD->inc_factor = inc_factor;
	}else if(o_mode == 'm' && (inc_factor >= 1 && inc_factor <= 100)){
		pBD->mode = -1;
		pBD->inc_factor = inc_factor;	
	}else{
		/*printf("Wrong parameter\n");*/
		return NULL;
	}

	/* set the capacity and return the pointer */
	pBD->capacity = init_capacity * (short int) sizeof(char);

	return pBD;
}

/**************************************************************************
Purpose:            Add elements in the Buffer
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   realloc
Parameters:         pBuffer const pBD, char symbol
Return value:       pBuffer
Algorithm:			After check wrong parameters, the function will check
					if the buffer is not full, if not will add the new 
					symbol. If the buffer is full, the function will check
					the operation mode, if is fixed the buffer will stop, 
					if it is in add mode will try allocate new space using
					the inc_factor for new size in bytes. Finally, if the 
					mode is multiplicative, the function will try allocate
					memory using a specig calc, otherwise will retun NULL.
**************************************************************************/
pBuffer b_addc(pBuffer const pBD, char symbol){
	/* variable to keep the new size of the buffer */
	short int newSize;

	/* check for a null pointer or an invalid char value before performs the task */
	if(symbol >= CHAR_MIN && symbol <= CHAR_MAX && pBD != NULL){
		pBD->r_flag = 0;

		if(pBD->addc_offset < pBD->capacity){/* check is have space */
			pBD->ca_head[pBD->addc_offset] = symbol;/* add the char */
			++pBD->addc_offset;/* increment the addc_offset */
			return pBD;
		}else{
			/* fixed mode */
			if(pBD->mode == 0)
				return NULL;
			/* additive mode */
			else if(pBD->mode == 1){
				/* calculate the new capacity */
				newSize = (pBD->inc_factor * sizeof(char) + pBD->capacity);
				/* if is bigger than 0 */
				if(newSize > 0){
					{
						/* create a temporary pointer to a char */
						char * tempPM1;

						tempPM1 = (char*) realloc(pBD->ca_head, newSize);
						/* check if the memory was allocated */
						if(tempPM1 == 0 || tempPM1 == NULL){
							/*printf("No memory enough to create a new Buffer");*/
							return NULL;
						}else{
							/* set the buffer pointer, add symbol, increment the addc, set the new capacity, 
							set the r_flag to indicates resize and return the pointer */
							pBD->ca_head = tempPM1;
							pBD->ca_head[pBD->addc_offset] = symbol;
							++pBD->addc_offset;
							pBD->capacity = newSize;
							pBD->r_flag = 1;
							return pBD;
						}

					}
				}else{
					return NULL;
				}
			/* multiplicative mode */
			}else if(pBD->mode == -1){
				/* check if the buffer limit was reached */
				if(pBD->capacity == SHRT_MAX) 
					return NULL;
				else{
					{
						/* variables to calculate the new size, and temporary pointer to a char */
						short int avaliableSpace, newIncrement, newCapacity;
						char * tempPM2;

						/* calculates the new size */
						avaliableSpace = SHRT_MAX - pBD->capacity;
						newIncrement = (avaliableSpace * pBD->inc_factor) / 100;
						newCapacity = pBD->capacity + newIncrement;

						tempPM2 = (char*) realloc(pBD->ca_head, newCapacity);
						/* check if the memory was allocated */
						if(tempPM2 == 0 || tempPM2 == NULL){
							printf("No memory enough to create a new Buffer");
							return NULL;
						}else{
							/* set the buffer pointer, add symbol, increment the addc, set the new capacity, 
							set the r_flag to indicates resize and return the pointer */
							pBD->ca_head = tempPM2;
							pBD->ca_head[pBD->addc_offset] = symbol;
							++pBD->addc_offset;
							pBD->capacity = newCapacity;
							pBD->r_flag = 1;
							
							return pBD;
						}
					}
				}
			}
		}
		return pBD;
	}else{
		return NULL;
	}
}


/**************************************************************************
Purpose:            Reset the BufferDescriptor
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       int
Algorithm:			If the pointer is valid will, set to 0 some flags.
**************************************************************************/
int b_reset (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL) 
		return R_FAIL_1;
	else{
		pBD->addc_offset = 0;
		pBD->eob = 0;
		pBD->getc_offset = 0;
		pBD->mark_offset = 0;
		pBD->r_flag = 0;
		
		return CORRECT_0_FLAG;
	}

}


/**************************************************************************
Purpose:            Free memory of the Buffer and BufferDescriptor 
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   free
Parameters:         Buffer * const pBD
Return value:       void
Algorithm:  
**************************************************************************/
void b_destroy(Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL){
		free(pBD->ca_head);
		free(pBD);
	}
	
}


/**************************************************************************
Purpose:            Test if the buffer is full
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       int
Algorithm:  
**************************************************************************/
int b_isfull (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL){
		if(pBD->capacity == pBD->addc_offset)
			return 1;
		else
			return 0;
	}
	return R_FAIL_1;

	
}


/**************************************************************************
Purpose:            Return the size
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       short
Algorithm:  
**************************************************************************/
short b_size (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL) 
		return pBD->addc_offset;
	return R_FAIL_1;
	
}


/**************************************************************************
Purpose:            Return the capacity
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       short
Algorithm:  
**************************************************************************/
short b_capacity(Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL) 
		return pBD->capacity;
	return R_FAIL_1;
}


/**************************************************************************
Purpose:            Set the mark_offset
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD, short mark
Return value:       char *
Algorithm:  
**************************************************************************/
char * b_setmark(Buffer * const pBD, short mark){
	/* check for a null pointer or an invalid mark value before performs the task */
	if(pBD == NULL || (mark <= SHRT_MIN && mark >= SHRT_MAX))
		return NULL;
	/* check if the mark is within the buffer limite */
	else if(mark >= 0 && mark <= pBD->addc_offset){
		pBD->mark_offset = mark;
		return &pBD->ca_head[mark];
	}
	return NULL;

}

/**************************************************************************
Purpose:            Return the mark_offset
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       short
Algorithm:  
**************************************************************************/
short b_mark (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL) 
		return pBD->mark_offset;
	return R_FAIL_1;
	
}


/**************************************************************************
Purpose:            Return the mode
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       int
Algorithm:  
**************************************************************************/
int b_mode (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL) 
		return pBD->mode;
	return R_FAIL_1;
}

/**************************************************************************
Purpose:            Return the inc_factor
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       size_t
Algorithm:  
**************************************************************************/
size_t b_inc_factor (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL) 
		return (unsigned int) pBD->inc_factor;
	return R_FAIL_256;
}


/**************************************************************************
Purpose:            Load a file inside the buffer
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   fgetc, b_addc
Parameters:         FILE * const fi, Buffer * const pBD
Return value:       int
Algorithm:			After chek the parameters the function will read
					char by char from the file and load in the buffer.
**************************************************************************/
int b_load (FILE * const fi, Buffer * const pBD){
	/* char used to keep the char readed from the file */
	char c;
	/* check for a null pointer and a valide file before performs the task */
	if(fi == NULL || pBD == NULL)
		return R_FAIL_1;
	else{
		/* read first element */
		c = (char) fgetc(fi);
		/* check for the end of the file */
		while(feof(fi) == 0)
			{
				/*  add the char readed into the buffer */
				if(b_addc(pBD, c) == NULL) 
					return LOAD_FAIL;
				/* read the next char */
				c = (char) fgetc(fi);
			}
			
			
	}
	return CORRECT_0_FLAG;
}


/**************************************************************************
Purpose:            Inform if the buffer is empty
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       int
Algorithm:  
**************************************************************************/
int b_isempty (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD == NULL) 
		return R_FAIL_1;
	else if(pBD->addc_offset == 0) 
		return SET_R_FLAG;
	return CORRECT_0_FLAG;
}


/**************************************************************************
Purpose:            Return eob
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       int
Algorithm:  
**************************************************************************/
int b_eob (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL) 
		return pBD->eob;
	return R_FAIL_1;
}


/**************************************************************************
Purpose:            Return the element at getc_offset position
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       char
Algorithm:			After check the parameters, the function will if the 
					getc_offset reached the end of the buffer and set the 
					eob. If not, will return the char at getc_offset 
					position.
**************************************************************************/
char b_getc (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD == NULL) 
		return -2;
	/* check if the getc reached the end of the buffer */
	else if(pBD->addc_offset == pBD->getc_offset){
		/* set eob to true */
		pBD->eob = 1;
		return R_FAIL_1;
	}
	{
	/* keep the element at getc position, set eob to false, increment getc and return the char */
	char aux = pBD->ca_head[pBD->getc_offset];
	pBD->eob = 0;
	++pBD->getc_offset;
	return aux;
	}
	
}

/**************************************************************************
Purpose:            Print the buffer elements
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   b_getc, printf
Parameters:         Buffer * const pBD
Return value:       int
Algorithm:			After check the parameter, the function will the if 
					the buffer is empty. If not, will go through the buffer
					and print all elements.
**************************************************************************/
int b_print (Buffer * const pBD){
	/*int i;*/
	char c;
	/* check for a null pointer before performs the task */
	if(pBD == NULL)
		return R_FAIL_1;
	/* check if the buffer is empty, print a message, and set getc to 0 */
	else if(pBD->addc_offset == 0){
		printf("The buffer is empty.\n");
		pBD->getc_offset = 0;
		return 0;
	}
	pBD->getc_offset = 0;
	/* get the firt element of the buffer */
	c = b_getc(pBD);
	/* print the char inside the buffer until reach the end of buffer */
	while(pBD->eob != 1){
		printf("%c", c);/* print element */
		c = b_getc(pBD);/* get next element */
	}
	/*for (i = 0; i < pBD->addc_offset; i++)
	{
		printf("%c", pBD->ca_head[i]);
	}*/

	printf("\n");
	{
		/* set eob and getc to 0 for a new b_print call */
		pBD->getc_offset = 0;
		pBD->eob = 0;
		return CORRECT_0_FLAG;	
	}
}


/**************************************************************************
Purpose:            Reduce unused space for the buffer
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   realloc
Parameters:         Buffer * const pBD
Return value:       Buffer *
Algorithm:			After check the parameters, the function will set 
					memory size used by the buffer, to the number of 
					elements plus one.
**************************************************************************/
Buffer * b_pack(Buffer * const pBD){
	char* tPointer;

	/* check for a null pointer before performs the task */
	if(pBD == NULL)
		return NULL;
	else{
		/* try allocate memory for the number of elements inside the buffer */
		tPointer = (char*)realloc(pBD->ca_head, pBD->addc_offset + 1);
		/* check if the memory was allocated */
		if(tPointer == 0 || tPointer == NULL){
			/* printf("No memory enough to create a new Buffer Handle\n"); */
			return NULL;
		}else{
			/* set the pointer , r_flag and new capacity*/
			pBD->ca_head = tPointer;
			pBD->r_flag = 1;
			pBD->capacity = pBD->addc_offset + 1;
			return pBD;
		}
	}
	
}


/**************************************************************************
Purpose:            Return r_flag
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         short init_capacity,char inc_factor,char o_mode
Return value:       char
Algorithm:  
**************************************************************************/
char b_rflag (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL) 
		return pBD->r_flag;
	return R_FAIL_1;
}


/**************************************************************************
Purpose:            Decrement the getc_offset
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       short
Algorithm:  
**************************************************************************/
short b_retract (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL){
		--pBD->getc_offset;
		return pBD->getc_offset;
	}
	return R_FAIL_1;
}


/**************************************************************************
Purpose:            Set the getc_offset with mark_offset value
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       short
Algorithm:  
**************************************************************************/
short b_retract_to_mark (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD == NULL){
		pBD->getc_offset = pBD->mark_offset;
		return pBD->getc_offset;
	}
	return R_FAIL_1;
}


/**************************************************************************
Purpose:            Return getc_offset
Author:             Danilo Rodrigo Cavalcante Bandeira
History/Versions:   1.0, January 29, 2015
Called functions:   None
Parameters:         Buffer * const pBD
Return value:       short
Algorithm:  
**************************************************************************/
short b_getc_offset (Buffer * const pBD){
	/* check for a null pointer before performs the task */
	if(pBD != NULL) 
		return  pBD->getc_offset;	
	return R_FAIL_1;
}




/*File name: buffer.c
 *Compiler: MS Visual Studio 2012
 *Author: Lucas Pazelo Vargas de Oliveira, 040 799 663
 *Course: CST 8152 – Compilers, Lab Section: 012
 *Assignment: 1
 *Date: January 29
 *Professor: Sv. Ranev
 *Purpose: This program allocats memory for a buffer, add characters in it, manage its size
 *and its data. It will create and manage a buffer to add and read the content from it.
 *Function list: b_create(),  b_addc(), b_reset ()  b_destroy(), b_isfull (), b_size (), b_capacity(),
 *b_setmark (), b_mark (), b_mode (), b_inc_factor (), b_load (), b_isempty (), b_eob (), b_getc (),
 *b_print (), b_pack(), b_rflag (), b_retract (), b_retract_to_mark (), b_getc_offset () 
 */

#include "buffer.h"

/*Purpose: This function creates and allocats memory for a buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions: calloc(), malloc(), free()
Parameters: init_capacity: short, 0/+32767
			inc_factor: char; 0, 1-100, 1-255
			o_mode: char; 'f', 'a', 'm'
Return value: pointer to the Buffer strcture
Algorithm: First of all it decides if the parameters are valid, then
it tries to alocate memory for the buffer and then save the data
sent as parameters in the buffer's fields
*/
Buffer * b_create(short init_capacity,char inc_factor,char o_mode){
	if(init_capacity == -1 || (inc_factor < 0 && o_mode == 'm'))
		return NULL;
	if(init_capacity == 0 && o_mode == 'f')
		return NULL;

	pbuffer = (pBuffer) calloc(1, sizeof (Buffer));
	if(pbuffer == NULL)
		return NULL;

	pbuffer->ca_head = (char *)malloc(init_capacity*sizeof(char));
	if(pbuffer->ca_head == NULL){
		free(pbuffer);
		return NULL;
	}

	if( o_mode == 'f' || inc_factor == 0){ /*Checks to see if the buffer will be fixed mode*/
		pbuffer->mode = 0;
		pbuffer->inc_factor = 0;
	}else if(o_mode == 'f' && inc_factor != 0){ /*Checks to see if the buffer will be fixed mode*/
		pbuffer->mode = 0;
		pbuffer->inc_factor = 0;
	}else if(o_mode == 'a'){ /*Checks to see if the buffer will be additive mode*/
		pbuffer->mode = 1;
		pbuffer->inc_factor = inc_factor;
	}else if(o_mode == 'm' && (inc_factor > 0 && inc_factor <= MAX_INC_FACTOR_M)){/*Checks to see if the buffer will be multiplicative mode*/
		pbuffer->mode = -1;
		pbuffer->inc_factor = inc_factor;
	}else{ /*returns NULL if non of the options were passed in the parameters*/
		printf("None of the programmed options");
		free(pbuffer->ca_head);
		free(pbuffer);
		return NULL;
	}

	pbuffer->capacity = init_capacity * sizeof(char);

	return pbuffer;
}

/*Purpose: This function save characters in its allocated memory
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions: realloc()
Parameters: pBD: pBuffer const
			symbol: char, 0/+127
Return value: pointer to the Buffer strcture
Algorithm: This function will check the parameters if they are valid. Then it will see if
the size of the current buffer is the same as the capacity, if it is not, then another char
is added to the buffer, otherwise the program will decide its next step by looking the
buffer's mode. If the buffer is fixed, it simply returns NULL. if it is additive, then it
will add the inc_factor to the capacity. And if it is multiplicative, it will do some
calculations to determine the new size of the buffer and return a pointer to the buffer reallocated
*/
pBuffer b_addc(pBuffer const pBD, char symbol){
	short newCapacity, availableSpace, newIncrement;
	char * auxPointer;

	if(pBD == NULL)
		return NULL;

	pBD->r_flag = 0;

	if(pBD->addc_offset < pBD->capacity){ /*Checks to see if the buffer is full*/
		pBD->ca_head[pBD->addc_offset] = symbol;
		pBD->addc_offset++;
		return pBD;
	}else{ /*It will check which mode this buffer is*/
		if(pBD->mode == 0) /*If it is fixed buffer, return NULL*/
			return NULL;
		else if(pBD->mode == 1){ /*If it is additive, add inc_factor to the capacity*/
			newCapacity = ((unsigned char)pBD->inc_factor * sizeof(char)) + pBD->capacity;
			if(newCapacity > 0){
				auxPointer = (char*) realloc(pBD->ca_head, newCapacity);
				if(auxPointer == NULL)
					return NULL;
				if(pBD->ca_head != auxPointer){
					pBD->r_flag = 1;
					pBD->ca_head = auxPointer;
				}
				pBD->ca_head[pBD->addc_offset] = symbol;
				pBD->addc_offset++;
				pBD->capacity = newCapacity;
				return pBD;
			}else
				return NULL;
		}else if(pBD->mode == -1){ /*If it is multiplicative, calculate a new size for the buffer*/
			if(pBD->capacity == SHRT_MAX)
				return NULL;
			else{
				availableSpace = SHRT_MAX - pBD->capacity;
				newIncrement = (availableSpace * pBD->inc_factor) / 100;
				newCapacity = pBD->capacity + newIncrement ;
				if(newCapacity == pBD->capacity){
					newCapacity = sizeof(char)*SHRT_MAX;
					auxPointer = (char*) realloc(pBD->ca_head, newCapacity);
					if(auxPointer == NULL)
						return NULL;
					if(pBD->ca_head != auxPointer){
						pBD->r_flag = 1;
						pBD->ca_head = auxPointer;
					}
					pBD->ca_head[pBD->addc_offset] = symbol;
					pBD->addc_offset++;
					pBD->capacity = newCapacity;
					return pBD;
				}
				auxPointer = (char*) realloc(pBD->ca_head, newCapacity);
				if(auxPointer == NULL)
					return NULL;
				if(pBD->ca_head != auxPointer){
					pBD->r_flag = 1;
					pBD->ca_head = auxPointer;
				}
				pBD->ca_head[pBD->addc_offset] = symbol;
				pBD->addc_offset++;
				pBD->capacity = newCapacity;
				return pBD;
			}
		}else
			return NULL;
	}
}

/*Purpose: Resets the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: int, -1, 0
Algorithm:
*/
int b_reset (Buffer * const pBD){
	if(pBD == NULL)
		return R_FAIL_1;
	pBD->addc_offset = 0;
	pBD->eob = 0;
	pBD->getc_offset = 0;
	pBD->mark_offset = 0;
	pBD->r_flag = 0;
	return R_OK_0;
}

/*Purpose: Destroies the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions: free()
Parameters: pBD: Buffer * const
Return value: void
Algorithm:
*/
void b_destroy(Buffer * const pBD){
	if(pBD == NULL)
		return;
	free(pBD->ca_head);
	free(pBD);
}

/*Purpose: Checks to see if the buffer is full
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: int, -1, 0, 1
Algorithm:
*/
int b_isfull (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	if(pBD->capacity == pBD->addc_offset) /*If non is invalid, returns 1 if the buffer is full*/
		return 1;
	return 0;
}

/*Purpose: Returns the size of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: short, 0/+32767, -1
Algorithm:
*/
short b_size (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	return pBD->addc_offset;
}

/*Purpose: Returns the capacity of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: short, 0/+32767, -1
Algorithm:
*/
short b_capacity(Buffer * const pBD){
	if(pBD == NULL)
		return R_FAIL_1;
	else if(pBD->capacity > 0)
		return pBD->capacity;
	return R_FAIL_1;
}

/*Purpose: Sets the mark_offset flag of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
			mark: short, 0/+32767
Return value: *char
Algorithm:
*/
char * b_setmark (Buffer * const pBD, short mark){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return NULL;
	else if(mark < 0)
		return NULL;
	else if(mark <= pBD->addc_offset){
		pBD->mark_offset = mark;
		return &pBD->ca_head[mark];
	}
	return NULL;
}

/*Purpose: Returns the mar_offset of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: short, 0/+32767, -1
Algorithm:
*/
short b_mark (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	return pBD->mark_offset;
}

/*Purpose: Returns the mode of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: int, -2, 0, 1
Algorithm:
*/
int b_mode (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_2;
	else if(pBD->eob < INT_MIN || pBD->eob > INT_MAX)
		return R_FAIL_2;
	return pBD->mode;
}

/*Purpose: Returns the inc_factor of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: size_t (unsigned int), 0/+32767
Algorithm:
*/
size_t b_inc_factor (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return 256;
	else if(pBD->inc_factor < CHAR_MIN || pBD->inc_factor > CHAR_MAX)
		return 256;
	return (unsigned char) pBD->inc_factor;
}

/*Purpose: Load the content of a file into the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions: fgetc(), feof(), b_addc()
Parameters: pBD: Buffer * const,
			fi: FILE * const
Return value: int, -2, -1, 0
Algorithm: Checks if the parameters are valid and then enter a loop
to read every character in the file and save in the buffer
*/
int b_load (FILE * const fi, Buffer * const pBD){
	char aux;
	if(fi == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	if(pBD == NULL)
		return R_FAIL_1;
	aux = (char) fgetc(fi);
	while(!feof(fi)){ /*Get in a loop until EOF*/
		if(b_addc(pBD, aux) == NULL){
			return LOAD_FAIL;
		}
		aux = (char) fgetc(fi);
	}
	return R_OK_0;
}

/*Purpose: Checks to see if the buffer is empty
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: int, -1, 0, 1
Algorithm:
*/
int b_isempty (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->addc_offset == 0)
		return 1;
	return 0;
}

/*Purpose: Returns the eob of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: int, -1, 0, 1
Algorithm:
*/
int b_eob (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	return pBD->eob;
}

/*Purpose: Return the getc_offset
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: char, 0/+127
Algorithm:
*/
char b_getc (Buffer * const pBD){
	char aux;
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_2;
	else if(pBD->getc_offset == pBD->addc_offset){
		pBD->eob = 1;
		return R_FAIL_1;
	} 
	pBD->eob = 0;
	aux = pBD->ca_head[pBD->getc_offset];
	pBD->getc_offset++;
	return aux; /*Returns the character*/
}

/*Purpose: Print the content in the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions: b_getc()
Parameters: pBD: Buffer * const,
Return value: int, -1, 0
Algorithm:
*/
int b_print (Buffer * const pBD){
	char aux;
	pBD->getc_offset = 0;
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->addc_offset == 0){
		printf("The buffer is empty.\n");
		return R_FAIL_1;
	}
	aux = b_getc(pBD);
	while(pBD->eob != 1){ /*Get in a loop to print every character in the buffer until pBD->eob != 1*/
		printf("%c", aux);
		aux = b_getc(pBD);
	}
	printf("\n");
	pBD->getc_offset = 0;
	pBD->eob = 0;
	return R_OK_0;
}

/*Purpose: Resize the buffer as needed
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions: realloc()
Parameters: pBD: Buffer * const,
Return value: pointer to the buffer
Algorithm:
*/
Buffer *b_pack(Buffer * const pBD){
	char * auxPointer;
	short newCapacity;
	pBD->r_flag = 0;
	if(pBD == NULL || (unsigned)pBD->addc_offset*sizeof(char) == SHRT_MAX) /*Checks for invalid parameters and overflow*/
		return NULL;
	newCapacity = (pBD->addc_offset+1)*sizeof(char);
	auxPointer = (char*) realloc(pBD->ca_head, newCapacity); /*Resize buffer*/
	if(auxPointer == NULL)
		return NULL;
	if(pBD->ca_head != auxPointer){
		pBD->r_flag = 1;
		pBD->ca_head = auxPointer;
	}
	pBD->capacity = newCapacity;
	return pBD;
}


/*Purpose: Return the realloc flag from the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: char, -1, 0, 1
Algorithm:
*/
char b_rflag (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	return pBD->r_flag;
}

/*Purpose: Decrements getc_offset by 1
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: short, 0/+32767, -1
Algorithm:
*/
short b_retract (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	if(pBD->getc_offset != 0)
		pBD->getc_offset--;
	return pBD->getc_offset;
}

/*Purpose: getc_offset receives the value from mark_offset
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: short, 0/+32767, -1
Algorithm:
*/
short b_retract_to_mark (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	pBD->getc_offset = pBD->mark_offset;
	return pBD->getc_offset;
}

/*Purpose: Return getc_offset
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: short, 0/+32767, -1
Algorithm:
*/
short b_getc_offset (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	if(pBD->getc_offset < SHRT_MIN || pBD->getc_offset > SHRT_MAX)
		return R_FAIL_1;
	return pBD->getc_offset;
}

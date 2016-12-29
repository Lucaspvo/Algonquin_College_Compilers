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
Called functions: calloc(), malloc()
Parameters: init_capacity: short, -32768/+32767
			inc_factor: char, 0/+127
			o_mode: char, 0/+127
Return value: pointer to the Buffer strcture
Algorithm: First of all it decides if the parameters are valid, then
it tries to alocate memory for the buffer and then save the data
sent as parameters in the buffer's fields
*/
Buffer * b_create(short init_capacity,char inc_factor,char o_mode){
	if(inc_factor > SHRT_MAX || inc_factor < 0)
		return NULL;
	else if(o_mode > CHAR_MAX || o_mode < 0)
		return NULL;
	else if(init_capacity > SHRT_MAX || init_capacity < 0)
		return NULL;

	pbuffer = (pBuffer) calloc(1, sizeof (Buffer));
	if(pbuffer == NULL) 
		puts("There is not enough memory to do calloc");

	pbuffer->ca_head = (char *)malloc(init_capacity*sizeof(char));
	if(pbuffer->ca_head == NULL) 
		puts("There is not enough memory to do malloc");

	if( o_mode == 'f' || inc_factor == 0){ /*Checks to see if the buffer will be fixed mode*/
		pbuffer->mode = 0;
		pbuffer->inc_factor = 0;
	}else if(o_mode == 'f' && inc_factor != 0){ /*Checks to see if the buffer will be fixed mode*/
		pbuffer->mode = 0;
		pbuffer->inc_factor = 0;
	}else if(o_mode == 'a' && (inc_factor >= MIN_INC_FACTOR_AM && inc_factor <= MAX_INC_FACTOR_A)){ /*Checks to see if the buffer will be additive mode*/
		pbuffer->mode = 1;
		pbuffer->inc_factor = inc_factor;
	}else if(o_mode == 'm' && (inc_factor >= MIN_INC_FACTOR_AM && inc_factor <= MAX_INC_FACTOR_M)){/*Checks to see if the buffer will be multiplicative mode*/
		pbuffer->mode = -1;
		pbuffer->inc_factor = inc_factor;
	}else{ /*returns NULL if non of the options were passed in the parameters*/
		printf("None of the programmed options");
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
	else if(symbol < CHAR_MIN || symbol > CHAR_MAX)
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
			newCapacity = (pBD->inc_factor * sizeof(char)) + pBD->capacity;
			if(newCapacity > 0){
				auxPointer = (char*) realloc(pBD->ca_head, newCapacity);
				if(auxPointer == NULL)
					return NULL;
				else{
					pBD->ca_head = auxPointer;
					pBD->ca_head[pBD->addc_offset] = symbol;
					pBD->addc_offset++;
					pBD->capacity = newCapacity;
					pBD->r_flag = 1;
					return pBD;
				}
			}else
				return NULL;
		}else if(pBD->mode == -1){ /*If it is multiplicative, calculate a new size for the buffer*/
			if(pBD->capacity == SHRT_MAX)
				return NULL;
			else{
				availableSpace = SHRT_MAX - pBD->capacity;
				newIncrement = (availableSpace * pBD->inc_factor) / 100;
				newCapacity = pBD->capacity + newIncrement ;
				if(newCapacity < SHRT_MAX){ /*If newCapacity is less then SHRT_MAX, realloctes with newCapacity*/
					auxPointer = (char*) realloc(pBD->ca_head, newCapacity);
					if(auxPointer == NULL)
						return NULL;
					pBD->ca_head = auxPointer;
					pBD->ca_head[pBD->addc_offset] = symbol;
					pBD->addc_offset++;
					pBD->capacity = newCapacity;
					pBD->r_flag = 1;
					return pBD;
				}else if(newCapacity > SHRT_MAX && pBD->capacity < SHRT_MAX){ /*If newCapacity is more than SHRT_MAX and pBD->capacity is less than SHRT_MAX
																						reallocates the buffer with the max positive value missing to SHRT_MAX*/
					auxPointer = (char*) realloc(pBD->ca_head, (SHRT_MAX - pBD->capacity));
					if(auxPointer == NULL)
						return NULL;
					pBD->ca_head = auxPointer;
					pBD->ca_head[pBD->addc_offset] = symbol;
					pBD->addc_offset++;
					pBD->capacity = newCapacity;
					pBD->r_flag = 1;
					return pBD;
				}
				else
					return NULL;
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
Return value: int, -32768/+32767 
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
Return value: int, -32768/+32767 
Algorithm:
*/
int b_isfull (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->capacity < SHRT_MIN || pBD->capacity > SHRT_MAX)
		return R_FAIL_1;
	else if(pBD->addc_offset < SHRT_MIN || pBD->addc_offset > SHRT_MAX)
		return R_FAIL_1;
	if(pBD->capacity == pBD->addc_offset) /*If non is invalid, returns 1 if the buffer is full*/
		return 1;
	else
		return 0;
}

/*Purpose: Returns the size of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: short, -32768/+32767 
Algorithm:
*/
short b_size (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->capacity < SHRT_MIN || pBD->capacity > SHRT_MAX) 
		return R_FAIL_1;
	else 
		return pBD->addc_offset;
}

/*Purpose: Returns the capacity of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: short, -32768/+32767 
Algorithm:
*/
short b_capacity(Buffer * const pBD){
	if(pBD == NULL)
		return R_FAIL_1;
	else if(pBD->capacity >= SHRT_MIN && pBD->capacity <= SHRT_MAX)
		return pBD->capacity;
	else
		return R_FAIL_1;
}

/*Purpose: Sets the mark_offset flag of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
			mark: short, -32768/+32767
Return value: *char
Algorithm:
*/
char * b_setmark (Buffer * const pBD, short mark){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return NULL;
	else if(mark > SHRT_MAX || mark < 0)
		return NULL;
	else if(mark <= pBD->addc_offset){
		pBD->mark_offset = mark;
		return &pBD->ca_head[mark];
	}
	else
		return NULL;
}

/*Purpose: Returns the mar_offset of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: short, -32768/+32767
Algorithm:
*/
short b_mark (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->mark_offset < SHRT_MIN || pBD->mark_offset > SHRT_MAX)
		return R_FAIL_1;
	else
		return pBD->mark_offset;
}

/*Purpose: Returns the mode of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const
Return value: int, -32768/+32767
Algorithm:
*/
int b_mode (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->eob < INT_MIN || pBD->eob > INT_MAX)
		return R_FAIL_1;
	else
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
	else
		return (unsigned int) pBD->inc_factor;
}

/*Purpose: Load the content of a file into the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions: fgetc(), feof(), b_addc()
Parameters: pBD: Buffer * const,
			fi: FILE * const
Return value: int, -32768/+32767
Algorithm: Checks if the parameters are valid and then enter a loop
to read every character in the file and save in the buffer
*/
int b_load (FILE * const fi, Buffer * const pBD){
	char aux;
	if(fi == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD == NULL)
		return R_FAIL_1;
	else{
		aux = (char) fgetc(fi);
			if(aux != EOF) /*Starts to load file here*/
				if(b_addc(pBD, aux) == NULL)
					return LOAD_FAIL;
		while(!feof(fi)){ /*Get in a loop until EOF*/
			aux = (char) fgetc(fi);
			if(aux != EOF)
				if(b_addc(pBD, aux) == NULL)
					return LOAD_FAIL;
		}
		return R_OK_0;
	}
}

/*Purpose: Checks to see if the buffer is empty
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: int, -32768/+32767
Algorithm:
*/
int b_isempty (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->addc_offset == 0)
		return 1;
	else
		return 0;
}

/*Purpose: Returns the eob of the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: int, -32768/+32767
Algorithm:
*/
int b_eob (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->eob < INT_MIN || pBD->eob > INT_MAX)
		return R_FAIL_1;
	else
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
	}else{ /*Returns the character*/
		pBD->eob = 0;
		aux = pBD->ca_head[pBD->getc_offset];
		pBD->getc_offset++;
		return aux;
	}
}

/*Purpose: Print the content in the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions: b_getc()
Parameters: pBD: Buffer * const,
Return value: int, -32768/+32767
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
	while(pBD->eob != 1){ /*Get in a loop to print every character in the buffer until pBD->eob != 1*/
		aux = b_getc(pBD);
		if(pBD->eob != 1)
			printf("%c", aux);
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
	if(pBD == NULL) /*Checks for invalid parameters*/
		return NULL;
	auxPointer = (char*) realloc(pBD->ca_head, pBD->addc_offset+1); /*Resize buffer*/
	if(auxPointer == NULL)
		return NULL;
	else{
		pBD->ca_head = auxPointer;
		pBD->capacity = pBD->addc_offset+1;
		pBD->r_flag = 1;
		return pBD;
	}
}


/*Purpose: Return the realloc flag from the buffer
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: char, 0/+127
Algorithm:
*/
char b_rflag (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->r_flag < CHAR_MIN || pBD->r_flag > CHAR_MAX)
		return R_FAIL_1;
	else
		return pBD->r_flag;
}

/*Purpose: Decrements getc_offset by 1
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: short, -32768/+32767
Algorithm:
*/
short b_retract (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->getc_offset < SHRT_MIN || pBD->getc_offset > SHRT_MAX)
		return R_FAIL_1;
	else{
		pBD->getc_offset--;
		return pBD->getc_offset;
	}
}

/*Purpose: getc_offset receives the value from mark_offset
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: short, -32768/+32767
Algorithm:
*/
short b_retract_to_mark (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	else if(pBD->mark_offset < SHRT_MIN || pBD->mark_offset > SHRT_MAX)
		return R_FAIL_1;
	else{
		pBD->getc_offset = pBD->mark_offset;
		return pBD->getc_offset;
	}
}

/*Purpose: Return getc_offset
Author: Lucas Pazelo Vargas de Oliveira
History/Versions: 1.0 January 27
Called functions:
Parameters: pBD: Buffer * const,
Return value: short, -32768/+32767
Algorithm:
*/
short b_getc_offset (Buffer * const pBD){
	if(pBD == NULL) /*Checks for invalid parameters*/
		return R_FAIL_1;
	if(pBD->getc_offset < SHRT_MIN || pBD->getc_offset > SHRT_MAX)
		return R_FAIL_1;
	else
		return pBD->getc_offset;
}
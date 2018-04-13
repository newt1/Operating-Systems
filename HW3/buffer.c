/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "semaphore.h"
#include "buffer.h"

/*
decrements pFullBuffer, will switch threads if the semaphore value is decremented on 0
char c is copied from buffer and returned
parameter pBuffer is a pointer to a Buffer struct
full + empty = buffer_size will always hold
*/
char remoove(Buffer* pBuffer){
	down(pBuffer->pFullBuffer);
	assert(pBuffer->pFullBuffer->a >= 0);

	char c = pBuffer->pCharArray[pBuffer->nextOut];
	pBuffer->nextOut = (pBuffer->nextOut + 1) % pBuffer->size;
	
	assert(pBuffer->pEmptyBuffer->a <= pBuffer->size);
	up(pBuffer->pEmptyBuffer);
	return c;
}

/*
decrements pEmptyBuffer, will switch threads if the semaphore value is decremented on 0
parameter c is copied and deposited on the buffer
parameter pBuffer is a pointer to a Buffer struct
full + empty = buffer_size will always hold
*/
void deposit(Buffer* pBuffer, char c){
	down(pBuffer->pEmptyBuffer);
	assert(pBuffer->pEmptyBuffer->a >= 0);

	pBuffer->pCharArray[pBuffer->nextIn] = c;
	pBuffer->nextIn = (pBuffer->nextIn + 1) % pBuffer->size;

	assert(pBuffer->pFullBuffer->a <= pBuffer->size);
	up(pBuffer->pFullBuffer);
}


/*
parameter size is the size of the buffer
parameter pBuffer is a pointer to a Buffer struct to initialize
the char array and semaphores pointers are allocated memory on the heap
createSem is implemented in semaphore.o
*/
void createBuffer(Buffer* pBuffer, int size){	
	pBuffer->size = size;
	pBuffer->nextIn = 0;
	pBuffer->nextOut = 0;

	pBuffer->pCharArray = malloc(sizeof(char) * size);
	if(!pBuffer->pCharArray){
        perror("malloc returned null to (*pBuffer).pCharArray");
        exit(EXIT_FAILURE);
    }    
	pBuffer->pEmptyBuffer = malloc(sizeof(semaphore));
	if(!pBuffer->pEmptyBuffer){
        perror("malloc returned null to (*pBuffer).pEmptyBuffer");
        exit(EXIT_FAILURE);
    }
	pBuffer->pFullBuffer = malloc(sizeof(semaphore));
	if(!pBuffer->pFullBuffer){
        perror("malloc returned null to (*pBuffer).pFullBuffer)");
        exit(EXIT_FAILURE);
    }
	createSem(pBuffer->pEmptyBuffer,size);
	createSem(pBuffer->pFullBuffer,0);
}

/*
frees memory
*/
void deleteBuffer(Buffer* pBuffer){	
	free(pBuffer->pCharArray);	
	free(pBuffer->pEmptyBuffer);	
	free(pBuffer->pFullBuffer);
}

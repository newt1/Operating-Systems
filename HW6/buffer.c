/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>   
#include <semaphore.h> //sem_init, sem_destroy
#include <sys/mman.h>  // mmap, munmap

#include "map.h"
#include "buffer.h"

/*
decrements pFullBuffer, will switch processes if the semaphore value is decremented on 0
char c is copied from buffer and returned
parameter pBuffer is a pointer to a Buffer struct
full + empty = buffer_size will always hold true
*/
char remoove(Buffer* pBuffer){
	sem_wait(pBuffer->pFullBuffer);
	int semValue;
	sem_getvalue(pBuffer->pFullBuffer, &semValue);	
	assert(semValue >= 0);

	char c = pBuffer->pCharArray[pBuffer->nextOut];
	pBuffer->nextOut = (pBuffer->nextOut + 1) % pBuffer->size;
	
	sem_getvalue(pBuffer->pEmptyBuffer, &semValue);	
	assert(semValue <= pBuffer->size);
	sem_post(pBuffer->pEmptyBuffer);
	return c;
}

/*
decrements pEmptyBuffer, will switch processes if the semaphore value is decremented on 0
parameter c is copied and deposited on the buffer
parameter pBuffer is a pointer to a Buffer struct
full + empty = buffer_size will always hold true
*/
void deposit(Buffer* pBuffer, char c){
	sem_wait(pBuffer->pEmptyBuffer);
	int semValue;
	sem_getvalue(pBuffer->pEmptyBuffer, &semValue);
	assert(semValue >= 0);

	pBuffer->pCharArray[pBuffer->nextIn] = c;
	pBuffer->nextIn = (pBuffer->nextIn + 1) % pBuffer->size;
	
	sem_getvalue(pBuffer->pFullBuffer, &semValue);
	assert(semValue <= pBuffer->size);
	sem_post(pBuffer->pFullBuffer);
}


/*
parameter size is the size of the buffer
the char array and semaphores pointers are allocated memory in a mapped memory file
sem_init is implemented is from Posix semaphores
returns pBuffer,  a pointer to a Buffer struct to initialize
*/
void createBuffer(Buffer* pBuffer, int size){	

	pBuffer->size = size;
	pBuffer->nextIn = 0;
	pBuffer->nextOut = 0;

	(pBuffer->pCharArray) = (char*)createMMAP(size * sizeof(char));

	(pBuffer->pEmptyBuffer) = (sem_t*)createMMAP(sizeof(sem_t));
	(pBuffer->pFullBuffer) = (sem_t*)createMMAP(sizeof(sem_t));

	sem_init((pBuffer->pEmptyBuffer), 1, size);
	sem_init((pBuffer->pFullBuffer), 1, 0);
}

/*
destroys the semeaphores and delete the mapped memory files
*/
void deleteBuffer(Buffer* pBuffer){	
	sem_destroy(pBuffer->pEmptyBuffer);
	sem_destroy(pBuffer->pFullBuffer);

	deleteMMAP(pBuffer->pCharArray, ((pBuffer->size) * sizeof(char)) );
	deleteMMAP(pBuffer->pEmptyBuffer, sizeof(sem_t));
	deleteMMAP(pBuffer->pFullBuffer, sizeof(sem_t));
	deleteMMAP(pBuffer, sizeof(Buffer));
}


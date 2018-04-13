/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#ifndef BUFFER_H
#define BUFFER_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>  // mmap, munmap
#include <unistd.h>    
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>   
#include <semaphore.h> //sem_init, sem_destroy

typedef struct{
	int size;
	int nextIn;
	int nextOut;
	char* pCharArray;
	sem_t* pEmptyBuffer;
	sem_t* pFullBuffer;
} Buffer;

char remoove(Buffer* pBuffer);
void deposit(Buffer* pBuffer, char c);
void createBuffer(Buffer* pBuffer, int size);
void deleteBuffer(Buffer* pBuffer);

#endif
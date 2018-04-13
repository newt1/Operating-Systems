/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#ifndef BUFFER_H
#define BUFFER_H

#include "semaphore.h"

typedef struct{
	int size;
	int nextIn;
	int nextOut;
	char* pCharArray;
	semaphore* pEmptyBuffer;
	semaphore* pFullBuffer;
} Buffer;

char remoove(Buffer* pBuffer);
void deposit(Buffer* pBuffer, char c);
void createBuffer(Buffer* pBuffer, int size);
void deleteBuffer(Buffer* pBuffer);

#endif
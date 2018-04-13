/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#ifndef MAP_H
#define MAP_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>  // mmap, munmap
#include <unistd.h>    // sleep
#include <sys/types.h>
#include <sys/wait.h>// kill
#include <signal.h>    // kill
#include <semaphore.h> //sem_init, sem_destroy

//borrowed funtions from the example
void* createMMAP(size_t size);  
void deleteMMAP(void* addr, size_t size);

#endif
/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // sleep
#include <sys/types.h>
#include <sys/wait.h> // kill
#include <signal.h>    // kill
#include <semaphore.h> //sem_init, sem_destroy
#include <sys/mman.h>  // mmap, munmap

#include "map.h"
#include "buffer.h"

#define OUTPUT_LINE_SIZE 80
#define BUFFER_SIZE 256

#define ERROR -1

//pointer to Right Buffer only
typedef struct { 
    Buffer* pRightBuffer;  
} RightBuffer;

//pointers to Left and Right Buffers
typedef struct {
    Buffer* pLeftBuffer;  
    Buffer* pRightBuffer;  
} LeftAndRightBuffers;

//pointer to Left Buffer only
typedef struct {
    Buffer* pLeftBuffer;  
} LeftBuffer;

// Prototypes for functions for execution of each thread.
void produceInput(void* state);             //Thread 1
void convertReturnsToBlanks(void* state);   //Thread 2
void squashAstriskPairs(void* state);       //Thread 3
void formatOutput(void* state);             //Thread 4


//borrowed funtions from the example
void waitForChildren(pid_t* childpids, int size);
pid_t forkChild(void (*function)(void *), void* state);

/*This program re-implements HW3 with processes and mapped memory files

The following functions were borrowed from the mmap-example
    -waitForChildren
    -forkChild
    -createMMAP
    -deleteMMAP

How I am using memory-mapped files
    -The createMMAP function creates a mapped memory file for each of the 3 buffers.
    - Since this file is mapped to memory rather than being allocated in the heap the processes can share the memory
    - If instead malloc was used each process would have duplicated memory instead of shared memory.
    -Each Buffer File also has a pointer to a char array and the full and empty semaphores. 
    -Each of the char array, full, and empty semaphores are stored in their own files rather than being allocated to ensure 
        memory is shared.

Sources considered
    -https://linux.die.net/man/7/sem_overview
    -http://man7.org/linux/man-pages/man2/mmap.2.html



    The createMMAP and deleteMMAP functions are stored in their own file map.c with header map.h

    Commands
    cp ~calvinn/comp530/submissions/HW6/{HW6.c,map.c,map.h,buffer.c,buffer.h} .
    gcc -pthread buffer.c HW6.c map.c


*/
int main(int argc, char** argv) {
    pid_t childpids[4];

    Buffer* pUnformattedInputFile = (Buffer*)createMMAP(sizeof(Buffer));
    Buffer* pReturnsToBlanksFile = (Buffer*)createMMAP(sizeof(Buffer));
    Buffer* pDoubleAstrisksToCaretsFile = (Buffer*)createMMAP(sizeof(Buffer));

    createBuffer(pUnformattedInputFile, BUFFER_SIZE);
    createBuffer(pReturnsToBlanksFile, BUFFER_SIZE);
    createBuffer(pDoubleAstrisksToCaretsFile, BUFFER_SIZE);


    RightBuffer produceInputInit = {
        pUnformattedInputFile
    };

    LeftAndRightBuffers convertReturnsToBlanksInit = {
        pUnformattedInputFile, 
        pReturnsToBlanksFile
    };

    LeftAndRightBuffers squashAstriskPairsInit = {
        pReturnsToBlanksFile, 
        pDoubleAstrisksToCaretsFile
    };

    LeftBuffer formatOutputInit = {
        pDoubleAstrisksToCaretsFile
    };

    childpids[0] = forkChild(produceInput, &produceInputInit);
    childpids[1] = forkChild(convertReturnsToBlanks, &convertReturnsToBlanksInit);
    childpids[2] = forkChild(squashAstriskPairs, &squashAstriskPairsInit);
    childpids[3] = forkChild(formatOutput, &formatOutputInit);

    waitForChildren(childpids, 4);

    deleteBuffer(pUnformattedInputFile);
    deleteBuffer(pReturnsToBlanksFile);
    deleteBuffer(pDoubleAstrisksToCaretsFile);

    return 0;
}

//borrowed from example
//changed pointers to void*
pid_t forkChild(void (*function)(void *), void* state){
    //This function takes a pointer to a function as an argument
    //and the functions argument. It then returns the forked child's pid.

   pid_t childpid;
       switch (childpid = fork()) {
               case ERROR:
                       perror("fork error");
                       exit(EXIT_FAILURE);
               case 0:	
                       (*function)(state);
               default:
                      return childpid;
       }
}


//borrowed from example
//added size parameter to delete all children
void waitForChildren(pid_t* childpids, int size){
   int status;
   int i;
   while(ERROR < wait(&status)){ //Here the parent waits on any child.
       if(!WIFEXITED(status)){ //If the termination err, kill all children.
            for(i = 0; i < size; i++){
                kill(childpids[i], SIGKILL);
            }
           break;
        }
   }
}

// The execution path for thread produceInput
void produceInput(void *state) {
    RightBuffer* pInit = state;

    char current;   

    /*loops until EOF is reached 
    (Ctrl D is converted to -1 and stored in EOF)
    deposit into right buffer
    */
    while((current = getchar()) != EOF){       //read character
        deposit(pInit->pRightBuffer, current);  

    }
    
    //pass on EOF
    deposit(pInit->pRightBuffer, EOF);


   exit(EXIT_SUCCESS);
}

// The execution path for thread convertReturnsToBlanks
void convertReturnsToBlanks(void *state) {
    LeftAndRightBuffers* pInit = state;
    
    char current;

    /* Loop until EOF
    get chars from left buffer
    convert '\n' and '\r' to ' '
    deposit into right buffer
    */ 
    while((current = remoove(pInit->pLeftBuffer)) != EOF){      //read character        
        if(current == '\n'){
            current = ' ';
        }
        deposit(pInit->pRightBuffer, current);
    }
    
    //pass on EOF
    deposit(pInit->pRightBuffer, EOF);

    

   exit(EXIT_SUCCESS);
}

// The execution path for thread squashAstriskPairs
void squashAstriskPairs(void *state) {
    LeftAndRightBuffers* pInit = state;

    char current, next;

    /*loop until EOF
    reads from left buffer twice if first is a star or once if not
    writes to right buffer
    */
    while((current = remoove(pInit->pLeftBuffer)) != EOF){ 
        if(current == '*'){   
            next = remoove(pInit->pLeftBuffer); 
            if(next == '*'){     
                deposit(pInit->pRightBuffer, '^');   
            }
            else if(next == EOF){
                deposit(pInit->pRightBuffer, current);   
                break;
            }
            else{                
                deposit(pInit->pRightBuffer, current);   
                deposit(pInit->pRightBuffer, next);
            }
        }
        else{
            deposit(pInit->pRightBuffer, current);   
        }
    }
    
    //pass on EOF
    deposit(pInit->pRightBuffer, EOF);  


    exit(EXIT_SUCCESS);
}

// The execution path for thread formatOutput
void formatOutput(void *state) {
    LeftBuffer* pInit = state;

    int index = 0;

    char outputLine[OUTPUT_LINE_SIZE+1];  //size 80, +1 for null zero
    outputLine[OUTPUT_LINE_SIZE] = '\0';    //81st character is '\0'

    /*  Reads left buffer until EOF
    if index = 79 then print out an 80 character line
    */
    while((outputLine[index] = remoove(pInit->pLeftBuffer)) != EOF){     
        if(index == (OUTPUT_LINE_SIZE-1)){            
            printf("%s\n",outputLine);
            index = -1;
        }
        index++;
    }


    exit(EXIT_SUCCESS);
}

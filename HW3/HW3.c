/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "st.h"
#include "buffer.h"
#include "semaphore.h"

#define OUTPUT_LINE_SIZE 80
#define BUFFER_SIZE 256

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
void* produceInput(void* state);             //Thread 1
void* convertReturnsToBlanks(void* state);   //Thread 2
void* squashAstriskPairs(void* state);       //Thread 3
void* formatOutput(void* state);             //Thread 4

/*This program re-implements HW1 with threads, Producers, and Consumers
The st library is used to implement threading
Thank you to previous TA Willem Neal Wyndham for video tutorial and 
samples for threading and memory allocation

*/
int main(int argc, char** argv) {

    // Initialize the libST runtime.
    st_init();

    Buffer* pUnformattedInput = malloc(sizeof(Buffer));
    if(!pUnformattedInput){
        perror("malloc returned null to pUnformattedInput");
        return(EXIT_FAILURE);
    }

    Buffer* pReturnsToBlanks = malloc(sizeof(Buffer));
    if(!pReturnsToBlanks){
        perror("malloc returned null to pReturnsToBlanks");
        return(EXIT_FAILURE);
    }
    Buffer* pDoubleAstrisksToCarets = malloc(sizeof(Buffer));
    if(!pDoubleAstrisksToCarets){
        perror("malloc returned null to pDoubleAstrisksToCarets");
        return(EXIT_FAILURE);
    }


    createBuffer(pUnformattedInput, BUFFER_SIZE);
    createBuffer(pReturnsToBlanks, BUFFER_SIZE);
    createBuffer(pDoubleAstrisksToCarets, BUFFER_SIZE);

    RightBuffer produceInputInit = {
        pUnformattedInput
    };

    LeftAndRightBuffers convertReturnsToBlanksInit = {
        pUnformattedInput, 
        pReturnsToBlanks
    };

    LeftAndRightBuffers squashAstriskPairsInit = {
        pReturnsToBlanks, 
        pDoubleAstrisksToCarets
    };

    LeftBuffer formatOutputInit = {
        pDoubleAstrisksToCarets
    };

    // Create thread produceInput
    if (st_thread_create(produceInput, &produceInputInit, 0, 0) == NULL) {
        perror("st_thread_create failed for thread produceInput");
        exit(EXIT_FAILURE);
    }

    // Create thread convertReturnsToBlanks
    if (st_thread_create(convertReturnsToBlanks, &convertReturnsToBlanksInit, 0, 0) == NULL) {
        perror("st_thread_create failed for thread convertReturnsToBlanks");
        exit(EXIT_FAILURE);
    }

    // Create thread squashAstriskPairs
    if (st_thread_create(squashAstriskPairs, &squashAstriskPairsInit, 0, 0) == NULL) {
        perror("st_thread_create failed for thread squashAstriskPairs");
        exit(EXIT_FAILURE);
    }

    // Create thread formatOutput
    if (st_thread_create(formatOutput, &formatOutputInit, 0, 0) == NULL) {
        perror("st_thread_create failed for thread formatOutput");
        exit(EXIT_FAILURE);
    }


    //empty out buffer
    //free((unformatted_input.buffer_chars));
    //free((returns_converted_to_blanks.buffer_chars));
    //free((astrisk_pairs_squashed.buffer_chars));

    // Exit from main via ST.
    st_thread_exit(NULL);
    return 0;
}

// The execution path for thread produceInput
void *produceInput(void *state) {
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



    // Exit from main via ST
    st_thread_exit(NULL);
}

// The execution path for thread convertReturnsToBlanks
void *convertReturnsToBlanks(void *state) {
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

    //left buffer memory can be freed
    deleteBuffer(pInit->pLeftBuffer);
    free(pInit->pLeftBuffer);

    // Exit from main via ST.
    st_thread_exit(NULL);
}

// The execution path for thread squashAstriskPairs
void *squashAstriskPairs(void *state) {
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

    //left buffer memory can be freed
    deleteBuffer(pInit->pLeftBuffer);
    free(pInit->pLeftBuffer);

    // Exit from main via ST.
    st_thread_exit(NULL);
}

// The execution path for thread formatOutput
void *formatOutput(void *state) {
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

    //left buffer memory can be freed
    deleteBuffer(pInit->pLeftBuffer);
    free(pInit->pLeftBuffer);

    // Exit from main via ST.
    st_thread_exit(NULL);
}

/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define OUTPUT_LINE_SIZE 80
#define READ 0
#define WRITE 1

// Prototypes for functions for each process.
void produceInput(int writeDescriptor);                                 
void convertReturnsToBlanks(int readDescriptor, int writeDescriptor);   
void squashAstriskPairs(int readDescriptor, int writeDescriptor);       
void formatOutput(int readDescriptor);                                  

/*This program re-implements HW1 with processes and pipes
The first process reads from stdin and writes to the first pipe
The second process reads from the pipe changes '\n' to ' ' and writes to the second pipe
The third process changes '**' to '^' and writes to the final pipe
The final process reads from the final pipe and writes 8- character lines

In this implementation the parent forks 4 processes and waits for all 4 to complete
Each process will loop until EOF is reached. The process will return and then exit.
Once each process has returned the parent can kill off the children and then exit
*/
int main(int argc, char** argv) {

    pid_t produceInputPID;
    pid_t convertReturnsToBlanksPID;
    pid_t squashAstriskPairsPID;
    pid_t formatOutputPID;

    int initialPipe[2];
    int returnsToBlanks[2];
    int doubleAstrisksToCarets[2];


    if(pipe(initialPipe) == -1){
        perror("Error in creating pipe: initialPipe");
        exit(EXIT_FAILURE);
    }
    if(pipe(returnsToBlanks) == -1){
        perror("Error in creating pipe: returnsToBlanks");
        exit(EXIT_FAILURE);
    }
    if(pipe(doubleAstrisksToCarets) == -1){
        perror("Error in creating pipe: doubleAstrisksToCarets");
        exit(EXIT_FAILURE);
    }



    switch(produceInputPID = fork()){
        case -1:
        perror("Error in forking doubleAstrisksToCarets");
        exit(EXIT_FAILURE);

        //child process
        case 0:
        close(initialPipe[READ]);
        close(returnsToBlanks[READ]);
        close(returnsToBlanks[WRITE]);
        close(doubleAstrisksToCarets[READ]);
        close(doubleAstrisksToCarets[WRITE]);
        produceInput(initialPipe[WRITE]);
        exit(EXIT_SUCCESS); //ensures child process does not fork another child
    }


    switch(convertReturnsToBlanksPID = fork()){
        case -1:
        perror("Error in forking convertReturnsToBlanks");
        exit(EXIT_FAILURE);

        //child process
        case 0:
        close(initialPipe[WRITE]);
        close(returnsToBlanks[READ]);
        close(doubleAstrisksToCarets[READ]);
        close(doubleAstrisksToCarets[WRITE]);
        convertReturnsToBlanks(initialPipe[READ],returnsToBlanks[WRITE]);
        exit(EXIT_SUCCESS); //ensures child process does not fork another child
    }

    switch(squashAstriskPairsPID = fork()){
        case -1:
        perror("Error in forking squashAstriskPairs");
        exit(EXIT_FAILURE);

        //child process
        case 0:
        close(initialPipe[READ]);
        close(initialPipe[WRITE]);
        close(returnsToBlanks[WRITE]);
        close(doubleAstrisksToCarets[READ]);
        squashAstriskPairs(returnsToBlanks[READ], doubleAstrisksToCarets[WRITE]);
        exit(EXIT_SUCCESS); //ensures child process does not fork another child
    }


    switch(formatOutputPID = fork()){
        case -1:
        perror("Error in forking formatOutput");
        exit(EXIT_FAILURE);

        //child process
        case 0:
        close(initialPipe[READ]);
        close(initialPipe[WRITE]);
        close(returnsToBlanks[READ]);
        close(returnsToBlanks[WRITE]);
        close(doubleAstrisksToCarets[WRITE]);
        formatOutput(doubleAstrisksToCarets[READ]);
        exit(EXIT_SUCCESS); 
    }

        close(initialPipe[READ]);
        close(initialPipe[WRITE]);
        close(returnsToBlanks[READ]);
        close(returnsToBlanks[WRITE]);
        close(doubleAstrisksToCarets[READ]);
        close(doubleAstrisksToCarets[WRITE]);

        /*wait(NULL) returns the PID of the killed process
        wait(NULL) returns -1 if no process is killed
        */
      while (wait(NULL) > 0)
      {
        
      }

    return 0;
}

/*
gets character from stdin and writes them to the first pipe

parameter writeDescriptor is the pipe to write to
*/
void produceInput(int writeDescriptor) {
    char current;   

    //will write EOF to pipe before exiting loop    
    do {       
        current = getchar();
        write(writeDescriptor, &current, sizeof(char));  
        
    } while(current != EOF);
    
    close(writeDescriptor);
}

/*
reads characters from the first pipe, changes '\n' to ' ' 
and writes to the second pipe

parameter readDescriptor is the pipe to read from
parameter writeDescriptor is the pipe to write to
*/
void convertReturnsToBlanks(int readDescriptor, int writeDescriptor) {
    char current;

    /* Loop until EOF
    get chars from left buffer
    convert '\n' to ' '
    deposit into right buffer
    */ 
    do {                   
        read(readDescriptor, &current, sizeof(char)); 
        if(current == '\n'){
            current = ' ';
        }
        write(writeDescriptor, &current, sizeof(char)); 
    } while(current != EOF);
    
    close(readDescriptor);
    close(writeDescriptor);
}

/*
reads characters from the second pipe, changes '**' to '^' 
and writes to the third pipe

parameter readDescriptor is the pipe to read from
parameter writeDescriptor is the pipe to write to
*/
void squashAstriskPairs(int readDescriptor, int writeDescriptor) {
    char current, next;
    const char caret = '^'; 

    /*loop until EOF reached
    reads from left buffer twice if first is a star otherwise reads once
    writes to right buffer
    */
    do { 
        read(readDescriptor, &current, sizeof(char));
        if(current == '*'){   
            read(readDescriptor, &next, sizeof(char)); 
            if(next == '*'){     
                write(writeDescriptor, &caret, sizeof(char));   
            }
            else{                
                write(writeDescriptor, &current, sizeof(char)); 
                write(writeDescriptor, &next, sizeof(char)); 
                current = next; //needed in case next is EOF
            }
        }
        else{
             write(writeDescriptor, &current, sizeof(char));
        }
    } while(current != EOF);

    close(readDescriptor);
    close(writeDescriptor);    
}

/*
reads characters from the final and third pipe to a string of length OUTPUT_LINE_SIZE
and prints out the line when it is full

parameter readDescriptor is the pipe to read from
*/
void formatOutput(int readDescriptor) {
    int index = 0;
    char current;

    char outputLine[OUTPUT_LINE_SIZE+1];  //size 80, +1 for null zero
    outputLine[OUTPUT_LINE_SIZE] = '\0';    //81st character is '\0'

    /*  Reads left buffer until EOF
    if index = 79 then print out an 80 character line
    */
    read(readDescriptor, &current, sizeof(char));
    while(current != EOF){   

        outputLine[index] = current;

        //print line
        if(index == (OUTPUT_LINE_SIZE-1)){            
            printf("%s\n",outputLine);
            index = -1;
        }
  
        read(readDescriptor, &current, sizeof(char));
        index++;
    }

    close(readDescriptor);
}

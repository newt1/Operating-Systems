/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* process 1 produceInput
gets character from stdin and writes them to the first pipe

Use STDIN_FILENO = 0 to read from stdin
Use STDOUT_FILENO = 1 to write to stdout
*/
int main(int argc, char** argv) {
    char current;   

    //will write EOF to pipe before exiting loop    
    do {       
        current = getchar();
        write(STDOUT_FILENO, &current, sizeof(char));  
        
    } while(current != EOF);
    
    close(STDOUT_FILENO);
}
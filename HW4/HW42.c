/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* process 2 convertReturnsToBlanks
reads characters from the first pipe, changes '\n' to ' ' 
and writes to the second pipe

Use STDIN_FILENO = 0 to read from stdin
Use STDOUT_FILENO = 1 to write to stdout
*/
int main(int argc, char** argv) {

    char current;

    /* Loop until EOF
    get chars from left buffer
    convert '\n' to ' '
    deposit into right buffer
    */ 
    do {                   
        read(STDIN_FILENO, &current, sizeof(char)); 
        if(current == '\n'){
            current = ' ';
        }
        write(STDOUT_FILENO, &current, sizeof(char)); 
    } while(current != EOF);
    
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
}
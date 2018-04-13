/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* process 3 squashAstriskPairs
reads characters from the second pipe, changes '**' to '^' 
and writes to the third pipe

Use STDIN_FILENO = 0 to read from stdin
Use STDOUT_FILENO = 1 to write to stdout
*/
int main(int argc, char** argv) {
    char current, next;
    const char caret = '^'; 

    /*loop until EOF reached
    reads from left buffer twice if first is a star otherwise reads once
    writes to right buffer
    */
    do { 
        read(STDIN_FILENO, &current, sizeof(char));
        if(current == '*'){   
            read(STDIN_FILENO, &next, sizeof(char)); 
            if(next == '*'){     
                write(STDOUT_FILENO, &caret, sizeof(char));   
            }
            else{                
                write(STDOUT_FILENO, &current, sizeof(char)); 
                write(STDOUT_FILENO, &next, sizeof(char)); 
                current = next; //needed in case next is EOF
            }
        }
        else{
             write(STDOUT_FILENO, &current, sizeof(char));
        }
    } while(current != EOF);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);    
}
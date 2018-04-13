/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define OUTPUT_LINE_SIZE 80

/* process 4 formatOutput
reads characters from the final and third pipe to a string of length OUTPUT_LINE_SIZE
and prints out the line when it is full

Use STDIN_FILENO = 0 to read from stdin
Use STDOUT_FILENO = 1 to write to stdout
*/
int main(int argc, char** argv) {
    int index = 0;
    char current;

    char outputLine[OUTPUT_LINE_SIZE+1];  //size 80, +1 for null zero
    outputLine[OUTPUT_LINE_SIZE] = '\0';    //81st character is '\0'

    /*  Reads left buffer until EOF
    if index = 79 then print out an 80 character line
    */
    read(STDIN_FILENO, &current, sizeof(char));
    while(current != EOF){   

        outputLine[index] = current;

        //print line
        if(index == (OUTPUT_LINE_SIZE-1)){            
            printf("%s\n",outputLine);
            index = -1;
        }
  
        read(STDIN_FILENO, &current, sizeof(char));
        index++;
    }

    close(STDIN_FILENO);
}

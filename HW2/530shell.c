/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work

Program Description: Program will read in input from user. 
Program is forked to allow child to processes input while parent waits. 
Input is parsed into strings seperated by spaces or tabs. 
The first string and set of arguments are passed to execvp.
execvp executes linux command files with the user input as the passed in arguments.
Parent waits for child to exit.
pParent loops and gets new input if end of file was not read.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//prototype functions 
char* promptInput();
char** parseLine(char*);

int main(void){

	int childPID;		
	char* inputLine;	//string of input
	char** arguments;	//parsed input	
	int status;			//parameter modified by wait(&status) passed to WIFEXITED
	int execvpCode;	//store return value of execvp()

	//end program if end of file reached
	while(feof(stdin) == 0){	
		
		inputLine = promptInput();	//promptInput() should be executed in parent process to check for End Of File flag
	
		//exit if end of file reached
		if(feof(stdin)){
			exit(EXIT_SUCCESS);
		}

		childPID = fork();

		
		//check for fork error
		if(childPID < 0){
			perror("fork error");
			exit(EXIT_FAILURE);
		}

		//childPID = 0 for child process
		if(childPID == 0){				
			arguments = parseLine(inputLine);	//split line into argument strings
			free(inputLine);

			//note: arguments and inputLine will not need to be freed in child process since execvp will wipe everything
			execvpCode = execvp(arguments[0], &arguments[0]);
			if(execvpCode == -1){
				perror("execvp error");	
				exit(EXIT_FAILURE);
			}				
		}
		else{			
			free(inputLine);	//inputLine needs to be freed
			do{
				wait(&status);

				//WIFEXITED(status) returns true if child terminated normally, otherwise returns 0, from wait man page
			} while(WIFEXITED(status) == 0);		
		}	
	}
	return (0); 
}


//prompts '% '
//returns user input until \n or end of file
char * promptInput(){	
	//will be doubled if limit is reached
	int maxInputSize = 100;

	char* inputLine =  (char*) malloc(sizeof(char) * maxInputSize);
	
	//print error if inputLine is set to 0
	if(inputLine == NULL){
		perror("memory allocation error in inputLine");
	}


	int i = 0;
	int endFlag = 0;
	
	
	//output "% "
	printf("%% ");

	//get file name
	do{
		//inputLine[i] = getchar();  also works
		*(inputLine + i) = getchar();
		

		//set end flag and replace \n with \0
		if(*(inputLine + i) == '\n'|| feof(stdin))
		{
			*(inputLine + i) = '\0';
			endFlag = 1;
		}
		

		//double memory allocation if memory is full
		if(i >= (maxInputSize-1) ){
			maxInputSize *= 2;
			inputLine = (char*) realloc(inputLine, sizeof(char) * maxInputSize);

			//print error if inputLine is set to 0
			if(inputLine == NULL){
				perror("memory reallocation error in inputLine");
			}
		}

		i++;
	} while(endFlag == 0);

		return inputLine;
}


//parameter-line: input line to be parsed
//return: pointer to character arrays, last array is null
//input line is split into strings by '\t' or ' '
char** parseLine(char* line)
{
	int endFlag = 0;

	//can be doubled if neccesary
	int maxNumStrings = 100;
	int initialMaxLength = 100;

	//hold the maximum length of each string
	int* maxStringLength = (int*) malloc(sizeof(int) * maxNumStrings);

	//initialize first 
	maxStringLength[0] = initialMaxLength;
	
	//return strings allocated 100 pointers
	char** returnStrings  = (char**) malloc(sizeof(char*) * maxNumStrings);

	//print error if returnStrings is set to 0
	if(returnStrings == NULL){
		perror("memory allocation error in returnStrings");
	}

	//first pointer allocated 100 characters
	returnStrings[0]  = (char*) malloc(sizeof(char) * maxStringLength[0]);

	//print error if returnStrings is set to 0
	if(returnStrings[0] == NULL){
		perror("memory allocation error in returnStrings[0]");
	}

	//x indexes characters in a string
	int x = 0;

	//y indexes strings in the return string list
	int y = 0;

	//i is input line index
	int i = 0;
	
	do{
		if(line[i] == '\0'){
			returnStrings[y][x] = '\0';
			endFlag = 1;
			y++;

			
			//reallocate number of return strings and the array that holds the max string length for each string
			if(y >= maxNumStrings){
				maxNumStrings *= 2;
				returnStrings = (char**) realloc(returnStrings, sizeof(char*) * maxNumStrings);

				//print error if returnStrings is set to 0
				if(returnStrings == NULL){
					perror("memory reallocation error in returnStrings");
				}

				maxStringLength = (int*) realloc(maxStringLength, sizeof(int) * maxNumStrings);

				//print error if returnStrings is set to 0
				if(returnStrings == NULL){
					perror("memory reallocation error in maxStringLength");
				}
			}
				
			returnStrings[y] = NULL;
		}
		else if(line[i] == ' ' || line[i] == '\t'){	
			
			returnStrings[y][x] = '\0';
			y++;
			x = 0;	

			maxStringLength[y] = initialMaxLength;
			returnStrings[y]  = (char*) malloc(sizeof(char) * initialMaxLength);
			
			//reallocate number of return strings and the array that holds the max string length for each string
			if(y >= maxNumStrings){
				maxNumStrings *= 2;
				returnStrings = (char**) realloc(returnStrings, sizeof(char*) * maxNumStrings);

				//print error if returnStrings is set to 0
				if(returnStrings == NULL){
					perror("memory reallocation error in returnStrings");
				}

				maxStringLength = (int*) realloc(maxStringLength, sizeof(int) * maxNumStrings);

				//print error if returnStrings is set to 0
				if(returnStrings == NULL){
					perror("memory reallocation error in maxStringLength");
				}
			}

		}
		else{
			returnStrings[y][x] = line[i];
			x++;

				if(x >= maxStringLength[y]){
					maxStringLength[y] *= 2;
					returnStrings[y] = (char*) realloc(returnStrings[y], sizeof(char) * maxStringLength[y]);

					//print error if returnStrings is set to 0
					if(returnStrings[y] == NULL){
						perror("memory reallocation error in returnStrings[y]");
					}
				}
		}

		i++;
	} while(endFlag == 0);


	return returnStrings;
}

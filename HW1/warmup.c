/*
Calvin Newton

UNC Honor Pledge: I certify that no unauthorized assistance has been received or given
in the completion of this work
*/

#include <stdio.h>
#include <string.h>

#define LINESIZE 80 /* The maximum number of characters in a line of input */

int main(void)
{
  char current;		//current character
  char nextChar;	//1 character look ahead for *
  char firstChar;	//store first character of next line of 80 characters
  int firstCharStored = 0;	//boolean flag set if the first character of the next line needs to be changed

  char outputLine[LINESIZE+1];	//size 80, +1 for null zero
  int i = 0;	//initialize

  outputLine[LINESIZE] = '\0';	//used to print only 80 character lines and no more
  
	  do {

		  for(i = 0; i<LINESIZE; i++)	//repeat 80 times
		  {

			  if(firstCharStored)
			  {
				  outputLine[0] = firstChar;
				  firstCharStored = 0;
				  i = 1;
			  }
			  
			  current = getchar();		//read character			  
			  
			  if (feof(stdin))
			  {
				  goto end;	//skip to end of program without printing
			  }
			  
			  if(current == '*')
			  {
				  nextChar = getchar();		//read next character

				  if(nextChar == '*')		//possibility 1 ** record only ^
				  {
					  outputLine[i] = '^';
				  }
				  else						//otherwise
				  {
					  outputLine[i] = '*';	//set character
					  i++;	
					  current = nextChar;	//record the current and continue
				  }

			  }		


			  if(current == '*')
			  {
				  //do nothing
			  }
			  else if(current == '\n')	//check for new line replacement with space
				{
					if(i == LINESIZE)
					{
						firstChar = ' ';
						firstCharStored = 1;							  
					}
					else
					{
						outputLine[i] = ' ';
					}
						  
				}
				else		//any other character
				{
					if(i == LINESIZE)
					{
						firstChar = current;
						firstCharStored = 1;							  
					}
					else 
					{
						outputLine[i] = current;	
					}
				}

			  

		  } 	
		  
			printf("%s\n",outputLine);


	  } while(1);

		end:


	  return(0);
}


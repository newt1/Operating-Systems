Copy everything in this folder into the directory where you have your c file.

Compile with the following command:

gcc <your c file> semaphore.o -L. -lst -o <out file name>

//run this command for testing
gcc HW3.c buffer.c semaphore.o -L. -lst -o HW3

gcc -g HW3.c buffer.c semaphore.o -L. -lst -o HW3
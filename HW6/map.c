
#include "map.h"

//borrowed from example
//changed return type to void* to be able to return any kind of pointer to the file
void* createMMAP(size_t size){
	//These are the neccessary arguments for mmap. See man mmap.
	void* addr = 0;
	int protections = PROT_READ|PROT_WRITE; //can read and write
	int flags = MAP_SHARED|MAP_ANONYMOUS; //shared b/w procs & not mapped to a file
	int fd = -1; //We could make it map to a file as well but here it is not needed.
	off_t offset = 0;
	  
	//Create memory map
	void* fileAddress =  mmap(addr, size, protections, flags, fd, offset);
	  
	if (fileAddress == MAP_FAILED){//on an error mmap returns void* -1.
	  perror("error with mmap");
	  exit(EXIT_FAILURE);
	}
	return fileAddress;
}
  
//borrowed from example
//added size parameter to allow deleting files of different sizes
void deleteMMAP(void* addr, size_t size){
	  //This deletes the memory map at given address. see man mmap
	  if (munmap(addr, size) == -1){
		  perror("error deleting mmap");
		  exit(EXIT_FAILURE);
	  }
  }
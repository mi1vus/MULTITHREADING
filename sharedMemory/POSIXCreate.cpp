#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <fstream> //files
#include <errno.h> //errors

#include <stdio.h> //printf
#include <unistd.h> // sys calls
#include <sys/types.h> // O_ constants
#include <sys/ipc.h>  // IPC_ constnts
#include <sys/stat.h> // mode constants
#include <sys/mman.h> // POSIX shared mem
#include <fcntl.h> // O_ constants




#define MB 1048576

char in [1000];

int main(int argc, char** argv) {
	int shmid = shm_open("/test.shm",(O_CREAT|O_RDWR),(S_IRWXU|S_IRWXG|S_IRWXO));
	if (shmid >= 0){
		ftruncate(shmid,MB);

		void * mem = mmap(NULL, MB, (PROT_READ | PROT_WRITE), MAP_SHARED, shmid, 0);
		if (mem != MAP_FAILED){
			memset(mem,13,MB);
		} else {
			printf ("Ошибка mmap = %s\n", strerror(errno));	
			return 1;			
		}
	} else {
		printf ("Ошибка shm_open = %s\n", strerror(errno));	
		return -1*shmid;			
	}
}

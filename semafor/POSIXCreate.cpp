
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

#include <sys/stat.h> // mode constant
#include <fcntl.h> // O_* constants
#include <semaphore.h> // POSIX semafores

char in [1000];

int main(int argc, char** argv) {
	sem_t* semid = sem_open("/test.sem",(O_CREAT|O_RDWR),(S_IRWXU|S_IRWXG|S_IRWXO),66);
	if (semid !=  SEM_FAILED){
		int* n = new int;
		sem_getvalue(semid,n);
		printf ("Семафор = %d\n", *n);
	} else {
		printf ("Ошибка semopen = %s\n", strerror(errno));	
		return 1;			
	}
}

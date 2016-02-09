
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <fstream>
#include <errno.h> //errors

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h> // mode constant
#include <fcntl.h> // O_* constants
#include <semaphore.h>

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

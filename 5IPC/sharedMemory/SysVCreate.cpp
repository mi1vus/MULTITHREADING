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
#include <sys/shm.h> //shared memory

#define MB 1048576

char in [1000];

int main(int argc, char** argv) {
	key_t key = ftok("/home/max/box/CPP/sharedMemory/tmp/mem2.temp", 1);
	if (key >= 0){
		int shmid = shmget(key,MB,(IPC_CREAT|511));
		if (shmid >= 0){
			void * mem = shmat(shmid, NULL, 16);
			if (mem > 0){
				memset(mem,42,MB);
			} else {
				printf ("Ошибка shmat = %s\n", strerror(errno));	
				return -1*((int)mem);			
			}
		} else {
			printf ("Ошибка shmget = %s\n", strerror(errno));	
			return -1*shmid;			
		}
	} else {
		printf ("Ошибка ftok = %s\n", strerror(errno));	
		return -1*key;			
	}
}

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
#include <fcntl.h> // O_ constants
#include <pthread.h> // POSIX multi thread



#define MB 1048576

char in [1000];

void* thread_funct(void * arg){
	printf ("In thread \n");
	return arg;
}

int main(int argc, char** argv) {
	pthread_t pid;
	int arg = 0;
	pthread_attr_t attr;
	printf ("Init \n");
	int initRes = pthread_attr_init(&attr);
	if (initRes != 0)
	{
		printf ("Go pause\n");	
		return 1;
	}
	printf ("Create \n");
	int createRes = pthread_create(&pid, &attr, thread_funct, &arg);
	printf ("After create \n");
	if (createRes == 0){
		int procid = getpid();
		printf ("SAVE pid = %d\n", procid);
		std::ofstream log;
		log.open("/home/olga/Git/CPP/mThread/message.txt", std::ios::out);
		log << procid << std::endl;
		log.close();	
		pause();
	} else {
		printf ("Ошибка pthread_create = %s\n", strerror(createRes));	
		return -1*createRes;			
	}
}

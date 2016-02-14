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
#include <sys/socket.h> // socketpair

#define MB 1048576

char buf[MB];

int main(int argc, char** argv) {
	int pfd[2];
	int sockRes = socketpair(AF_UNIX,SOCK_STREAM,0/*IPPROTO_TCP*/,pfd);
	if (sockRes == 0){
		if (!fork()){
			close(pfd[1]);
			const char hello[] = "hello parent, I am child";
			int writed = write(pfd[0], hello, sizeof(hello));
			printf("Write over\n");
			pause();
		} else {
			close(pfd[0]);
			sleep(1);
			int readed = read(pfd[1], buf, MB);
			if (readed > 0){
				buf[readed] = '\0';
				printf("Received %d: %s\n", readed, buf);
			}
			pause();
		}
	} else {
		printf ("Ошибка shm_open = %s\n", strerror(errno));	
		return errno;			
	}
}

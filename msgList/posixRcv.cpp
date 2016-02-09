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
#include <sys/msg.h> // SysV message queue

#include <sys/stat.h> // mode constant
#include <fcntl.h> // O_* constants
#include <mqueue.h> //POSIX message queue
 

char in [1000];

int main(int argc, char** argv) {
	const char* msg = "Hello!w";

	mqd_t msqid = mq_open("/test.mq",(O_RDONLY/*|O_EXCL*/));

	if (msqid != -1){
		int res = mq_receive(msqid,in,1000,NULL);
		if (res >= 0){
			in[res] = '\0';
			std::ofstream log;
			log.open("/home/box/message.txt", std::ios::out);
			log << in << std::endl;
			log.close();	
		} else {
			printf ("Ошибка получения = %s\n", strerror(errno));	
			return -1*res;
		}
	} else {
		printf ("Ошибка создания = %s\n", strerror(errno));	
		return -1*msqid;
	}

	int rc = mq_close(msqid);
}

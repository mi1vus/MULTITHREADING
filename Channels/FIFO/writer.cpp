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


#define MB 1048576

char in [MB];

int main(int argc, char** argv) {
	/* write "Hi" to the FIFO */
	int cnt = 10;	
	char tmp[12]={0x0};
	while (cnt > 0){
		int fd = open("in.fifo", O_WRONLY | O_NONBLOCK);
		sprintf(tmp,"%11d", cnt);
		int writed = write(fd, tmp, sizeof(tmp));
		close(fd);
		sleep(1);
		cnt--;
	}
	return 0;
}

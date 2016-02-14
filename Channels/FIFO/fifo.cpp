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

char buf[MB];

int main(int argc, char** argv) {
	int in = mkfifo("in.fifo",(S_IRWXU|S_IRWXG|S_IRWXO));
	int out = mkfifo("out.fifo",(S_IRWXU|S_IRWXG|S_IRWXO));

	int fd,readed,writed;
	while(1){
		/* open, read, and display the message from the FIFO */
		fd = open("in.fifo", O_RDONLY);
		readed = read(fd, buf, MB);
		if (readed > 0){
			buf[readed] = '\0';
			printf("Received: %s\n", buf);
			close(fd);

			/* write "Hi" to the FIFO */
			fd = open("out.fifo", O_WRONLY | O_NONBLOCK);
			writed = write(fd, buf, readed);
			printf("Write over\n");
		}
		close(fd);
	}
	/* remove the FIFO */
	unlink("out.fifo");
	unlink("in.fifo");
}

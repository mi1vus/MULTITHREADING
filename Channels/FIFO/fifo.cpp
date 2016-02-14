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
	int in = mkfifo("/home/box/in.fifo",(S_IRWXU|S_IRWXG|S_IRWXO));
	int out = mkfifo("/home/box/out.fifo",(S_IRWXU|S_IRWXG|S_IRWXO));
	int fd_in = open("/home/box/in.fifo", O_RDONLY);
	int fd_out = open("/home/box/out.fifo", O_WRONLY | O_NONBLOCK);

	int fd,readed,writed;
	//while(1){
		/* open, read, and display the message from the FIFO */
		readed = read(fd_in, buf, MB);
		//if (readed > 0){
			buf[readed] = '\0';
			//printf("Received: %s\n", buf);

			/* write "Hi" to the FIFO */
			writed = write(fd_out, buf, readed);
			//printf("Write over\n");
		//}
	//}
	close(fd_in);
	close(fd_out);
	/* remove the FIFO */
	unlink("/home/box/out.fifo");
	unlink("/home/box/in.fifo");
}

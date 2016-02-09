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
//	key_t key = ftok("/tmp/msg.temp", 1);

		struct mq_attr attr, *buf;
		buf = &attr;
		attr.mq_maxmsg = 100;
		attr.mq_msgsize = 500;
		attr.mq_flags = 0;
		attr.mq_curmsgs = 0;
//mode_t omask;
//omask = umask(0);

	mqd_t msqid = mq_open("/test.mq",(O_CREAT|O_WRONLY/*|O_EXCL*/),(S_IRWXU|S_IRWXG|S_IRWXO),buf);

//umask(omask);
	if (msqid != -1){
			/* Вывести изменившиеся поля */
			printf ("Число сообщений в очереди = %ld\n", buf->mq_curmsgs);
			printf ("Максимальный размер сообщения = %ld\n", buf->mq_msgsize);
			printf ("Максимальное число сообщений = %ld\n", buf->mq_maxmsg);
			printf ("Флагов = %ld\n", buf->mq_flags);
			printf ("mqid = %d\n", msqid);
		int res = mq_send(msqid,msg,9,10);
		if (res >= 0){
			printf ("Отправлено сообщение %d = %s\n", sizeof(msg), msg);		
		} else {
			printf ("Ошибка отправки = %s\n", strerror(errno));	
			return -1*res;
		}
	} else {
		printf ("Ошибка создания = %s\n", strerror(errno));	
		return -1*msqid;
	}

	int rc = mq_close(msqid);
//	rc = mq_unlink("/temp.mq");
}

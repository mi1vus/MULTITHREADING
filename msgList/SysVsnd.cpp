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
char in [1000];

struct message {
    long mtype;
    char mtext[6];
};

int main(int argc, char** argv) {
	struct message mg;
	mg.mtype = 10;
	char* msg = "Hello!w";
	strcpy(mg.mtext, msg);
	long dummy;
	key_t key = ftok("/tmp/msg.temp", 1);
	int msqid = msgget(key,(IPC_CREAT|511));
	int res = msgsnd(msqid,&mg,(sizeof(mg)-sizeof(dummy)),0);
	if (res >= 0){
		struct msqid_ds msqid_ds, *buf;
		buf = &msqid_ds;
		/* Опрос измененной структуры данных */
		msgctl (msqid, IPC_STAT, buf);
		/* Вывести изменившиеся поля */
		printf ("Число сообщений в очереди = %d\n", buf->msg_qnum);
		printf ("Ид-р последнего отправителя = %d\n", buf->msg_lspid);
		printf ("Время последнего отправления = %d\n", buf->msg_stime);	
	} else {
		return -1*res;
	}
}

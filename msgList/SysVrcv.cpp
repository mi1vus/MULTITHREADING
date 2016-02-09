#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <fstream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

char in [1000];

struct message {
    long mtype;
    char mtext[80];
};

int main(int argc, char** argv) {
	struct message mg;
	long dummy;
	key_t key = ftok("/tmp/msg.temp", 1);
	int msgid = msgget(key,0);
	int res = msgrcv(msgid,&mg,(sizeof(mg)-sizeof(dummy)),0,0);
	if (res > 0){
		std::ofstream log;
		log.open("/home/box/message.txt", std::ios::out);
		log << mg.mtext << std::endl;
		log.close();

		mg.mtext[res-1] = '\0';
		std::cout << "size:" << res << std::endl;
		std::cout << "received:" << mg.mtext << std::endl;
	} else {
		return -1*res;
	}
}

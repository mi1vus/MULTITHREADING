/*************************************************************
Programm listen localhost:1112
*************************************************************/

﻿#include <stdio.h>
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"


int set_nonblock(int fd)
{
	int flags;
#if defined(O_NONBLOCK)
	﻿if(-1 == (flags = fcntl(fd, F_GETFL, 0)))
		﻿flags = 0;
	﻿return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	flags = 1;
	return ioctl(fd, FIOBIO, &flags);
#endif
}

int main(int argc, char** argv)
{
	int masterS = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(1112);
	sAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(masterS, (struct sockaddr *)(&sAddr), sizeof(sAddr));

	listen(masterS,SOMAXCONN);

	while (1)
	{
		int slaveS = accept(masterS, 0, 0);
		set_nonblock(slaveS);
		int Buf[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		int res = recv(slaveS, Buf, 10, MSG_NOSIGNAL);
		if (!res)
			continue;
		send(slaveS, Buf, 10, MSG_NOSIGNAL);
		shut_down(slaveS, SHUT_RDWR);
		close(slaveS);
	}
	return 0;
}


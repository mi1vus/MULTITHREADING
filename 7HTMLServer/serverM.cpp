//containers
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <cerrno> // errno
#include <stdio.h> // sprintf
#include <stdlib.h> // EXIT_SUCCESS и EXIT_FAILURE
#include <cstring> // strcat
#include <fstream> // file IO
//socket
#include "fcntl.h"
#include "sys/types.h"
#include <sys/ioctl.h>
#include "sys/socket.h"
#include "netinet/in.h"
#include "unistd.h" //getopt
#include <arpa/inet.h> //addr to ip - inet_ntoa
//epoll
#include "sys/epoll.h"

#define MaxReceivedEvents 50
#define MaxMessageLength 100

int set_nonblock(int fd)
{
	int flags;
	#if defined(O_NONBLOCK)
		if(-1 == (flags = fcntl(fd, F_GETFL, 0)))
			flags = 0;
		return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	#else
		flags = 1;
		return ioctl(fd, FIOBIO, &flags);
	#endif
}

char* getIPFromAddr(struct sockaddr_in slaveAddr){
	char * addr = inet_ntoa(slaveAddr.sin_addr);
	char port[8];
	sprintf(port, "%d", slaveAddr.sin_port);

	char* ip = new char[30];
	strcpy(ip,addr);
	strcat(ip,":");
	strcat(ip,port);
	return ip;
}
/*    
	std::ostream* fp = &cout;
	std::ofstream fout;
	if (argc > 1) {
		fout.open(argv[1]);
		fp = &fout;
	}
*/
struct Par_t {
	Par_t(int& EpollFd, int& fd, std::ostream* log):EpollFd(EpollFd),fd(fd),log(log){};
	int& EpollFd;
	int& fd;
	std::ostream* log;
};

void* processClientSend(void* ppars){
	Par_t* pars = (Par_t*)ppars;

	static char Buffer[MaxMessageLength];

	int reciveResult = recv(pars->fd,Buffer,MaxMessageLength,MSG_NOSIGNAL);
	//connection closed
	if ((reciveResult == 0) && (errno != EAGAIN)){
		//close connection
		shutdown(pars->fd,SHUT_RDWR);
		close(pars->fd);
		std::cout << "slave disconnected with FD = " << pars->fd << std::endl;
	} else
	//read data from slave
	if (reciveResult > 0){
		Buffer[reciveResult] = '\0';
		std::cout << " resultBuffer: recSize = " << reciveResult 
					    << " msg = \n" << Buffer << std::endl;
		send(pars->fd,Buffer,reciveResult,MSG_NOSIGNAL);
	}
}

void* processClientConnection(void* ppars){
	Par_t* pars = (Par_t*)ppars;
	
	//slave ip
	struct sockaddr_in* slaveAddr = (struct sockaddr_in*)malloc(sizeof(*slaveAddr));
	socklen_t* slaveAddrSize = (socklen_t*)malloc(sizeof(*slaveAddrSize));
	*slaveAddrSize = sizeof(*slaveAddr);

	//register new connection
	int slaveFD = accept(pars->fd, (sockaddr*)slaveAddr, slaveAddrSize);
	set_nonblock(slaveFD);

//	struct epoll_event Event;
	struct epoll_event* Event = (struct epoll_event*)malloc(sizeof(*Event));

	Event->data.fd = slaveFD;
	Event->events = EPOLLIN;// catch read event, all unread data generate new event

	// register slave Event listener in control
	epoll_ctl(pars->EpollFd,EPOLL_CTL_ADD,slaveFD,Event);

	char * ip = getIPFromAddr(*slaveAddr);

	std::cout << "slave connected with FD = " << slaveFD
			<< " AND slAddr = " << ip
			<< std::endl;
}





struct Args_t {
    //Args_t():ip(""),port(-1),dir(""){};
    char *ip;
    int port;
    char *dir;
};

void readArgs(int argc, char** argv, Args_t* res){
	const char *optString = "h:p:d:";
	int opt = getopt( argc, argv, optString);
	    while( opt != -1 ) {
		switch( opt ) {
		    case 'h':
			res->ip = optarg;
			break;			
		    case 'p':
			res->port = atoi(optarg) ;
			break;			
		    case 'd':
			res->dir = optarg;
			break;			
		    default:
			/* сюда на самом деле попасть невозможно. */
			break;
		}		
		opt = getopt( argc, argv, optString);
	    }
	    
	    //globalArgs.inputFiles = argv + optind;
	    //globalArgs.numInputFiles = argc - optind;
}


int main(int argc, char** argv)
{
/*DAEMONIZATION*/
/*	pid_t pid, sid;
	
	//Fork the Parent Process
	pid = fork();
	
	if (pid < 0) { exit(EXIT_FAILURE); }
	
	//We got a good pid, Close the Parent Process
	if (pid > 0) { exit(EXIT_SUCCESS); }
	
	//Create a new Signature Id for our child
	sid = setsid();
	if (sid < 0) { exit(EXIT_FAILURE); }
	
	//Change Directory
	//If we cant find the directory we exit with failure.
	if ((chdir("/home/olga/box/7HTMLServer/log")) < 0) { exit(EXIT_FAILURE); }
	
	//Close Standard File Descriptors
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
*/	
//----------------
//Main Process
//----------------
//LOG
	std::ofstream log("/home/olga/box/7HTMLServer/log/log.txt", std::ios_base::out | std::ios_base::trunc); // создаём объект класса ofstream для записи и связываем его с файлом cppstudio.txt
//READ ARGS
	struct Args_t params;
	readArgs(argc, argv,&params);
	if (	params.port == 0 ||
		params.ip == NULL ||
		params.dir == NULL		
		) { std::cout << "Not all params! Crash!"; log.close(); exit(EXIT_FAILURE); }

	// запись строки в файл
	std::cout << "1) Params = "
		<< params.ip << ":"
		<< params.port << " " 
		<< params.dir << std::endl << std::endl;
			
	// create master socket in core OS and return file descriptor
	int masterFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// socket settings
	struct sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(params.port);
	sAddr.sin_addr.s_addr = inet_addr(params.ip);//htonl(INADDR_ANY);
	bind(masterFD, (struct sockaddr *)(&sAddr), sizeof(sAddr));

	std::cout << " ip " << inet_ntoa(sAddr.sin_addr) << std::endl;
	
	// nonblock socket - do not wait all data
	// read any and continue
	set_nonblock(masterFD);

	//listen as server ip and port from settings
	listen(masterFD, SOMAXCONN);

	std::cout << "master begin listen with FD = " << masterFD << std::endl;

	// create poll in OS core and return file descriptor
	// poll for correct work with slave socket`s
	int EPollFD = epoll_create1(0);

	struct epoll_event Event;
	Event.data.fd = masterFD;
	Event.events = EPOLLIN;// catch read event, all unread data generate new event

	// register Event listener in control
	epoll_ctl(EPollFD,EPOLL_CTL_ADD,masterFD,&Event);

	while (true)
	{
		struct epoll_event Events[MaxReceivedEvents];
		// receive N events
		int N = -1;
		std::cout << "begin wait event..." << std::endl;
		// if N < 0 - core signal error - repeat wait
		while(true){
			N = epoll_wait(EPollFD,Events,MaxReceivedEvents,-1);
			if (N >= 0)
				break;
		}
		std::cout << "wait terminated..." << std::endl;

		for (int i = 0; i < N; ++i)
		{
			// new connection
			if (Events[i].data.fd == masterFD){
				//Par_t* parms = new Par_t(EPollFD,masterFD,&std::cout);
				//processClientSend(parms);
								//slave ip
				struct sockaddr_in slaveAddr; socklen_t slaveAddrSize = sizeof(slaveAddr);

				//register new connection
				int slaveFD = accept(masterFD, (sockaddr*)&slaveAddr, &slaveAddrSize);
				set_nonblock(slaveFD);

				struct epoll_event Event;
				Event.data.fd = slaveFD;
				Event.events = EPOLLIN;// catch read event, all unread data generate new event

				// register slave Event listener in control
				epoll_ctl(EPollFD,EPOLL_CTL_ADD,slaveFD,&Event);

				char * ip = getIPFromAddr(slaveAddr);

				//save to chat list
				//slaveSockets.insert(std::pair<unsigned int,std::string> (slaveFD, ip));

				std::cout << "slave connected with FD = " << slaveFD
						<< " AND slAddr = " << ip
						<< std::endl;
			} else
			// read data
			{
				int fdd = Events[i].data.fd;
				Par_t* parms = new Par_t(EPollFD,fdd,&std::cout);
				
				pthread_t pid;
				pthread_attr_t attr;
				std::cout << "new thread to receive message " << std::endl;
				int initRes = pthread_attr_init(&attr);
				int createRes = pthread_create(&pid, &attr, processClientSend, parms);
				if (createRes == 0){
				} else {
					printf ("Ошибка pthread_create = %s\n", strerror(createRes));	
					return -1*createRes;			
				}
				
			}
		}
	}
	log.close(); // закрываем файл
	return 0;
}

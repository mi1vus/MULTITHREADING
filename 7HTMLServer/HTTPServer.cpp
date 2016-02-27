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
#include "http_parser.h"


#define MAX_RECEIVED_EVENTS 50
#define MAX_MESSAGE_LENGTH 1048576


std::ostream* ostream;
char msg[MAX_MESSAGE_LENGTH];
static const char* test = "HTTP/1.0 200 OK\r\n"
			   "Content-length: 20\r\n"
			   "Connection: close\r\n"
			   "Content-Type: text/html\r\n"
			"\r\n"
			"12345678901234567890";
static const char* test2 = 	"GET /index.html?id=2 HTTP/1.0\r\n"
				"Host: ru.wikipedia.org";
static const char* test3 = 	"POST /index.html?id=2 HTTP/1.0\r\n"
				"Host: ru.wikipedia.org";

static const char* templ = "HTTP/1.0 200 OK\r\n"
		           "Content-length: %d\r\n"
		       	   "Connection: close\r\n"
		       	   "Content-Type: text/html\r\n"
		       	   "\r\n"
		       	   "%s";

static const char not_found[] = "HTTP/1.0 404 NOT FOUND\r\n"
				"Content-length: 0\r\n"
				"Connection: close\r\n"
				"Content-Type: text/html\r\n"
				"\r\n";

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
	Par_t(int& EpollFd, int& fd, std::ostream* log):EpollFd(EpollFd),fd(fd),log(log),sendedAnswer(0){};
	int EpollFd;
	int fd;
	int sendedAnswer;
	std::ostream* log;
};

int on_message_begin(http_parser* _) {
  (void)_;
  (*ostream) << "\n***MESSAGE BEGIN***\n\n";
  return 0;
}

int on_headers_complete(http_parser* _) {
  (void)_;
  (*ostream) << "\n***HEADERS COMPLETE***\n\n";
  return 0;
}

int on_message_complete(http_parser* _) {
  (void)_;
  (*ostream) << "\n***MESSAGE COMPLETE***\n\n";
  return 0;
}

int on_url(http_parser* parser, const char* at, size_t length) {
	Par_t* pars = (Par_t*)(parser->data);
	Par_t p = * pars;
	char* buf = new char[1024];
	int exists = -1;
	for(int i=0; i < length; i++ )
		if (at[i] == '?')
			exists = i;
	strcpy (buf, at);
	if (exists != -1)
		buf[exists] = '\0';
	else
		buf[length] = '\0';

	if (buf[0] == '/')
		buf = buf + 1;

	unsigned int size = sprintf(msg, "Url: %.*s\n", (int)length, at);
	msg[size] = 0;
	(*ostream) << msg;
	size = sprintf(msg, "New Url: %s\n", buf);
	msg[size] = 0;
	(*ostream) << msg;

	FILE *f = fopen(buf, "rb");

	if (f!= NULL){
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		char *string = (char*) malloc(fsize + 1);
		fread(string, fsize, 1, f);
		fclose(f);

		string[fsize] = 0;

		unsigned int size = sprintf(buf, templ, fsize, string);

		//unsigned int size = (unsigned)strlen(not_found);
		int sended = send(pars->fd,buf,size,MSG_NOSIGNAL);
		size = sprintf(msg, "send OK: %ds\n", sended);
		msg[size] = 0;
		(*ostream) << msg;

		pars->sendedAnswer = sended;
	}
	return 0;
}

int on_header_field(http_parser* _, const char* at, size_t length) {
  (void)_;
  unsigned int size = sprintf(msg, "Header field: %.*s\n", (int)length, at);
  msg[size] = 0;
  (*ostream) << msg;
  return 0;
}

int on_header_value(http_parser* _, const char* at, size_t length) {
  (void)_;
  unsigned int size = sprintf(msg, "Header value: %.*s\n", (int)length, at);
  msg[size] = 0;
  (*ostream) << msg;
  return 0;
}

int on_body(http_parser* _, const char* at, size_t length) {
  (void)_;
  unsigned int size = sprintf(msg, "Body: %.*s\n", (int)length, at);
  msg[size] = 0;
  (*ostream) << msg;
  return 0;
}

void usage(const char* name) {
  fprintf(stderr,
          "Usage: %s $type $filename\n"
          "  type: -x, where x is one of {r,b,q}\n"
          "  parses file as a Response, reQuest, or Both\n",
          name);
  exit(EXIT_FAILURE);
}

bool parseRequest(void* ppars, char* buf, size_t len){
	  http_parser_settings settings;
	  memset(&settings, 0, sizeof(settings));
	  settings.on_message_begin = on_message_begin;
	  settings.on_url = on_url;
	  settings.on_header_field = on_header_field;
	  settings.on_header_value = on_header_value;
	  settings.on_headers_complete = on_headers_complete;
	  settings.on_body = on_body;
	  settings.on_message_complete = on_message_complete;
/*
	  http_parser parserResp;
	  http_parser_init(&parserResp, HTTP_RESPONSE);
	  size_t nparsed = http_parser_execute(&parserResp, &settings, test, strlen(test));


	  if (nparsed != (size_t)strlen(test)) {
	    fprintf(stderr,
	            "Error: %s (%s)\n",
	            http_errno_description(HTTP_PARSER_ERRNO(&parserResp)),
	            http_errno_name(HTTP_PARSER_ERRNO(&parserResp)));
	  }
*/
	  http_parser parser;
	  http_parser_init(&parser, HTTP_REQUEST);
	  parser.data = ppars;
	  size_t nparsed2 = http_parser_execute(&parser, &settings, buf, len);

	  Par_t* pars = (Par_t*)(parser.data);
	  if (pars->sendedAnswer <= 0)
		  return false;
	  else
		  return true;
/*	  if (nparsed2 != (size_t)strlen(test2)) {
	    fprintf(stderr,
	            "Error: %s (%s)\n",
	            http_errno_description(HTTP_PARSER_ERRNO(&parserReqw)),
	            http_errno_name(HTTP_PARSER_ERRNO(&parserReqw)));
	  }
*/
}


void* processClientSend(void* ppars){
	Par_t* pars = (Par_t*)ppars;

	static char Buffer[MAX_MESSAGE_LENGTH];

	int reciveResult = recv(pars->fd,Buffer,MAX_MESSAGE_LENGTH,MSG_NOSIGNAL);
	//connection closed
	if ((reciveResult == 0) && (errno != EAGAIN)){
		//close connection
		shutdown(pars->fd,SHUT_RDWR);
		close(pars->fd);
		(*ostream) << "slave disconnected with FD = " << pars->fd << std::endl;
	} else
	//read data from slave
	if (reciveResult > 0){
		Buffer[reciveResult] = '\0';
		(*ostream) << " resultBuffer: recSize = " << reciveResult
					    << " msg = \n" << Buffer << std::endl;
//PARSE REQUEST AND SEND RESPONSE FROM CALLBACK

		if(!parseRequest(ppars, Buffer, reciveResult)){
			unsigned int size = (unsigned)strlen(not_found);
			send(pars->fd,not_found,size,MSG_NOSIGNAL);
			//send(pars->fd,Buffer,reciveResult,MSG_NOSIGNAL);
		}
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

	(*ostream) << "slave connected with FD = " << slaveFD
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
}


int main(int argc, char** argv)
{
//LOG
	std::ofstream log("/home/olga/box/7HTMLServer/log/log.txt", std::ios_base::out | std::ios_base::trunc); // создаём объект класса ofstream для записи и связываем его с файлом cppstudio.txt
	//ostream = &(std::cout);
	ostream = &log;
//READ ARGS
	struct Args_t params;
	readArgs(argc, argv,&params);
	if (	params.port == 0 ||
		params.ip == NULL ||
		params.dir == NULL
		) { (*ostream) << "Not all params! Crash!"; log.close(); exit(EXIT_FAILURE); }

	// запись строки в файл
	(*ostream) << "1) Params = "
		<< params.ip << ":"
		<< params.port << " "
		<< params.dir << std::endl << std::endl;

/*DAEMONIZATION*/
	pid_t pid, sid;

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
	if ((chdir(params.dir)) < 0) { exit(EXIT_FAILURE); }

	//Close Standard File Descriptors
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

//----------------
//Main Process
//----------------
	// create master socket in core OS and return file descriptor
	int masterFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// socket settings
	struct sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(params.port);
	sAddr.sin_addr.s_addr = inet_addr(params.ip);//htonl(INADDR_ANY);
	bind(masterFD, (struct sockaddr *)(&sAddr), sizeof(sAddr));

	(*ostream) << " ip " << inet_ntoa(sAddr.sin_addr) << std::endl;

	// nonblock socket - do not wait all data
	// read any and continue
	set_nonblock(masterFD);

	//listen as server ip and port from settings
	listen(masterFD, SOMAXCONN);

	(*ostream) << "master begin listen with FD = " << masterFD << std::endl;

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
		struct epoll_event Events[MAX_RECEIVED_EVENTS];
		// receive N events
		int N = -1;
		(*ostream) << "begin wait event..." << std::endl;
		// if N < 0 - core signal error - repeat wait
		while(true){
			N = epoll_wait(EPollFD,Events,MAX_RECEIVED_EVENTS,-1);
			if (N >= 0)
				break;
		}
		(*ostream) << "wait terminated..." << std::endl;

		for (int i = 0; i < N; ++i)
		{
			// new connection
			if (Events[i].data.fd == masterFD){
				//Par_t* parms = new Par_t(EPollFD,masterFD,&(*ostream));
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

				(*ostream) << "slave connected with FD = " << slaveFD
						<< " AND slAddr = " << ip
						<< std::endl;
			} else
			// read data
			{
				int fdd = Events[i].data.fd;
				Par_t* parms = new Par_t(EPollFD,fdd,ostream);

				pthread_t pid;
				pthread_attr_t attr;
				(*ostream) << "new thread to receive message " << std::endl;
				int initRes = pthread_attr_init(&attr);
				int createRes = pthread_create(&pid, &attr, processClientSend, parms);
				if (createRes == 0){
				} else {
					(*ostream) <<  ("Ошибка pthread_create = %s\n", strerror(createRes));
					return -1*createRes;
				}

			}
		}
	}
	log.close(); // закрываем файл
	return 0;
}

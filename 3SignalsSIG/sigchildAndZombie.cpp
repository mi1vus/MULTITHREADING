//============================================================================
// Name        : sigchid.cpp
// Author      : Max
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <fstream>
#include <iomanip>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


using namespace std;

void handler(int signum){
	if (signum == SIGCHLD){
		int* t = new int;
		waitpid(-1,t,WNOHANG);
	}
}

int main(int argc, char ** argv)
{
	pid_t pid = fork();

	//parent proc
	if (pid != 0){
		ofstream f;
		f.open("//home//olga//box//pid_child", ios::out);
		f << pid << "\n" ;
		f.close();

		//blocking signals in signal handler - all block
		sigset_t set;
		sigfillset(&set);

		struct sigaction kill_zombie;
			kill_zombie.sa_handler = handler;
			kill_zombie.sa_mask = set;
			kill_zombie.sa_flags = SA_RESTART;


		//old action befor change
		struct sigaction  old;

		//ignore soft kill and interrupt(ctl+C)
//			sigaction(SIGINT,&ignore, &old);
		if (argc < 2)
			sigaction(SIGCHLD,&kill_zombie, &old);

		//blocking signals in signal handler - all block
		sigset_t set2;
		sigfillset(&set2);
		sigdelset(&set2,SIGCHLD);

		int n;
		sigwait(&set2,&n);
	}
	//child process
	else {
		int ppid = getppid();

		ofstream f;
		f.open("//home//olga//box//pid_parent", ios::out);
		f << ppid << "\n" ;
		f.close();

		if (argc < 2){
			int pid = getpid();



			//blocking signals in signal handler - all block
			sigset_t set;
			sigfillset(&set);

			//action on signal receive - ignore
			struct sigaction  ignore;
				ignore.sa_handler = SIG_IGN;
				ignore.sa_mask = set;
				ignore.sa_flags = SA_RESTART;

			struct sigaction kill_zombie;
				ignore.sa_handler = SIG_IGN;
				ignore.sa_mask = set;
				ignore.sa_flags = SA_RESTART;


			//old action befor change
			struct sigaction  old;

			//ignore soft kill and interrupt(ctl+C)
//			sigaction(SIGINT,&ignore, &old);
//			sigaction(SIGTERM,&ignore, &old);
		}

		pause();
	}

	return 0;
}

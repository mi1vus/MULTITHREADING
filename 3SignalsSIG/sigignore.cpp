#include <iostream>
#include <fstream>
#include <iomanip>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

int main(int argc, char ** argv)
{
	pid_t pid = fork();

	//parent proc
	if (pid != 0){
		ofstream f;
		f.open("//home//box//pid", ios::out);

		//pid - child pid
		f << pid << "\n" ;

		f.close();
	}
	//child process
	else {
//		int ppid = getppid();
//		ofstream f;
//		f.open("//home//olga//ppid", ios::out);
//
//		//parent pid for child
//		f << ppid << "\n" ;
//
//		f.close();

		if (argc < 2){
			//blocking signals in signal handler - all block
			sigset_t set;
			sigfillset(&set);

			//action on signal receive - ignore
			struct sigaction  ignore;
				ignore.sa_handler = SIG_IGN;
				ignore.sa_mask = set;
				ignore.sa_flags = SA_RESTART;

			//old action befor change
			struct sigaction  old;

			//ignore soft kill and interrupt(ctl+C)
			sigaction(SIGINT,&ignore, &old);
			sigaction(SIGTERM,&ignore, &old);
		}

		pause();
	}

	return 0;
}

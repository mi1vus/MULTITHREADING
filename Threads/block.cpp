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
#include <pthread.h> //threads

//#define PTHREAD_COND_INITIALIZER {_PTHREAD_COND_SIG_init, {0}} // initializer to init macros

#define MB 1024
char in[MB];

using namespace std;
// type-definition: 'pt2Function' now can be used as type
typedef void * (*pt2Function)(void *);

#define NUM_THREADS     2

void *fun0(void *threadid)
{
	long tid;
	pthread_mutex_t* mut = (pthread_mutex_t*)threadid;

	pthread_cond_t cond;// = PHTREAD_COND_INITIALIZER;
	pthread_cond_init(&cond,NULL);

	tid = (long)threadid;
	cout << "Condition variable wait, " << tid << endl;
	pthread_cond_wait(&cond,mut);
	cout << "Condition variable end wait, " << tid << endl;
	pthread_cond_destroy(&cond);
	pthread_exit(NULL);
}
void *fun1(void *threadid)
{
	long tid;
	pthread_barrier_t* bp = (pthread_barrier_t*)threadid;
	tid = (long)threadid;
	cout << "Barrier wait, " << tid << endl;
	pthread_barrier_wait(bp);
	cout << "Barrier end wait, " << tid << endl;
	pthread_barrier_destroy(bp);
	pthread_exit(NULL);
}


int main (int argc, char** argv)
{
	pthread_t threads[NUM_THREADS];
	pt2Function functions[NUM_THREADS];
	void ** args = new void*[NUM_THREADS];
	functions[0] = &fun0;
	functions[1] = &fun1;
	int rc;
	int i;

	int fd = open("main.pid", O_WRONLY | O_CREAT, 0666);
	sprintf(in,"%d", getpid());
	int writed = write(fd, in, sizeof(rc));
	close(fd);

	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_barrier_t bp;	
	pthread_barrier_init(&bp,NULL,2);

	i=0;args[i] = (void *)&mut;
	i=1;args[i] = (void *)&bp;
	for( i=0; i < NUM_THREADS; i++ ){
//		cout << "main() : creating thread, " << i << endl;
		
		rc = pthread_create(&threads[i], NULL, functions[i], args[i]);
		if (rc){
			cout << "Error:unable to create thread," << rc << endl;
			exit(-1);
		}
	}
	//pause();
	pthread_exit(NULL);
}

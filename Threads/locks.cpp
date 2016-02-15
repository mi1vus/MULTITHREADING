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


#define MB 1024
char in[MB];

using namespace std;
// type-definition: 'pt2Function' now can be used as type
typedef void * (*pt2Function)(void *);

#define NUM_THREADS     4

void *fun0(void *threadid)
{
	long tid;
	pthread_mutex_t* mut = (pthread_mutex_t*)threadid;
	tid = (long)threadid;
	cout << "Mutex wait, " << tid << endl;
	pthread_mutex_lock(mut);
	cout << "Mutex end wait, " << tid << endl;
	pthread_exit(NULL);
}
void *fun1(void *threadid)
{
	long tid;
	pthread_spinlock_t* mut = (pthread_spinlock_t*)threadid;
	tid = (long)threadid;
	cout << "Spin wait, " << tid << endl;
	pthread_spin_lock(mut);
	cout << "Spin end wait, " << tid << endl;
	pthread_exit(NULL);
}
void *fun2(void *threadid)
{
	long tid;
	pthread_rwlock_t* mut = (pthread_rwlock_t*)threadid;
	tid = (long)threadid;
	cout << "rlock wait, " << tid << endl;
	pthread_rwlock_rdlock(mut);
	cout << "rlock end wait, " << tid << endl;
	pthread_exit(NULL);
}
void *fun3(void *threadid)
{
	long tid;
	pthread_rwlock_t* mut = (pthread_rwlock_t*)threadid;
	tid = (long)threadid;
	cout << "wlock wait, " << tid << endl;
	pthread_rwlock_wrlock(mut);
	cout << "wlock end wait, " << tid << endl;
	pthread_exit(NULL);
}

int main (int argc, char** argv)
{
	pthread_t threads[NUM_THREADS];
	pt2Function functions[NUM_THREADS];
	void ** args = new void*[NUM_THREADS];
	functions[0] = &fun0;
	functions[1] = &fun1;
	functions[2] = &fun2;
	functions[3] = &fun3;
	int rc;
	int i;

	int fd = open("main.pid", O_WRONLY | O_CREAT, 0666);
	sprintf(in,"%d", getpid());
	int writed = write(fd, in, sizeof(rc));
	close(fd);

	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&mut);
	pthread_spinlock_t spin;
	pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE);
	pthread_spin_lock(&spin);
	pthread_rwlock_t rlock = PTHREAD_RWLOCK_INITIALIZER;
	pthread_rwlock_rdlock(&rlock);
	pthread_rwlock_t wlock = PTHREAD_RWLOCK_INITIALIZER;
	pthread_rwlock_wrlock(&wlock);
	i=0;args[i] = (void *)&mut;
	i=1;args[i] = (void *)&spin;
	i=2;args[i] = (void *)&wlock;
	i=3;args[i] = (void *)&wlock;
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

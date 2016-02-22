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
#include <stdlib.h>
#include <unistd.h> // sys calls
#include <sys/types.h> // O_ constants
#include <sys/ipc.h>  // IPC_ constnts
#include <sys/stat.h> // mode constants
#include <fcntl.h> // O_ constants
#include <omp.h> // pragms


#define MB 1024
char in[MB];

using namespace std;
// type-definition: 'pt2Function' now can be used as type
typedef void * (*pt2Function)(void *);

#define NUM_THREADS     4

int foo(int i){
	return i*i;
}

int main (int argc, char** argv)
{
	int count;
	#pragma omp parallel num_threads(10)
	{	
		//count = 0;

		int tid = omp_get_thread_num();
		printf("Hello world from thread = %d \n",tid);
		if(tid == 0){
			int nthreads = omp_get_num_threads();
			printf("Number of threads = %d\n",nthreads);
		}
		#pragma omp for reduction(+:count)
		for (int i = 0; i < 20;i++)
		{
			int tid = omp_get_thread_num();
			printf("Hello world from thread = %d i = %d \n",tid,i);
			count += foo(i);
		}
	}
	printf("count = %d \n",count);

}

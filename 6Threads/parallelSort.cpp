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

#define MB 1000000
char in[MB];
int mass[MB/4];

using namespace std;
// type-definition: 'pt2Function' now can be used as type
typedef void * (*pt2Function)(void *);

// Слияние двух упорядоченных массивов
// m1 - Первый массив
// m2 - Второй массив
// l1 - Длина первого массива
// l2 - Длина второго массива
// Возвращает объединённый массив
template <class T>
T* merge(T *m1, T* m2, int l1, int l2)
{
    T* ret = new T[l1+l2];
    int n = 0;
    {
	    // Сливаем массивы, пока один не закончится
	    while (l1 && l2) {
		if (*m1 < *m2) {
		    ret[n] = *m1;
		    m1++;
		    --l1;
		} else {
		    ret[n] = *m2;
		    ++m2;
		    --l2;
		}
		++n;
	    }
	    // Если закончился первый массив
	    if (l1 == 0) {
		for (int i = 0; i < l2; ++i) {
		    ret[n++] = *m2++;
		}
	    } else { // Если закончился второй массив
		for (int i = 0; i < l1; ++i) {
		    ret[n++] = *m1++;
		}
	    }
    }
    return ret;
}

// Функция восходящего слияния
template <class T>
void mergeSort(T * mas, int len)
{
    int n = 1, l, ost,cnt=1,calc;
    T * mas1;
    while (n < len) 
    {
        l = 0;
       
//	printf("n= %d len = %d\n",n,len);
	calc = len%(n*2) == 0 ? len/(n*2) : len/(n*2) + 1;
//	printf("cnt = %d calc = %d\n",cnt,calc);
	cnt = 1;

        //#pragma omp parallel shared(n,l,len,mas,mas1)
        { 
        	//#pragma omp for
			for (int i = 0; i < calc + 1; ++i)
			{
				if (l < len) {
//				    printf("cnt = %d l= %d len = %d\n", cnt, l, len);
				    if (l + n < len){ 
					int tid = omp_get_thread_num();
					if(tid == 0){
					int nthreads = omp_get_num_threads();
				//	printf("Number of threads = %d\n",nthreads);
					}

					    ost = (l + n * 2 > len) ? (len - (l + n)) : n;
					    mas1 = merge(mas + l, mas + l + n, n, ost);
					    for (int i = 0; i < n + ost; ++i)
						mas[l+i] = mas1[i];//тут нужно что-то поменять, потому что это лишнее копирование, и оно увеличивает время работы алгоритма в два раза
					    //delete [] mas1;
					    l += n * 2;
					    ++cnt;
				    }
				}
			}
		//#pragma omp barrier	
        }
        n *= 2;
    }
}

int main (int argc, char** argv)
{
	int count,num;
	//#pragma omp parallel num_threads(10)
	{	
		count = 0;
		
//		ifstream file("in.txt");
		int size = read(STDIN_FILENO, in, MB);
		in[size] = '\0';
		stringstream file(in);
		while(file >> num){
			mass[count] = num;
			++count;
		}

		mergeSort(mass,count);
		
		for(int i = 0; i < count;++i){
			std::cout << mass[i] << " ";
		}
	}
	//printf("count = %d \n",count);
}

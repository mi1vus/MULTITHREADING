
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <fstream>
#include <errno.h> //errors

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

char in [1000];

int main(int argc, char** argv) {
	key_t key = ftok("/home/olga/Git/CPP/semafor/tmp/sem.temp", 1);
	if (key >= 0){
		int semid = semget(key,16,IPC_CREAT);
		if (semid >= 0){
			int ctl = 0;
			for (int i = 0; i < 16; ++i){
				struct semid_ds * par = new struct semid_ds();
				/*int retrn = semctl(semid, i, IPC_STAT, par);
				if(retrn == -1)
				{
					printf ("Ошибка semctl %d 1 = %s\n",i, strerror(errno));	
					return -1*retrn;			
				}*/
				par->sem_perm.mode = 511;
				ctl = semctl(semid, i, IPC_SET, par);
				if(ctl == -1)
				{
					printf ("Ошибка semctl %d 2 = %s\n",i, strerror(errno));	
					return -1*ctl;			
				}
			}
			struct sembuf* sops = new struct sembuf[16];
			for (int i = 0; i < 16; ++i){
				struct sembuf * buf = new struct sembuf();
				buf->sem_num = i;
				buf->sem_op = i;
				buf->sem_flg = 0;
				sops[i] = *buf;
			}
			int res = semop(semid, sops, 16);
			if (res >= 0){
				
			} else {
				printf ("Ошибка semop = %s\n", strerror(errno));	
				return -1*res;			
			}
		} else {
			printf ("Ошибка semget = %s\n", strerror(errno));	
			return -1*semid;			
		}
	} else {
		printf ("Ошибка ftok = %s\n", strerror(errno));	
		return -1*key;			
	}
}

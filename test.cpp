#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM 1024


int main() {

	char c;
	int shmid;
	key_t key;
	int *shm, *s;

	puts("You are currenty running CLIENT program");


	key = 5555;

	if ((shmid = shmget(key, SHM, IPC_CREAT | 0666)) < 0) {
		perror("Error geting shared memory");
		return 1;
	}

	if ((shm = shmat(shmid, NULL, 0)) == (int *) -1) {
		perror("Error accessng shared memory");
		return 1;
	}

	s = shm;

	while(1) {
		sleep(1);
		int i;
		for(i = 0 ; i < 10 ; i++) {
			printf(":%d",*shm++);
		}
	}



	return 0;
}


int initialize() {
	
}

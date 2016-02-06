#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM 1024


int main() {

	int shmid;
	key_t key;
	int *shm, *s;

	puts("Server initialization");
	
	if ( fork() == 0 ) {
		//child

	} else {
		//parent

	}

	return 0;
}


int initialize() {
	
}

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
		if ( fork() == 0 ) {
			puts("3");
		} else {
			puts("2");
		}

		puts("BOTH 3 AND 2");
	} else {
		//parent
		puts("PARENT");
	}

	return 0;
}


int initialize() {
	
}

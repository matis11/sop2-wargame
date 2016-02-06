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
		// Handle clients
		
		if ( fork() == 0 ) {
			while(1) {
				// Listen Player1 actions
				// ---------------------------
				//  - Receive messages
				//  - Run functions
				//  - Send results
				//
			}
		} else {
			while(1) {
				// Listen Player2 actions
				// ---------------------------
				//  - Receive messages
				//  - Run functions
				//  - Send results
				//
			}
		}

	} else {
		// Server's logic
		// -------------------
		//  - Count units for both players
		//  - Calculate wars
		//

		puts("I'm server");
	}

	return 0;
}


int initialize() {
	
}

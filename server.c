#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM 1024
#define SHMID 1234


int main() {

	int shmid;
	key_t key;
	int *shm, *s;
	int *buf;

	//Memory creation && init

	shmid = shmget(SHMID, SHM, IPC_CREAT|0600); 
	if (shmid == -1){
		perror("Error1"); 
		exit(1);
	}

	buf = (int*)shmat(shmid, NULL, 0); 
	if (buf == NULL){
		perror("Add memory to mother");
       		exit(1);
	}

	puts("Server initialization");
	
	if ( fork() == 0 ) {
		// Handle clients
		
		if ( fork() == 0 ) {
			
			shmid = shmget(SHMID,SHM, 0);
			buf = shmat(shmid, NULL, 0);
			if(buf == NULL) {
				perror("Add memory to player1");
				exit(1);
			}

			while(1) {
				// Listen Player1 actions
				// ---------------------------
				//  - Receive messages
				//  - Run functions
				//  - Send results
				//
			}
			shmdt(shm);
		}
	       	else {
			shmid = shmget(SHMID,SHM, 0);
	                buf = shmat(shmid, NULL, 0);
			if(buf == NULL) {								                        perror("Add memory to player2");						 		exit(1);
	 	       	}

			while(1) {
				// Listen Player2 actions
				// ---------------------------
				//  - Receive messages
				//  - Run functions
				//  - Send results
				//
			}

			shmdt(buf);
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



void podnies(int semid, int semnum){
       buf.sem_num = semnum;
       buf.sem_op = 1;
                buf.sem_flg = 0;
	       	if (semop(semid, &buf, 1) == -1){
		       	perror("Podnoszenie semafora"); 
			exit(1);	
	   	}


void edit(int x) {
	// Set semaphore
	// Save to SHAREDMEMORY
	// Remove semaphore
}

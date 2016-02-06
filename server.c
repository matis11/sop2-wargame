#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM 1024
#define SHMID 1234
#define NUMBEROFMUTEX 1 // Amout of mutex in array
#define SEMCOLLECTIONID 45281


static struct sembuf sembuff;
int main() {

	int shmid;
        int semid;
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

    
    //Mutex init
   semid = semget(SEMCOLLECTIONID, NUMBEROFMUTEX, IPC_CREAT|0600);
    if (semid == -1){
        perror("Mutex error");
        exit(1);
    }
    ￼￼￼semctl(semid, 0, SETVAL, 1); //dajemy semaforowi wartosc 1
        
    
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
		if(buf == NULL) {
                	perror("Add memory to player2");
                	exit(1);
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

	//shmctl(shmid,IPC_RMID, buf);
	return 0;
}



void vergholen(int semid, int semnum, int v){ //id semafora, numer semafora w tablicy, wartosc o ile podnosimy
       sembuff.sem_num = semnum;
       sembuff.sem_op = v;
       sembuff.sem_flg = 0;
       if (semop(semid, &sembuff, 1) == -1){
            perror("V operation");
            exit(1);
       }
}

void proberen(int semid, int semnum, int p){
    sembuff.sem_num = semnum;
    sembuff.sem_op = -p;
    sembuff.sem_flg = 0;
    if (semop(semid, &sembuff, 1) == -1){
        perror("P operation");
        exit(1);
    }
}




void writeToMemory(int value, int position, int* memory,int semid) {
    proberen(semid, 0, 1);
    memory[position] = value;
    vergholen(semid, 0, 1);
}


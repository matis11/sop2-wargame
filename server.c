#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM 1024
#define SHMID 1234
#define NUMBEROFMUTEX 2 // Amout of mutex in array
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
    ￼￼￼	semctl(semid, 0, SETVAL, 1); //semafor numer 0 ma wartosc 1
        semctl(semid, 1, SETVAL, 10); //semafor numer 1 ma wartosc 10


	//Wydaej mi sie ze tutaj mozna stworzyc kolejki komunikatow a obsluga potem w odpowiednich forkach



     //--------------------------------------------------
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
			int i = 0;
			while(1) {
				i++;
				// Listen Player1 actions
				// ---------------------------
				//  - Receive messages
				//  - Run functions
				//  - Send results
				//
				writeToMemory(i,1,buf,semid);
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
				pri
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



void verhogen(int semid, int semnum, int v){ //id semafora, numer semafora w tablicy, wartosc o ile podnosimy
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


int readFromMemory(int position,int* memory,int semid) {
    proberen(semid, 1, 10);
    proberen(semid, 0 ,1);
    verhogen(semid, 1, 9);
    verhogen(semid, 1, 1);
    int readed = memory[position];    
    verhogen(semid, 0, 1);
    return readed;
}

void writeToMemory(int value, int position, int* memory,int semid) {
    proberen(semid, 1, 1);
    proberen(semid, 0, 10);
    memory[position] = value;
    verhogen(semid, 0, 10);
    verhogen(semid, 1, 1);
}


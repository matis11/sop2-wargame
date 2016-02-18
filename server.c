#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>


#define SHM 1024 //Size of memory in bytes
#define SHMID 1234
#define NUMBEROFMUTEX 2 // Amout of mutex in array
#define SEMCOLLECTIONID 45281


static struct sembuf sembuff;
// ------------------------ INFO -----------------------------------
// W pamieci na pozycji:
// 0 - zasoby gracza 1
// 8 - ilosc jed. light gracza 1
//16 - ilosc jed. heavy gracza 1
//24 - ilosc jed. kawal gracza 1
//32 - ilosc jed. robot gracza 1
//40- zasoby gracza 2
//48 - ilosc jed. light gracza 2
//56 - ilosc jed. heavy gracza 2
//64 - ilosc jed. kawal gracza 2
//72 - ilosc jed. robot gracza 2

//80 - pkt. zwyciestwa gracza 1
//88 - pkt. zwyciestwa gracza 2

#define RESOURCES_1 0
#define NUMLIGHT_1 8
#define NUMHEAVY_1 16
#define NUMCAVALRY_1 24
#define NUMWORKERS_1 32
#define RESOURCES_2 40
#define NUMLIGHT_2 48
#define NUMHEAVY_2 56
#define NUMCAVALRY_2 64
#define NUMWORKERS_2 72
#define VICTORYPOINTS_1 80
#define VICTORYPOINTS_2 88


//-------------------------- END -------------------------------------


//id semafora, numer semafora w tablicy, wartosc o ile podnosimy
void verhogen(int semid, int semnum, int v){
    sembuff.sem_num = semnum;
    sembuff.sem_op = v;
    sembuff.sem_flg = 0;
    if (semop(semid, &sembuff, 1) == -1){
        printf("V operation\n");
        exit(1);
    }
}

//id semafora, numer semafora, wartosc o ile opuszczamy(DODATNIA!)
void proberen(int semid, int semnum, int p){
    sembuff.sem_num = semnum;
    sembuff.sem_op = -p;
    sembuff.sem_flg = 0;
    if (semop(semid, &sembuff, 1) == -1){
        printf("P operation\n");
        exit(1);
    }
}


int readFromMemory(int position, int* memory,int semid) {
   // proberen(semid, 1, 10);
   // proberen(semid, 0 ,1);
   // verhogen(semid, 1, 9);
    //verhogen(semid, 1, 1);
    proberen(semid, 0, 1); //added
    printf("Reading\n");
    int readed = memory[position];
    verhogen(semid, 0, 1);
    return readed;
}

void writeToMemory(int value, int position, int* memory,int semid) {
   // proberen(semid, 1, 1);
   // proberen(semid, 0, 10);
    proberen(semid,0,1); //added
    printf("Writing\n");
    memory[position] = value;
    verhogen(semid,0,1); //added
   // verhogen(semid, 0, 10);
   // verhogen(semid, 1, 1);
}


void updateResoures(int* memory, int semid) {
    //Player 1
    int numbersOfWorkersPlayer1 = readFromMemory(NUMWORKERS_1,memory,semid);
    int currentResourecesPlayer1 = readFromMemory(RESOURCES_1,memory,semid);
    int newResources1 = numbersOfWorkersPlayer1 * 5 + 50 + currentResourecesPlayer1;
    writeToMemory(newResources1, RESOURCES_1, memory, semid);
    //Player 2
    int numbersOfWorkersPlayer2 = readFromMemory(NUMWORKERS_2,memory,semid);
    int currentResourecesPlayer2 = readFromMemory(RESOURCES_2,memory,semid);
    int newResources2 = numbersOfWorkersPlayer2 * 5 + 50 + currentResourecesPlayer2;
    writeToMemory(newResources2, RESOURCES_2, memory, semid);
}

//player - which player attack (0 or 1)
void attack(int player, int numOfLightAttack, int numOfHeavyAttack, int numOfCavarlyAttack, int *memory, int sid) {
    const float valueAttackLight = 1;
    const float valueAttackHeavy = 1.5;
    const float valueAttackCavarly = 3.5;
    const float valueDefenseLight = 1.2;
    const float valueDefenseHeavy = 3;
    const float valueDefenseCavarly = 1.2;
    
    //UWAGA Troche do dupy ale musiałem zrobic copy-paste w elsie i pozmieniac niektore rzeczy w zaleznosci od playera, inaczej cięzko to ogarnąć z tego względu ze musimy na stałe wpisywać pozycje gdzie znajduje sie w pamięci szukana wartość a ta jest rózna dla Player1 i Player2, ale kod dziala
    
    if (player == 0) {
        
        if(fork() == 0) {
            //status before attack
            int numOfLightP1 = readFromMemory(NUMLIGHT_1, memory, sid);
            int numOfHeavyP1 = readFromMemory(NUMHEAVY_1, memory, sid);
            int numOfCavarlyP1 = readFromMemory(NUMCAVALRY_1, memory, sid);
            int numOfLightP2 = readFromMemory(NUMLIGHT_2, memory, sid);
            int numOfHeavyP2 = readFromMemory(NUMHEAVY_2, memory, sid);
            int numOfCavarlyP2 = readFromMemory(NUMCAVALRY_2, memory, sid);
        
            //amount of soldier in base
            int restLightP1 = numOfLightP1 - numOfLightAttack;
            int restHeavyP1 = numOfHeavyP1 - numOfHeavyAttack;
            int restCavarlyP1 = numOfCavarlyP1 - numOfCavarlyAttack;
            
            if( restLightP1 < 0 || restHeavyP1 < 0 || restCavarlyP1 < 0 ) {
                printf("SERVER: Can't attack\n");
                return;
            }
            
            writeToMemory(restLightP1, NUMLIGHT_1, memory, sid);
            writeToMemory(restHeavyP1, NUMHEAVY_1, memory, sid);
            writeToMemory(restCavarlyP1, NUMCAVALRY_1, memory, sid);
            
            int sumAttackP1 = (numOfLightAttack * valueAttackLight) + (numOfHeavyAttack * valueAttackHeavy) + (numOfCavarlyAttack * valueAttackCavarly);
            int sumDefenseP2 = (numOfLightP2 * valueDefenseLight) + (numOfHeavyP2 * valueDefenseHeavy) + (numOfCavarlyP2 * valueDefenseCavarly);
            
            sleep(5);
            
            if(sumAttackP1 > sumDefenseP2) {
                
                printf("SERVER: Attack finished. Great victory of player1! All opponent's units are dead\n");
                writeToMemory(0, NUMLIGHT_2, memory, sid);
                writeToMemory(0, NUMHEAVY_2, memory, sid);
                writeToMemory(0, NUMCAVALRY_2, memory, sid);
                writeToMemory(restLightP1 + numOfLightAttack, NUMLIGHT_1, memory, sid);
                writeToMemory(restHeavyP1 + numOfHeavyAttack, NUMHEAVY_1, memory, sid);
                writeToMemory(restCavarlyP1 + numOfCavarlyAttack, NUMCAVALRY_1, memory, sid);
                
            }
            else {
                int lossLightP2 = numOfLightP2 * (sumAttackP1/sumDefenseP2);
                int lossHeavyP2 = numOfHeavyP2 * (sumAttackP1/sumDefenseP2);
                int lossCavarlyP2 = numOfCavarlyP2 * (sumAttackP1/sumDefenseP2);
                
                int lossLightP1 = numOfLightAttack * (sumAttackP1/sumDefenseP2);
                int lossHeavyP1 = numOfHeavyAttack * (sumAttackP1/sumDefenseP2);
                int lossCavarlyP1 = numOfCavarlyAttack * (sumAttackP1/sumDefenseP2);
                
                writeToMemory(numOfLightP2 - lossLightP2, NUMLIGHT_2, memory, sid);
                writeToMemory(numOfHeavyP2 - lossHeavyP2, NUMHEAVY_2, memory, sid);
                writeToMemory(numOfCavarlyP2 - lossCavarlyP2, NUMCAVALRY_2, memory, sid);
                writeToMemory(numOfLightP1 - lossLightP1, NUMLIGHT_1, memory, sid);
                writeToMemory(numOfHeavyP1 - lossHeavyP1, NUMHEAVY_1, memory, sid);
                writeToMemory(numOfCavarlyP1 - lossCavarlyP1, NUMCAVALRY_1, memory, sid);
                
                printf("SERVER: Attack finished.\nPlayer1 Status(Dead / Attack) : \n Light: %d / %d \n Heavy: %d / %d \n Cavarly: %d / %d \n \n Player2 Status(Dead / Defended) : \n Light: %d / %d \n Heavy: %d / %d \n Cavarly: %d / %d \n", lossLightP1, numOfLightAttack, lossHeavyP1, numOfHeavyAttack, lossCavarlyP1, numOfCavarlyAttack, lossLightP2, numOfLightP2, lossHeavyP2, numOfHeavyP2, lossCavarlyP2, numOfCavarlyP2);
                
            }
            
        }
        
        
    }
    else {
        
        if(fork() == 0) {
            
            //status before attack
            int numOfLightP1 = readFromMemory(NUMLIGHT_1, memory, sid);
            int numOfHeavyP1 = readFromMemory(NUMHEAVY_1, memory, sid);
            int numOfCavarlyP1 = readFromMemory(NUMCAVALRY_1, memory, sid);
            int numOfLightP2 = readFromMemory(NUMLIGHT_2, memory, sid);
            int numOfHeavyP2 = readFromMemory(NUMHEAVY_2, memory, sid);
            int numOfCavarlyP2 = readFromMemory(NUMCAVALRY_2, memory, sid);
            
            //amount of soldier in base
            int restLightP2 = numOfLightP2 - numOfLightAttack;
            int restHeavyP2 = numOfHeavyP2 - numOfHeavyAttack;
            int restCavarlyP2 = numOfCavarlyP2 - numOfCavarlyAttack;
            
            if( restLightP2 < 0 || restHeavyP2 < 0 || restCavarlyP2 < 0 ) {
                printf("SERVER: Can't attack\n");
                return;
            }
            
            writeToMemory(restLightP2, NUMLIGHT_2, memory, sid);
            writeToMemory(restHeavyP2, NUMHEAVY_2, memory, sid);
            writeToMemory(restCavarlyP2, NUMCAVALRY_2, memory, sid);
            
            int sumAttackP2 = (numOfLightAttack * valueAttackLight) + (numOfHeavyAttack * valueAttackHeavy) + (numOfCavarlyAttack * valueAttackCavarly);
            int sumDefenseP1 = (numOfLightP1 * valueDefenseLight) + (numOfHeavyP1 * valueDefenseHeavy) + (numOfCavarlyP1 * valueDefenseCavarly);
            
            sleep(5);
            
            if(sumAttackP2 > sumDefenseP1) {
                
                printf("SERVER: Attack finished. Great victory of player1! All opponent's units are dead\n");
                writeToMemory(0, NUMLIGHT_1, memory, sid);
                writeToMemory(0, NUMHEAVY_1, memory, sid);
                writeToMemory(0, NUMCAVALRY_1, memory, sid);
                writeToMemory(restLightP2 + numOfLightAttack, NUMLIGHT_2, memory, sid);
                writeToMemory(restHeavyP2 + numOfHeavyAttack, NUMHEAVY_2, memory, sid);
                writeToMemory(restCavarlyP2 + numOfCavarlyAttack, NUMCAVALRY_2, memory, sid);
                
            }
            else {
                int lossLightP1 = numOfLightP1 * (sumAttackP2/sumDefenseP1);
                int lossHeavyP1 = numOfHeavyP1 * (sumAttackP2/sumDefenseP1);
                int lossCavarlyP1 = numOfCavarlyP1 * (sumAttackP2/sumDefenseP1);
                
                int lossLightP2 = numOfLightAttack * (sumAttackP2/sumDefenseP1);
                int lossHeavyP2 = numOfHeavyAttack * (sumAttackP2/sumDefenseP1);
                int lossCavarlyP2 = numOfCavarlyAttack * (sumAttackP2/sumDefenseP1);
                
                writeToMemory(numOfLightP2 - lossLightP2, NUMLIGHT_2, memory, sid);
                writeToMemory(numOfHeavyP2 - lossHeavyP2, NUMHEAVY_2, memory, sid);
                writeToMemory(numOfCavarlyP2 - lossCavarlyP2, NUMCAVALRY_2, memory, sid);
                writeToMemory(numOfLightP1 - lossLightP1, NUMLIGHT_1, memory, sid);
                writeToMemory(numOfHeavyP1 - lossHeavyP1, NUMHEAVY_1, memory, sid);
                writeToMemory(numOfCavarlyP1 - lossCavarlyP1, NUMCAVALRY_1, memory, sid);
                
                printf("SERVER: Attack finished.\nPlayer1 Status(Dead / Attack) : \n Light: %d / %d \n Heavy: %d / %d \n Cavarly: %d / %d \n \n Player2 Status(Dead / Defended) : \n Light: %d / %d \n Heavy: %d / %d \n Cavarly: %d / %d \n", lossLightP1, numOfLightAttack, lossHeavyP1, numOfHeavyAttack, lossCavarlyP1, numOfCavarlyAttack, lossLightP2, numOfLightP2, lossHeavyP2, numOfHeavyP2, lossCavarlyP2, numOfCavarlyP2);
                
            }
            
        }
        
    }
}


int main() {

	int shmid;
    int semid;
	key_t key;
	int *shm, *s;
	int *buf;

	//Memory creation && init

	shmid = shmget(SHMID, SHM, IPC_CREAT|0600); 
	if (shmid == -1){
		printf("Error1\n");
		exit(1);
	}

	buf = (int*)shmat(shmid, NULL, 0); 
	if (buf == NULL){
		printf("Add memory to mother\n");
        exit(1);
	}

    
    //Mutex init
    semid = semget(SEMCOLLECTIONID, NUMBEROFMUTEX, IPC_CREAT|0600);
    if (semid == -1){
        	printf("Mutex error\n");
        	exit(1);
    }
    semctl(semid, 0, SETVAL, 1); //semafor numer 0 ma wartosc 1
    semctl(semid, 1, SETVAL, 10); //semafor numer 1 ma wartosc 10


	//Wydaej mi sie ze tutaj mozna stworzyc kolejki komunikatow a obsluga potem w odpowiednich forkach



     //--------------------------------------------------
	printf("Server initialization\n");
    
    writeToMemory(300, RESOURCES_1, buf, semid);
    writeToMemory(300, RESOURCES_2, buf, semid);
    
	
	if ( fork() == 0 ) {
		// Handle clients
		
		if ( fork() == 0 ) {
			
			shmid = shmget(SHMID,SHM, 0);
            buf = (int*)shmat(shmid, NULL, 0);
			if(buf == NULL) {
				printf("Add memory to player1\n");
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
                //printf("Write %d\n", i);
				//writeToMemory(i,1,buf,semid);
                //sleep(0.5);
                
                
                
			}
            shmdt(shm);
        }
        else {
            shmid = shmget(SHMID,SHM, 0);
	        buf = (int*)shmat(shmid, NULL, 0);
            if(buf == NULL) {
                	printf("Add memory to player2\n");
                	exit(1);
            }
            
			while(1) {
				// Listen Player2 actions
				// ---------------------------
				//  - Receive messages
				//  - Run functions
				//  - Send results
				//
                sleep(0.5);
                int a = readFromMemory(RESOURCES_1,buf,semid);
                printf("Read %d\n", a);
               

			}

			shmdt(buf);
		}

	} else {
		// Server's logic
		// -------------------
		//  - Count units for both players
		//  - Calculate wars
		//
        if (fork() == 0) {
            while (1) {
                //updating game
                updateResoures(buf,semid);
                sleep(1);
            }
        }
        
        
		printf("I'm server\n");
	}

	//shmctl(shmid,IPC_RMID, buf);
    
	return 0;
}



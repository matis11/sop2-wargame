#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

#define PLAYER1_KEY 5555
#define PLAYER2_KEY 6666

char player_name[255];
int player_id;

int main() {

	int shmid;
	key_t key;
	int *shm, *s;
	int str_length = 255;
	int msg_id;

	struct msg_info {
	    long mtype; //must be: 1
	    char info[255];
    } msg_info;

	struct msg_attack {
		long mtype; //must be: 2
		int forces[4];
		
	} msg_attack;

	struct msg_build {
		long mtype; //must be: 3
		int type;
		int quantity;
	} msg_build;

	struct msg_status {
		long mtype; //must be: 4
		unsigned long long int resources[5];
	} msg_info;

	int msg_info_size = sizeof(struct msg_info) - sizeof(long);
	int msg_attack_size= sizeof(struct msg_attack) - sizeof(long);
	int msg_build_size = sizeof(struct msg_build) - sizeof(long);
	int msg_status_size = sizeof(struct msg_status) - sizeof(long);

	system("clear");
	setbuf(stdout, NULL);
	printf("What's your name commander?\n");
	gets(player_name);
	system("clear");

	// Determine player nr
	while(1) {
		if ( msg_id = msgget((key_t) PLAYER1_KEY, 0666 | IPC_CREAT | IPC_EXCL) != -1) {
			player_id = 1;
			msg_info.mtype = 1;
			strcpy(msg_info.info, "hello");
			if (msgsnd(msg_id, &msg_info, msg_info_size, 0) != -1) {
				break;
			}
		} else if ( msg_id = msgget((key_t) PLAYER2_KEY, 0666 | IPC_CREAT | IPC_EXCL) != -1) {
			player_id = 2;
			msg_info.mtype = 1;
			strcpy(msg_info.info, "hello");
			if (msgsnd(msg_id, &msg_info, msg_info_size, 0) != -1) {
				break;
			}
		}
	}

	if (fork() == 0) {
		while(1) {
			// Get status from server using msg
			msgrcv(msg_id, msg_status, msg_status_size, 4, 0);
			display_status(msg_status.recources[0],
					msg_status.resources[1],
					msg_status.resources[2],
					msg_status.resources[3],
					msg_status.resources[4]);
		}
	} else {
		printf("\n\nWhat are your orders, sir?\n");
		while(1) {
			char order[255];
			scanf(" %s", &order);
			printf("PERFORMING %s sir!\n", order);
			
			if (strstr(order, "build") != NULL) {
				msg_build.mtype = 3;

				char temp_type[255];
				
				scanf("%s", &temp_type);
				scanf("%d", &msg_build.quantity);

				if (strstr(temp_type, "light") != NULL) {
					msg_build.type = 0;
				} else if (strstr(temp_type, "heavy") != NULL) {
					msg_build.type = 1;
				} else if (strstr(temp_type, "cavarly") != NULL) {
					msg_build.type = 2;
				} else if (strstr(temp_type, "workers") != NULL) {
					msg_build.type = 3;
				}

				printf("Building %d x %d\n", msg_build.type, msg_build.quantity);
				if (msgsnd(msg_id, &msg_build, msg_build_size, 0) == -1) {
					perror("Problem with sending build order");
					return 1;
				}
			} else if (strstr(order, "attack") != NULL) {
				msg_attack.mtype = 2;

				scanf("%d", &msg_attack.forces[0]);
				scanf("%d", &msg_attack.forces[1]);
				scanf("%d", &msg_attack.forces[2]);
				scanf("%d", &msg_attack.forces[3]);

				printf("TO VICTORY! With %d light, %d\ heavy, %d cavarly, %d workers \n", msg_attack.forces[0], msg_attack.forces[1], msg_attack.forces[2], msg_attack.forces[3]);
				if (msgsnd(msg_id, &msg_attack, msg_attack_size, 0) == -1) {
					perror("Problem with sending attack order");
					return 1;
				}
			} else {
				printf("Sorry, cannot do\n");
			}
		}
	}

	return 0;
}


int display_status(gold, light, heavy, cavalry, workers) {
	printf("\0337\033[H\033[K");
	printf("\033[030;47m #%d: %s \033[0m", player_id, player_name);
	printf("\033[030;43m GOLD: %llu$ \033[0m", gold);
	printf("\033[030;41m LIGHT: %llu \033[0m", light);
	printf("\033[030;42m HEAVY: %llu \033[0m", heavy);
	printf("\033[030;45m CAVALRY: %llu \033[0m", cavalry);
	printf("\033[030;46m WORKERS: %llu\033[0m", workers);
	printf("\0338");
}



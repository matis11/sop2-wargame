#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

char player_name[255];

int main() {

	int shmid;
	key_t key;
	int *shm, *s;
	int str_length = 255;

	system("clear");
	setbuf(stdout, NULL);
	printf("What's your name commander?\n");
	gets(player_name);
	system("clear");

	if (fork() == 0) {
		int i = 1;
		while(1) {
			display_status(i,i,i,i,i);
			sleep(1);
			i++;
		}
	} else {
		while(1) {
			printf("\n\nWhat are your orders, sir?\n");
			while(1) {
				char order[255];
				gets(order);
				printf("PERFORMING %s sir!\n", order);
			}
		}
	}

	return 0;
}


int display_status(gold, light, heavy, cavalry, workers) {
	printf("\0337\033[H\033[K");
	printf("\033[030;47m %s \033[0m", player_name);
	printf("\033[030;43m GOLD: %d$ \033[0m", gold);
	printf("\033[030;41m LIGHT: %d \033[0m", light);
	printf("\033[030;42m HEAVY: %d \033[0m", heavy);
	printf("\033[030;45m CAVALRY: %d \033[0m", cavalry);
	printf("\033[030;46m WORKERS: %d\033[0m", workers);
	printf("\0338");
}

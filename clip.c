#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define p1k 88188
#define p2k 99199

#define mainq 22138
#define p1q 16321
#define p2q 63699
int mid;
int session=1;
void display_stats(int r, int l, int h, int c, int w) {
	        printf("There is %d gold in your treasury\n",r);
	        if((l=0)&&(h>0)){
		printf("%d of heavy infantry are waiting in barracks for your orders\n",h); 
		}else if((l>0)&&(h=0)){
		printf("%d of light infantry are waiting in barracks for your orders\n",l);
		}else if((l==0)&&(h==0)){
		printf("You have no infantry\n");
		}else{
		printf("%d of light infantry and %d of heavy infantry are waiting in barracks for your orders\n",l,h);
		}
		if(c==0){
		printf("You have no cavalry\n");
		}else{
		printf(" %d cavalries are statione in stable\n",c);
		}
		if(w==0){
		printf("Your economy is weak, no one is working for you\n");
		}else{
	        printf("There are %d workers in your base.\n",w);
		}
}

void end(int signo){
msgctl(mid,IPC_RMID,0);
session=0;
exit(0);
}

int main() {
	int shmid;
	key_t key;
	int pid;
	int px;
	int mi;
	char* pname[20];

	struct minfo{
		long mtype; //1
		char info[199];
    	};

	struct mattack{
		long mtype; //2
		int units[4];	
	};

	struct mrecruit{
		long mtype; //3
		int type;
		int quantity;
	};

	struct mstats{
		long mtype; //4
		int resources[5];
	};

	struct msgt{
		long mtype; //5
		int type;
		int qid;
	};
	struct mattack attack;
	struct minfo inf;
	struct mstats stats;
	struct mrecruit recr;
	struct msgt st;
	while((mi = msgget(mainq, 0600)) == -1) {
	printf("Waiting for server\n");
		sleep(1);	
	}
	st.mtype=10;
	st.qid=1;

	system("clear");
	setbuf(stdout, NULL);
	printf("Welcome to SOP2-WARGAME\n");
	sleep(2);
	system("clear");
	
	

	msgrcv(mi,&st,sizeof(struct msgt)-sizeof(long),11,0);
	if(st.qid==1){
		px=0;
		mid = msgget(p1k,0666);
		pid=1;		
	}else if(st.qid==2){
		px=1;
		mid = msgget(p2k,0666);
		pid=2;
	}


	signal(SIGINT,end);
	if(fork()==0){
	if(fork()==0){
	struct minfo infy;
	while(session){
		if(msgrcv(mid,&infy,sizeof(struct minfo) - sizeof(long),100,IPC_NOWAIT) != -1){
			printf("%s\n",infy.info);
		}
	}}
		while(session) {
			// Get status from server using msg
			msgrcv(mid, &stats, sizeof(struct mstats) - sizeof(long), 4, 0);
			display_stats(stats.resources[0],stats.resources[1],stats.resources[2],stats.resources[3],stats.resources[4]);
		}
	} else {
		printf("\nWaiting for your command, my lord.\n");
		printf("Available commands: attack x y z, recrutation x y, quit\n");
		printf("To attack write command ans, as parameters, quantity of units of different types\n");
		printf("x is for light infantry, y is for heavy infantry, z is for cavalry\n");
		printf("After recrutation command type unit (id 0-3) and then quantity\n");
		printf("l - light infantry, h - heavy infantry, c - cavalry, w - worker\n");
		while(1) {
			char com[255];
			scanf(" %s", com);
			printf("PERFORMING %s sir!\n", com);
			
			if (strstr(com, "recrutation") != NULL) {
				recr.mtype = 3;
				char unit[1];
				scanf(" %s", unit);
				scanf(" %d", &recr.quantity);

				if (strcmp(unit, "l") != NULL) recr.type = 0;
				else if (strcmp(unit, "h") != NULL) recr.type = 1;
				else if (strcmp(unit, "c") != NULL) recr.type = 2;
				else if (strcmp(unit, "w") != NULL) recr.type = 3;
				
				printf("Recruiting %d of %s\n", recr.quantity, unit);
				if (msgsnd(mid, &recr, sizeof(struct mrecruit) - sizeof(long), 0) == -1) {
					perror("My lord, we have problem with recruitment of units.");
					return 1;
				}
			} else if (strcmp(com, "attack") != NULL) {
				attack.mtype = 2;
				scanf("%d", &attack.units[0]);
				scanf("%d", &attack.units[1]);
				scanf("%d", &attack.units[2]);
				printf("Attack with %d units of light infantry, %d units of heavy infantry and %d cavalriers\n", attack.units[0], attack.units[1], attack.units[2]);
				if (msgsnd(mid, &attack, sizeof(struct mattack) - sizeof(long), 0) == -1) {
					perror("My lord, we have problem with attack enemy!");
					return 1;
				}
			} else if (strcmp(com, "exit") != NULL) {
			inf.mtype=1;
			strcpy(inf.info,"exit");
			if(msgsnd(mid, &inf, sizeof(struct minfo) - sizeof(long), 0) == -1){
				perror("Problem with compliting exit order!");

			} else {
				printf("Sorry, my lord, your order isn't clear.\n");
			}
		}
	}

	return 0;
	}}

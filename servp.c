#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#define r1 0  //surowce p1
#define l1 4  //lekka p1
#define h1 8  //ciężka p1
#define c1 12 //kaw p1
#define w1 16 //robotnicy p1
#define v1 20 //punkty zwycięstwa p1
#define r2 24 //---||---p2
#define l2 28 //---||---p2
#define h2 32 //---||---p2
#define c2 36 //---||---p2
#define w2 40 //---||---p2
#define v2 44 //---||---p2
#define p1s 55 //semafory, well...
#define p2s 66
int shmid;
int semid;
int mainq;
int p1q;
int p2q;
int session=1;
int *buf;
static struct sembuf sb;

//struktury wiadomości

struct mattack {
	long mtype;
	int units[3];
};
struct mrecruit {
	long mtype;
	int type;
	int quantity;
};
struct mstats {
	long mtype;
	int resources[5];
};
struct minfo {
	long mtype;
	char info[100];
};
struct msgt {
	long mtype;
	int q;
};

//Write/Read do pamięci z semaforami
void writer(int data, int pos, int *memo, int semid){
	sb.sem_num = pos/4;	//P sem
	sb.sem_op = -50;
	sb.sem_flg=0;
		memo[pos]=data; //zapis
	sb.sem_num = pos/4;
	sb.sem_op = 50;
	sb.sem_flg = 0;
}

int reader(int pos, int* memo, int semid){
int x;
	sb.sem_num = pos/4;	//P sem
	sb.sem_op = -1;
	sb.sem_flg=0;
		x=memo[pos];	//czytanie
	sb.sem_num = pos/4;	//V sem
	sb.sem_op = 1;
	sb.sem_flg = 0;
return x;			//odczyt
}

//Attack
void attack(int *mem, int l, int h, int c, int pid, int semid){
float att[3]={1,1.5,3.5};
float deff[3]={1.2,3,1.2};
struct minfo attinf;
attinf.mtype=132;
if((l <= 0) && (h <= 0) && (c <= 0)){
	strcpy(attinf.info,"Problem with sending attack.\n");
	if(pid==1) msgsnd(p1q, &attinf, sizeof(struct minfo) - sizeof(long), 0);
	else msgsnd(p2q,&attinf, sizeof(struct minfo) - sizeof(long), 0);
return;
}
if (pid==1){
	int tl = reader(l1,mem,semid) - l;
	int th = reader(h1,mem,semid) - h;
	int tc = reader(c1,mem,semid) - c;
	if((tl < 0) || (th < 0) || (tc < 0)){
		strcpy(attinf.info,"You selected too many units!\n");
		msgsnd(p1q,&attinf,sizeof(struct minfo) - sizeof(long),0);
		return;
	}
	int dl = reader(l2,mem,semid);
	int dh = reader(h2,mem,semid);
	int dc = reader(c2,mem,semid);
	float p1a = l*att[0] + h*att[1] + c*att[2];
	float p1d = l*deff[0] + h*deff[1] + c*deff[2];
	float p2a = dl*att[0] + dh*att[1] + dc*att[2];
	float p2d = dl*deff[0] + dh*deff[1] + dc*deff[2];
	sleep(5);
	if(p1a>p2d){
		writer(0,l2,mem,semid);
		writer(0,h2,mem,semid);
		writer(0,c2,mem,semid);
		l-=l*p2a/p1d;
		h-=h*p2a/p1d;
		c-=c*p2a/p1d;
		writer(l,l1,mem,semid);
		writer(h,h1,mem,semid);
		writer(c,c1,mem,semid);
		strcpy(attinf.info,"P1 won the battle\n");
		msgsnd(p1q,&attinf,sizeof(struct minfo) - sizeof(long),0);
		strcpy(attinf.info,"P2 lost the battle\n");
		msgsnd(p2q,&attinf,sizeof(struct minfo) - sizeof(long),0);
		int vp = reader(v1,mem,semid);
		if(vp==4){
			strcpy(attinf.info,"P1 won the game!\n");
			msgsnd(p1q,&attinf,sizeof(struct minfo) - sizeof(long),0);
			strcpy(attinf.info,"P2 lost! \n");
			msgsnd(p2q,&attinf,sizeof(struct minfo) - sizeof(long),0);
			shmctl(shmid,IPC_RMID,NULL);
			kill(0,SIGINT);
		}else{
			vp++;
			writer(vp,v1,mem,semid);
		}
	}else{
		dl = dl-dl*p1a/p2d;
		dh = dh-dh*p1a/p2d;
		dc = dc-dc*p1a/p2d;
		l = l-l*p2a/p1d;
		h = h-h*p2a/p1d;
		c = c-c*p2a/p1d;	
		writer(l,l1,mem,semid);
		writer(h,h1,mem,semid);
		writer(c,c1,mem,semid);
		writer(dl,l2,mem,semid);
		writer(dh,h2,mem,semid);
		writer(dc,c2,mem,semid);
		strcpy(attinf.info,"No one won this battle\n");
		msgsnd(p1q,&attinf,sizeof(struct minfo) - sizeof(long),0);
		msgsnd(p2q,&attinf,sizeof(struct minfo) - sizeof(long),0);
	}
}else{
	int tl = reader(l2,mem,semid) - l;
	int th = reader(h2,mem,semid) - h;
	int tc = reader(c2,mem,semid) - c;
	if((tl < 0)||(th < 0)||(tc < 0)){
		strcpy(attinf.info,"You selected too many units!\n");
		msgsnd(p2q,&attinf,sizeof(struct minfo) - sizeof(long),0);
		return;
	}
	int dl = reader(l1,mem,semid);
	int dh = reader(h1,mem,semid);
	int dc = reader(c1,mem,semid);
	float p1a = dl*att[0] + dh*att[1] + dc*att[2];
	float p1d = dl*deff[0] + dh*deff[1] + dc*deff[2];
	float p2a = l*att[0] + h*att[1] + c*att[2];
	float p2d = l*deff[0] + h*deff[1] + c*deff[2];
	sleep(5);
	if(p2a>p1d){
		writer(0,l1,mem,semid);
		writer(0,h1,mem,semid);
		writer(0,c1,mem,semid);
		l-=l*p1a/p2d;
		h-=h*p1a/p2d;
		c-=c*p1a/p2d;
		writer(l,l2,mem,semid);
		writer(h,h2,mem,semid);
		writer(c,c2,mem,semid);
		strcpy(attinf.info,"P2 won the battle\n");
		msgsnd(p2q,&attinf,sizeof(struct minfo) - sizeof(long),0);
		strcpy(attinf.info,"P1 lost the battle\n");
		msgsnd(p1q,&attinf,sizeof(struct minfo) - sizeof(long),0);
		int vp = reader(v2,mem,semid);
		if(vp==4){
			strcpy(attinf.info,"P2 won the game!\n");
			msgsnd(p2q,&attinf,sizeof(struct minfo) - sizeof(long),0);
			strcpy(attinf.info,"You lost!\n");
			msgsnd(p1q,&attinf,sizeof(struct minfo) - sizeof(long),0);
			shmctl(shmid,IPC_RMID,NULL);
			kill(0,SIGINT);
		}else{
			vp++;
			writer(vp,v2,mem,semid);
		}
	}else{
		dl = dl-dl*p2a/p1d;
        	dh = dh-dh*p2a/p1d;
		dc = dc-dc*p2a/p1d;
		l = l-l*p1a/p2d;
		h = h-h*p1a/p2d;
		c = c-c*p1a/p2d;
		writer(l,l2,mem,semid);
		writer(h,h2,mem,semid);
		writer(c,c2,mem,semid);
		writer(dl,l1,mem,semid);
		writer(dh,h1,mem,semid);
		writer(dc,c1,mem,semid);
		strcpy(attinf.info,"No one won this battle\n");
		msgsnd(p1q,&attinf,sizeof(struct minfo) - sizeof(long),0);
		msgsnd(p2q,&attinf,sizeof(struct minfo) - sizeof(long),0);
	}	
}}

//Recruitment
void recruit(int *mem, int semid, int type, int quant, int pid){
//koszty, czas, struct informacyjny
struct minfo recinfo;
recinfo.mtype=123;
int time[4]={2,3,5,2};
int cost[4]={100,250,550,150};
int x;
//sprawdzenie aktualnych zasobów
if(pid==1){
	x = read(r1,mem,semid);
	if(type>4){
		strcpy(recinfo.info,"Bad command\n");
		msgsnd(p1q,&recinfo,sizeof(struct minfo) - sizeof(long),0);
	}
}else if(pid==2){
	x = read(r2,mem,semid);
	if(type>4){
		strcpy(recinfo.info,"Bad value of type\n");
		msgsnd(p2q,&recinfo,sizeof(struct minfo) - sizeof(long),0);
	}
	}
int y = quant*cost[type];
if(y<x){
	strcpy(recinfo.info,"Not enough resources!");
		if(pid==1){
			msgsnd(p1q,&recinfo,sizeof(struct minfo) - sizeof(long),0);
		}else if (pid==2){
			msgsnd(p2q,&recinfo,sizeof(struct minfo) - sizeof(long),0);
		}
	return;
}else{
	if(pid==1) writer(x-y,r1,mem,semid);
	else if(pid==2) writer (x-y,r2,mem,semid);

	if(fork()==0){
		if(pid==1){
			sb.sem_num=p1s;
			sb.sem_op=-1;
			sb.sem_flg=0;
		}else if (pid==2){
			sb.sem_num=p2s;
			sb.sem_op=-1;
			sb.sem_flg;
		}
		int x;
		for(x=0;x<quant;x++){
		sleep(time[type]);
		if(pid==1){
			if(type==0){
			int x=reader(l1,mem,semid);
			x++;
			writer(x,l1,mem,semid);
			}else if(type==1){
			int x=reader(h1,mem,semid);
			x++;
			writer(x,h1,mem,semid);
			}else if(type==2){
			int x=reader(c1,mem,semid);
			x++;
			writer(x,c1,mem,semid);
			}else if(type==3){
			int x=reader(w1,mem,semid);
			x++;
			writer(x,w1,mem,semid);
			}
		}else if(pid==2){
			if(type==0){
			int x=reader(l2,mem,semid);
			x++;
			writer(x,l2,mem,semid);
			}else if(type==1){
			int x=reader(h2,mem,semid);
			x++;
			writer(x,h2,mem,semid);
			}else if(type==2){
			int x=reader(c2,mem,semid);
			x++;
			writer(x,c2,mem,semid);
			}else if(type==3){
			int x=reader(w2,mem,semid);
			x++;
			writer(x,w2,mem,semid);
			}}
		}
		if(pid==1){
			sb.sem_num=p1s;
			sb.sem_op=1;
			sb.sem_flg=0;
		}else if(pid==2){
			sb.sem_num=p2s;
			sb.sem_op=1;
			sb.sem_flg=0;
		}}	
		exit(0);
}
}

//Status
void status(int *mem, int semid){
	int temp1=reader(w1,mem,semid);//p1
	int temp2=reader(r1,mem,semid);
	writer(temp2+5*(temp1+10),r1,mem,semid);
	temp1=reader(w2,mem,semid);//p2
	temp2=reader(r2,mem,semid);
	writer(temp2+5*(temp1+10),r2,mem,semid);
}

//close
void end(int signo){
session = 0;
msgctl(mainq,IPC_RMID,0);
msgctl(p1q,IPC_RMID,0);
msgctl(p2q,IPC_RMID,0);
}





int main(){
	int *shm = NULL;
	shmid = shmget(5000,512,IPC_CREAT|0600);
	buf = (int*)shmat(shmid,NULL,0);
	semid = semget(1111,14,IPC_CREAT|0600);
	int p;
	for(p=0;p<12;p++) semctl(semid,p,SETVAL,100);
	semctl(semid,55,SETVAL,1);//mutex dla p1
	semctl(semid,66,SETVAL,1);//mutex dla p2
	signal(SIGINT,end);	
	mainq = msgget(8888,IPC_CREAT|IPC_EXCL|0600);
	int count=0;
	struct msgt ps,r;
	struct minfo i;
	
	while(count<2){  //podłączenie klientów
		msgrcv(mainq,&r,sizeof(struct msgt)-sizeof(long),10,0);
		count++;
		if(count==1){
			p1q = msgget(11111,IPC_CREAT|0600);
		}else{
			p2q = msgget(22222,IPC_CREAT|0600);
		}
		ps.q=count;
		ps.mtype=19;
	}
	//start
	writer(300,r1,buf,semid);
	writer(300,r2,buf,semid);
	if(fork()==0){
		if(fork()==0){
		shmid=shmget(5000,512,0);
		struct minfo inf;
		struct mrecruit rec;
		struct mattack at;
		while(session){//odbiór wiadomości od klienta
			if(msgrcv(p1q,&inf,sizeof(struct minfo)-sizeof(long),1,IPC_NOWAIT)!=-1){
				if(strcmp(inf.info,"exit")){
				session=0;
				msgctl(p1q,IPC_RMID,0);
				msgctl(p2q,IPC_RMID,0);
				shmctl(shmid,IPC_RMID,NULL);
				exit(0);			
				}						
			}
			if(msgrcv(p1q,&rec,sizeof(struct mrecruit)-sizeof(long),2,IPC_NOWAIT!=-1)){
				recruit(buf,semid,rec.type,rec.quantity,1);		
			}
			if(msgrcv(p1q,&at,sizeof(struct mattack)-sizeof(long),3,IPC_NOWAIT!=-1)){
				attack(buf,at.units[0],at.units[1],at.units[2],1,semid);
			}}
			shmdt(shm);	
		}else{

			if(fork()==0){
				struct mstats s1;
				struct mstats s2;
				struct msgt s;
				struct minfo inf;
				struct minfo inf2;
				while(session){
					inf2.mtype=100;
					if((msgget(11111,0666)== -1) || (msgget(22222,0666)== -1)){
						printf("CONNECTION LOST\n");
						kill(0,SIGINT);
					}
					if(msgrcv(mainq,&s,sizeof(struct msgt)-sizeof(long),10,IPC_NOWAIT)==-1){
						printf("Server is full.\n");
					}
				status(buf,semid);
				sleep(1);
				s1.resources[0]=reader(r1,buf,semid);
				s1.resources[1]=reader(l1,buf,semid);
				s1.resources[2]=reader(h1,buf,semid);
				s1.resources[3]=reader(c1,buf,semid);
				s1.resources[4]=reader(v1,buf,semid);
				s1.mtype=7;
				s2.resources[0]=reader(r2,buf,semid);
				s2.resources[1]=reader(l2,buf,semid);
				s2.resources[2]=reader(h2,buf,semid);
				s2.resources[3]=reader(c2,buf,semid);
				s2.resources[4]=reader(v2,buf,semid);
				s2.mtype=7;
				}}}}
			else{
			//p2
			shmid=shmget(5000,512,0);
			struct minfo inf;
			struct mrecruit rec;
			struct mattack at;
			while(session){//odbiór wiadomości od klienta
			        if(msgrcv(p1q,&inf,sizeof(struct minfo)-sizeof(long),1,IPC_NOWAIT)!=-1){
				if(strcmp(inf.info,"exit")){
				    session=0;
				    msgctl(p1q,IPC_RMID,0);
				    msgctl(p2q,IPC_RMID,0);
				    shmctl(shmid,IPC_RMID,NULL);
				    exit(0);
				}}
				if(msgrcv(p1q,&rec,sizeof(struct mrecruit)-sizeof(long),2,IPC_NOWAIT!=-1)){
				       	recruit(buf,semid,rec.type,rec.quantity,2);
			        }
			        if(msgrcv(p1q,&at,sizeof(struct mattack)-sizeof(long),3,IPC_NOWAIT!=-1)){
				        attack(buf,at.units[0],at.units[1],at.units[2],2,semid);
				}}
		        shmdt(shm);
			}
return 0;
}






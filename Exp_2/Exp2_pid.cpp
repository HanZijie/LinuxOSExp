#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>

pid_t p1;
pid_t p2;

static int sem_id;
void P(int semid,int index)
{
    struct sembuf sem;	
    sem.sem_num = index;
    sem.sem_op = -1;	
    sem.sem_flg = 0; //操作标记：0或IPC_NOWAIT等
    semop(semid,&sem,1);	//1:表示执行命令的个数	
    return;
}

void V(int semid,int index)
{	
    struct sembuf sem;	
    sem.sem_num = index;
    sem.sem_op =  1;
    sem.sem_flg = 0;	
    semop(semid,&sem,1);	
    return;
}

static int shMemoryId;
typedef struct shareMemory{
    int sum;
}ShareMemory;

int main(int argc, char const *argv[])
{
    sem_id = semget((key_t)1,2,0666 | IPC_CREAT);
    shMemoryId = shmget((key_t)2,sizeof(ShareMemory),0666 | IPC_CREAT);
    ShareMemory* m = (ShareMemory *)shmat(shMemoryId,NULL,0);
    semctl(sem_id,0,SETVAL,0);
    semctl(sem_id,1,SETVAL,1);

    p1 = fork();
    if(p1 == 0){
        int i = 1;
        for(i = 1;i <= 100;i++){
            P(sem_id,1);
            printf("Process 1 is adding\n");
            m->sum += i;
            V(sem_id,0);
        }
        exit(0);
    }else{
        p2 = fork();
        if(p2 == 0){
            int a = 1;
            for (a = 0; a < 100;a++){
                P(sem_id,0);
                printf("Process 2 is printing:\n%d\n",m->sum);
                V(sem_id,1);
            }
            exit(0);
        }else{
            int *addr;
            wait(addr);
            wait(addr);
            semctl(sem_id,1,IPC_RMID);
            return 0;
        }
    }
    return 0;
}

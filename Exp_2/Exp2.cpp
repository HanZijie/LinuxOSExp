#include <sys/sem.h>
#include <sys/types.h>
#include <pthread/pthread.h>
#include <stdio.h>

static int sem_id = 0;

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

static int sum = 0;

void* subp1(void *a){
    int i = 1;
    for(i = 1;i <= 100;i++){
        P(sem_id,1);
        printf("Thread 1 is adding\n");
        sum += i;
        V(sem_id,0);
    }
}

void* subp2(void *b){
    int a = 1;
    for (a = 0; a < 100;a++){
        P(sem_id,0);
        printf("Thread 2 is printing:\n%d\n",sum);
        V(sem_id,1);
    }
}

int main(int argc, char const *argv[])
{
    sem_id = semget((key_t) 1234, 2, 0666 | IPC_CREAT);
    pthread_t t1,t2;

    union semun s1,s2;
    s1.val = 0;
    s2.val = 1;
    semctl(sem_id,0,SETVAL,s1);
    semctl(sem_id,1,SETVAL,s2);

    pthread_create(&t1,NULL,subp1,NULL);
    pthread_create(&t2,NULL,subp2,NULL);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    semctl(sem_id,1,IPC_RMID);

    return 0;
}

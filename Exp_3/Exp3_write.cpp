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

static int shMemoryId;
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

int main(int argc, char const *argv[])
{
    shMemoryId = shmget((key_t)2,0,0666 | IPC_CREAT);
    char* m = (char *)shmat(shMemoryId,NULL,0);
    int sem_id = semget((key_t)1,2,0666 | IPC_CREAT);
    int a = 0;
    FILE* fp = fopen("copy","w+");
    fclose(fp);
    fp = fopen("copy","ab");
    while (1){
        P(sem_id,0);
        a = a%30;
        if(m[30] == a){
            printf("copy is done %x n %x\n",m[30],a);
            fclose(fp);
            V(sem_id,1);
            break;
        }
        //printf("write: %x and %d\n",m[a],m[30]);
        fwrite((m+a), sizeof(char) , 1,fp);
        a++;
        V(sem_id,1);
    }
    return 0;
}
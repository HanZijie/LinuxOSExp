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
    int i = 0;
    FILE* fp = fopen("source","rb");
    m[30] = 31;
    while(1){
        P(sem_id,1);
        i = i%30;
        m[i] = (char)fgetc(fp);
        if(feof(fp)){
            printf("read is done\n");
            m[30] = i;
            fclose(fp);
            V(sem_id,0);
            break;
        }
        //printf("%c",m[i]);
        i++;
        V(sem_id,0);
    }
    return 0;
}

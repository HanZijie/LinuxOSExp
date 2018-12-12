//
//  main.cpp
//  OSExp
//
//  Created by 韩字杰 on 2018/12/12.
//  Copyright © 2018年 韩字杰. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

using std::string;

pid_t p1;
pid_t p2;
int pipefd[2];

void sigHandle(int sig_no){
    kill(p1,SIGUSR1);
    kill(p2,SIGUSR2);
}
void p1_exit(int sig_no){
    std::cout << "Child 1 is killed py parent\n";
    close(pipefd[0]);
    close(pipefd[1]);
    exit(0);
}
void p2_exit(int sig_no){
    std::cout << "Child 2 is killed py parent\n";
    close(pipefd[0]);
    close(pipefd[1]);
    exit(0);
}

int main(int argc, char const *argv[])
{
    pipe(pipefd);
    std::cout << getpid();
    signal(SIGINT,sigHandle);
    p1 = fork();
    int *addr;
    if(p1 == 0){
        signal(SIGINT,SIG_IGN);
        signal(SIGUSR1,p1_exit);
        int i = 1;
        while(1){
            char str1[30];
            sprintf(str1,"I send you %d times\n",i);
            write(pipefd[1],str1,sizeof(str1));
            i++;
            sleep(1);
        }
    }
    p2 = fork();
    if(p2 == 0){
        signal(SIGINT,SIG_IGN);
        signal(SIGUSR2,p2_exit);
        
        while(1){
            sleep(1);
            char str_[30];
            read(pipefd[0],str_,30);
            std::cout << "Receive:";
            std::cout << str_ << std::endl;
        }
    }else{
        wait(addr);
        wait(addr);
        close(pipefd[0]);
        close(pipefd[1]);
        std::cout << "Parent is killed\n";
    }
    
    return 0;
}

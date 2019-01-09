#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define PRINT_FONT_BLA  printf("\033[30m"); //黑色
#define PRINT_FONT_RED  printf("\033[31m"); //红色
#define PRINT_FONT_GRE  printf("\033[32m"); //绿色
#define PRINT_FONT_YEL  printf("\033[33m"); //黄色
#define PRINT_FONT_BLU  printf("\033[34m"); //蓝色
#define PRINT_FONT_PUR  printf("\033[35m"); //紫色
#define PRINT_FONT_CYA  printf("\033[36m"); //青色
#define PRINT_FONT_WHI  printf("\033[37m"); //白色

void printPermission(mode_t mode){
    PRINT_FONT_YEL
    mode_t ruler = 0x100;
    mode_t tmp;
    int i;
    //文件类型
    if (S_ISLNK(mode))
        putchar('l');
    else if (S_ISDIR(mode))
        putchar('d');
    else if (S_ISCHR(mode))
        putchar('c');
    else if (S_ISBLK(mode))
        putchar('b');
    else if (S_ISSOCK(mode))
        putchar('s');
    else if (S_ISFIFO(mode))
        putchar('p');
    else if (S_ISREG(mode))
        putchar('-');
    //用户权限
    for (i = 1; i <= 9; i++, ruler >>= 1)
    {
        if((i-1)%3 == 0 && i != 1){
            printf(" ");
        }
        tmp = ruler & mode;
        if (tmp == 0)
            putchar('-');
        else
        {
            switch (i % 3)
            {
            case 0:
                putchar('x');
                break;
            case 1:
                putchar('r');
                break;
            case 2:
                putchar('w');
                break;
            }
        }
    }
}
void myls(struct stat statbuf, struct dirent *entry, long long *total)
{
    struct passwd *psw;
    struct group *grp;
    struct tm currtime, mtime;
    long secsince1970 = time(NULL);
    psw = getpwuid(statbuf.st_uid);
    grp = getgrgid(statbuf.st_gid);
    localtime_r(&(statbuf.st_mtime), &mtime);
    localtime_r(&secsince1970, &currtime);
    printPermission(statbuf.st_mode);
    PRINT_FONT_CYA
    printf(" %hu ", statbuf.st_nlink);
    printf("%s ", psw->pw_name);
    printf("%s ", grp->gr_name);
    printf("%9lu ", statbuf.st_size);
    printf("%2d月%2d日 ", mtime.tm_mon + 1, mtime.tm_mday);
    if (mtime.tm_year == currtime.tm_year)
        printf("%2d:%2d ", mtime.tm_hour, mtime.tm_min);
    else
        printf("%5d ", mtime.tm_year + 1900);
    PRINT_FONT_GRE
    printf("%s\n", entry->d_name);
    *total += statbuf.st_size;
}
void mytab(int tab){
    PRINT_FONT_GRE
    int i = 1;
    for (; i < tab; i++){
        putchar('.');
    }
    printf("|");
}
void printdir(char *dir, int depth, long long *total)
{
    DIR *dp;
    char father[strlen(dir)];
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL)
    {
        printf("failing to opendir %s\n", dir);
        return;
    }
    chdir(dir);
    while (entry = readdir(dp))
    {
        lstat(entry->d_name, &statbuf);
        if (0 == strcmp("..", entry->d_name) || 0 == strcmp(".", entry->d_name))
            continue;
        mytab(depth);
        myls(statbuf, entry, total);
        if (S_ISDIR(statbuf.st_mode))
            printdir(entry->d_name, depth + 4, total); 
    }
    chdir("..");
    closedir(dp);
}
int depth = 0;
int main(int argc, char *argv[])
{
    long long total = 0;
    printdir(getcwd(NULL, NULL), depth, &total);
    printf("total %lld\n", total);
    return 0;
}

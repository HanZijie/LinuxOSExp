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

int main(int argc, char const *argv[])
{
    struct tm currtime;
    long secsince1970 = time(NULL);
    localtime_r(&secsince1970, &currtime);
    printf(";33m HELLO_WORLD%d \033[0m\n",currtime.tm_year);
    return 0;
}

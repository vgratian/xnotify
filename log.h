#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int logmsg(const char*, int, char*, char*);
int logrotate(const char*, struct tm*);
struct tm *gettm();
char *getpname(pid_t);

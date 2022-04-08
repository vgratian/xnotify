
#include "log.h"

// get process name by PID
char *getpname(pid_t pid) {

    char *pname;
    char fp[128];
    FILE *f;
    int i;
    char c;

    if ( ! (pname = malloc(128 * sizeof(char))) )
        return NULL;

    sprintf(fp, "/proc/%d/comm", pid);

    if ( ! (f = fopen(fp, "r")) ) {
        free(pname);
        return NULL;
    }

    i = 0;
    while (1) {
        c = fgetc(f);
        if ( c == EOF || c == '\n' || i == 127 )
            break;
        pname[i]=c;
        i++;
    }
    pname[i]='\0';

    fread(pname, 128, 1, f);
    fclose(f);
    return pname;
}

// rotate existing log file
// new file name is the same as the original + timestamp (date+hour)
// e.g. 'a.log' -> 'a.log.2022040723
int logrotate(const char *fp, struct tm *t) {
    char newfp[128];
    sprintf(newfp, "%s.%d%d%d%d", fp, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour+1);
    
    if ( rename(fp, newfp) != 0 ) {
        perror("rename");
        return 1;
    }
    return 0;
}

struct tm *gettm() {
    time_t t;
    time(&t);
    return localtime(&t);
}

int logmsg(const char *fp, int maxsize, char *sbj, char *msg) {

    FILE *f;
    struct stat st;
    struct tm *t = gettm();
    pid_t ppid = getppid();
    char *pname = getpname(ppid);

    if ( stat(fp, &st) == 0 && st.st_size >= maxsize ) {
        if ( logrotate(fp, t) != 0 )
            return 1;
    }

    if ( ! (f=fopen(fp, "a")) ) {
        return 1;
    }

    char date[128];
    // strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr)
    strftime(date, 128, "%Y-%m-%d %H:%M:%S", t);

    fprintf(f, "%s [%d:%s] (%s) %s\n", date, ppid,
        (pname) ? pname : "-", 
        (sbj) ? sbj : "-", msg);
    fclose(f);

    free(pname);

    return 0;
}

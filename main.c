#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "xwindow.h"

#ifdef LOGFN
#include "log.h"
#define LOGFP HOMEDIR "/" LOGFN
#endif

#define MUTEDFP HOMEDIR "/.muted" 

#define VERSION "0.0.1"
#define USAGE "Usage: xnotify [-dmusvh] [SUBJECT] MESSAGE\n"
#define ABOUT "Xnotify - create a notification pop-up window\n"

void usage() {
    printf(USAGE);
}

void version() {
    printf(VERSION);
    printf("\n");
}

void help() {
    printf(ABOUT);
    printf("\n");
    printf(USAGE);
    printf("\n");
    printf("Options:\n");
    printf("  -d   debug mode: run in foreground (default is\n");
    printf("       to fork and run in background)\n");
    printf("  -m   mute and exit (suppress notifications, but\n");
    printf("       allow logging)\n");
    printf("  -u   unmute and exit\n");
    printf("  -s   print state (muted or unmuted) and exit\n");
    printf("  -v   print version and exit\n");
    printf("  -h   print help and exit\n");
    printf("\n");
    printf("Examples:\n");
    printf("  xnotify \"new email!\"\n");
    printf("  xnotify \"jo@jabbr.jj\" \"hey was up?\"\n");
    printf("  xnotify \"acpid\" \"battery critically low: %%2\"\n");
    printf("\n");
}

// number of pending notifications (i.e. currently on the screen)
int pending() {
    FILE *fd;
    char buf[64];
    int count = 0;

    fd = popen("pgrep -c xnotify", "r");
    if (fd == NULL) {
        fprintf(stderr, "running pgrep command failed\n");
        return 0;
    }


    if (fgets(buf, 64, fd) != NULL) {
        count = atoi(buf);
        if (count != 0) // discard myself
            count--;
    }

    pclose(fd);
    return count;
}

int mute() {
    FILE *fd;
    if ( (fd = fopen(MUTEDFP, "w")) ) {
        fclose(fd);
        return 0;
    }
    return 1;
}

int unmute() {
    return remove(MUTEDFP);
}

// returns 1 if muted, otherwise 0
int muted() {
    FILE *fd;
    if ( (fd = fopen(MUTEDFP, "r")) ) {
        fclose(fd);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {

    char *subject, *message;
	int i=1, num, debug=0;

    // expect min 2 arguments
    if ( argc < 2 ) {
        usage();
        return 1;
    }

    // parse option
    if ( argv[1][0] == '-' ) {
        switch(argv[1][1]) {
            case 'h':
                help();
                return 0;
            case 'v':
                version();
                return 0;
            case 'd':
                debug=1;
                break;
            case 's':
                printf("%s\n", muted() ? "muted" : "unmuted");
                return 0;
            case 'm':
                return mute();
            case 'u':
                return unmute();
            default:
                printf("invalid option '%s'\n", argv[1]);
                printf("run '%s -h' for help\n", argv[0]);
                return 1;
        }
        i+=1;
    }

    // parse notification message and (optional) subject
    if ( (argc - i) == 2 ) {
        subject = argv[i];
        message = argv[i+1];
    } else if ( (argc - i) == 1 ) {
        subject = NULL;
        message = argv[i];
    } else {
        printf("Missing 'message' or incorrect number of arguments\n");
        return 1;
    }

    #ifdef LOGFP
    logmsg(LOGFP, LOGMAXS, subject, message);
    #endif

    if ( muted() )
        return 0;

    num = pending();

    if ( ! debug ) {
        if ( fork() )
            return 0;
		setsid();
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
    }

    return run(subject, message, num, getpid());
}

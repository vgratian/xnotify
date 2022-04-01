#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xwindow.h"

#define VERSION "0.0.1"
#define USAGE "Usage: xnotify [-d] [SUBJECT] MESSAGE\n"
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
    printf(" -d     debug mode: run in foreground (default is\n");
    printf("        to fork and run in background)\n");
    printf(" -v     print version and exit\n");
    printf(" -h     print help and exit\n");
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

    fd = popen("pgrep 'xnotify ' | wc -l", "r");
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


int main(int argc, char **argv) {

    char *subject, *message;
	int i, num, debug=0;

    // check arguments
    if ( argc < 2 ) {
        usage();
        return 1;
    }

    if ( (strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)
        || (strcmp(argv[1], "help") == 0) ) {
        help();
        return 0;
    }

    if ( strcmp(argv[1], "-v") == 0 ) {
        version();
        return 0;
    }

    i=1;
    if ( strcmp(argv[1], "-d") == 0 ) {
        debug = 1;
        i = 2;
    }

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

	// stop if too many pending notifications
	num = pending();
	if ( num > XNOTIFY_MAX ) {
		printf("Too many pending notifications\n");
		return 1;
	}


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

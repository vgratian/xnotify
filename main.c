#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TOP 1
#define BOTTOM (1<<1)
#define LEFT (1<<2)
#define RIGHT (1<<3)

#include "config.h"
#include "args.c"
#include "xwindow.c"

int main(int argc, char **argv) {

    printf("started main\n");
    if ( args_parse(argc, argv) != 0 ) {
        printf("failed args\n");
        return 1;
    }
    printf("parsed args:\n");
    printf("   title = [%s]\n", args.title);
    printf("     msg = [%s]\n", args.msg);
    printf(" timeout = [%d]\n", args.timeout);
    printf(" pending = [%d]\n", args.pending);

    return run();
}

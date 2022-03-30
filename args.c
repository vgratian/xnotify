

const char *usage = "notify - send a notification alert to user\n" \
    "\n" \
    "usage: notify -m MSG [ARGS]\n" \
    "\n" \
    "args:\n" \
    "   -m MSG  - message to show\n" \
    "   -f FNT  - X font name to use\n" \
    "   -d SEC  - delay before hiding\n";

static struct {
    char            *title;
    char            *msg;
    unsigned int    timeout;
    unsigned int    pending;

} args;

// parse single argument
int arg_parse(int i, int argc, char **argv) {

    if ( strlen(argv[i]) < 2 ) {
        fprintf(stderr, "invalid flag [%s]\n", argv[i]);
        return 1;
    }

	if ( argv[i][1] == 'h' ) {
        printf(usage);
        exit(0);
    }

    if ( i+1 >= argc ) {
        fprintf(stderr, "flag [%s] requires argument\n", argv[i]);
        return 1;
    }

    switch (argv[i][1]) {
        case 's':
            args.title = argv[i+1];
            break;
        case 'm':
            args.msg = argv[i+1];
            break;
        case 't':
            args.timeout = atoi(argv[i+1]);
            break;
        case 'p':
            args.pending = atoi(argv[i+1]);
            break;
    }
    return 0;
}

int args_parse(int argc, char **argv) {

    args.timeout = TIMEOUT;
    args.pending = 0;

    int i;

    for (i=0; i<argc; i++) {
        if (argv[i][0] == '-') {
            if ( arg_parse(i, argc, argv) != 0 )
                return 1;
        }
    }

    if ( ! args.msg ) {
        fprintf(stderr, "required argument [-m MSG]\n");
        return 1;
    }

    return 0;
}


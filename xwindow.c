
#include <locale.h>
#include <unistd.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

#if (POSITION & TOP)
    #define Y_COORD MARGIN_Y + n * (MARGIN_Z + WINDOW_H)
#else
    #define Y_COORD x.sh - (MARGIN_Y + (n+1) * (MARGIN_Z + WINDOW_H))
#endif

#if (POSITION & LEFT)
    #define X_COORD MARGIN_X
#else
    #define X_COORD x.sw - (MARGIN_X + WINDOW_W)
#endif

char *WCLASS = "xnotify";
char *WNAME = "Notification";

static struct {
    Display         *d;
    int             s, sw, sh;  // Screen ID, width & height
    Window          w, p;       // Window and parent window (root)
    int             x, y;       // Window x, y coordinates
} x;

/* - function declarations - */

int select_position(int);
int create_window(pid_t);
void terminate(int);
int draw_text(char*, char*);
unsigned int get_opacity();
void set_opacity(unsigned int);
void fade_in(unsigned int);
void fade_out(unsigned int);
int run();

/* - function definitions - */

/* - functions to create and initialize X window - */

// Select a position for the pop-up-window on the screen.
// Argument 'n' indicates number of notifications that
// are currently on the screen. Return 1 if there is
// no convenient place for new window.
int select_position(int n) {
    // get size of default screen
    x.s = XDefaultScreen(x.d);
    x.sw = XDisplayWidth(x.d, x.s);
    x.sh = XDisplayHeight(x.d, x.s);

    x.x = X_COORD;
    x.y = Y_COORD;

    // window can't be on or beyond screen edge
    // (we check only y, since multiple windows
    // will be arranged horizontally)
    return (x.y == 0 || x.y >= x.sh) ? 1 : 0;
}

// Create and map X window. Return 1 on failure.
int create_window(pid_t pid) {

	XColor c;
	Colormap cm;

    // parse background color
    cm = DefaultColormap(x.d, 0);
	if ( ! (XParseColor(x.d, cm, BCOLOR, &c)) ) {
        fprintf(stderr, "failed to parse color [%s]\n", BCOLOR);
        return 1;
    }
	if ( ! (XAllocColor(x.d, cm, &c)) ) {
        fprintf(stderr, "failed to allocate color\n");
        return 1;
    }

    XSetWindowAttributes attrs;
    attrs.background_pixel = c.pixel;
    // ask WM not to decorate window
    attrs.override_redirect = True;

    x.p = RootWindow(x.d, x.s);

    x.w = XCreateWindow(x.d, x.p, x.x, x.y, WINDOW_W, WINDOW_H, WINDOW_B,
        DefaultDepth(x.d, x.s), InputOutput, DefaultVisual(x.d, x.s),
        CWBackPixel | CWOverrideRedirect, &attrs);
    if ( x.w == 0 ) {
        fprintf(stderr, "failed to create window\n");
        return 1;
    }

    // tell WM window should be fixed
    XSizeHints shint;
    shint.min_width = shint.max_width = WINDOW_W;
    shint.min_height = shint.max_height = WINDOW_H;
    shint.flags = PMinSize | PMaxSize;
    XSetWMNormalHints(x.d, x.w, &shint);

    // set X properties that identify our window
    XClassHint chint = { WCLASS, WNAME };
    XSetClassHint(x.d, x.w, &chint);
    XStoreName(x.d, x.w, WCLASS);

    // PID of our process
    XChangeProperty(x.d, x.w, XInternAtom(x.d, "_NET_WM_PID", False),
        XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &pid, 1);

    // fix initial transparency
    set_opacity(OPACITY_MIN);

    // subscribe to events
    unsigned long em;
    em = ExposureMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask;
    XSelectInput(x.d, x.w, em);

    XMapWindow(x.d, x.w);

    // XFreeColors(x.d, cm, &c.pixel, 1, 0); // necessary?
    return 0;
}

// Signal handler for alarm(): terminate X window gracefully.
// Since our process is probably blocked by XNextEvent(), the
// reliable way to do this, is to send an event to ourself.
void terminate(int signum) {

    XButtonEvent e;

    if (x.d) {
        e.type = ButtonRelease;
        e.send_event = False;
        e.display = x.d;
        e.root = x.p;
        e.window = x.w;
        XSendEvent(x.d, x.w, True, ButtonReleaseMask, (XEvent*) &e);
        XFlush(x.d);
    } else {
        exit(2);
    }
}

/* - functions to display text on X window - */

// Load XFT resources (fonts, foreground color) and draw
// the notification text on the window.
//
// This function also calculates if there is enough space
// for the text on the window (and if necessary, only draws
// part of the body text (msg)). The current impementation
// is only POC and needs a lot of improvement.
int draw_text(char *sbj, char *msg) {

    int s;

    XftFont   *fb = NULL;
    XftFont   *fs = NULL;
    XftDraw   *d;
    XftColor  c;

    Visual    *vis = XDefaultVisual(x.d, x.s);
    Colormap  cm = XDefaultColormap(x.d, x.s);

    if (! (d = XftDrawCreate(x.d, x.w, vis, cm)) ) {
        fprintf(stderr, "failed to create Xft draw\n");
        return 1;
    }

    // returns 1 on success (unlike most X functions)
    if ( XftColorAllocName(x.d, vis, cm, FCOLOR, &c) != 1 )  {
        fprintf(stderr, "failed to alloc Xft color [%s]\n", FCOLOR);
        return 1;
    }

    if (! (fb = XftFontOpenName(x.d, x.s, FONT_BODY)) ) {
        fprintf(stderr, "failed to load body font [%s]\n", FONT_BODY);
        return 1;
    }

    XClearWindow(x.d, x.w);

    // draw subject on first line (optional)
    if ( sbj ) {
        // load subject font only if we need to draw it
        if (! (fs = XftFontOpenName(x.d, x.s, FONT_SUBJ)) ) {
            fprintf(stderr, "failed to load subject font [%s]\n", FONT_BODY);
            return 1;
        }
        // no way to tell if drawing the text was success
        // (other than segmentation fault)
        XftDrawStringUtf8(d, &c, fs, PAD_X, PAD_Y, (XftChar8 *) sbj, strlen(sbj));
    }

    // check if there is enough space for body text 
    int len = strlen(msg);
    // available space on window (pixels)
    int size_w = WINDOW_W - 2 * PAD_X;
    int size_h = WINDOW_H - 2 * PAD_Y;

    XGlyphInfo xg;
    XftTextExtentsUtf8(x.d, fb, (XftChar8 *) msg, len, &xg);

    /*
    printf(" XGlyphInfo: ---------- \n");
    printf("     width:  %d\n", xg.width);
    printf("     height: %d\n", xg.height);
    printf("     x:      %d\n", xg.x);
    printf("     y:      %d\n", xg.y);
    printf("     xOff:   %d\n", xg.xOff);
    printf("     yOff:   %d\n", xg.yOff);
    printf(" ---------------------- \n");
	printf("     size_w  %d\n", size_w);
	printf("     size_h  %d\n", size_h);
    printf(" ---------------------- \n");
    */

    // no space: strip msg
	if ( xg.width > size_w ) {

        int newlen;

		// find percent "p" of msg we can display (+2 chars for "..")
		// p * xg.width = (size_w - 2) --->
		float p = (float) (size_w - 2) / (float) xg.width;
		newlen = (int) (p * (float) len);
		if (newlen % 2) // avoid corrupting 2-byte characters
            newlen--; // TODO: doesn't work for mixed strings

		msg[newlen] = '.';
		msg[newlen+1] = '.';
		msg[newlen+2] = '\0';
        len = newlen+2;
	}

    XftDrawStringUtf8(d, &c, fb, PAD_X, (PAD_Y + PAD_Z + xg.height),
        (XftChar8 *) msg, len);

    XFlush(x.d);

    // release resources
    XftFontClose(x.d, fb);
    if (fs) {
        XftFontClose(x.d, fs);
    }
    XftDrawDestroy(d);
    XftColorFree(x.d, vis, cm, &c);
    return 0;
}

/* - functions that change window transparency - */

unsigned int get_opacity() {

    int fmt;
    unsigned long num, left;
    unsigned char *data;
    unsigned int val;

    Atom actual, req = XInternAtom(x.d, "_NET_WM_WINDOW_OPACITY", False);
    if ( XGetWindowProperty(x.d, x.w, req, 0L, 1L, False,
            XA_CARDINAL, &actual, &fmt, &num, &left, &data)
            == Success && data != None ) {
        memcpy(&val, data, sizeof(unsigned int));
        XFree((void*) data);
    } else {
        val = OPACITY_MAX;
    }
    return val;
}

void set_opacity(unsigned int val) {
    Atom a = XInternAtom(x.d, "_NET_WM_WINDOW_OPACITY", False);
    XChangeProperty(x.d, x.w, a, XA_CARDINAL, 32, PropModeReplace,
        (unsigned char*) &val, 1L);
}

void fade_in(unsigned int max) {
    unsigned int val = get_opacity();
    printf(" ~~~> current: %lu, max: %lu\n", val, max);
    val += FADEIN_DELTA;
    while ( val < max ) {
        set_opacity(val);
        XSync(x.d, False);
        val += FADEIN_DELTA;
        usleep(FADEIN_NSECS);
    }
    XFlush(x.d);
}


void fade_out(unsigned int min) {
    unsigned int val = get_opacity();
    printf(" ~~~> current: %lu, max: %lu\n", val, min);
    val -= FADEOUT_DELTA;
    while ( val > min ) {
        set_opacity(val);
        XSync(x.d, False);
        val -= FADEOUT_DELTA;
        usleep(FADEOUT_NSECS);
    }
    XFlush(x.d);
}

/* - main routine - */

/*
    Arguments:

    sbj (subject)  -  text to be displayed on the top
    msg (message)  -  notification body text
    num            -  number of pending notifications
    pid            -  PID of current process

*/

int run(char *sbj, char *msg, int num, pid_t pid) {

    // for UTF8 support
    setlocale(LC_ALL, getenv("LANG"));

    if ( ! (x.d = XOpenDisplay(NULL)) ) {
        fprintf(stderr, "failed to open X display\n");
        return 1;
    }

    // calcuate x, y coordinates of window
    if ( select_position(num) != 0 ) {
        fprintf(stderr, "not enough space on screen\n");
        return 1;
    }

    //printf("screen (%d) w=%d, h=%d\n", x.s, x.sw, x.sh);
    //printf("window (%d) x=%d, y=%d\n", x.w, x.x, x.y);

    if ( create_window(pid) != 0 ) {
        fprintf(stderr, "failed to create X window\n");
        return 1;
    }

    // Window should disappear after TIMEOUT seconds,
    // use alarm to terminate on time. When mouse pointer
    // hovers over the notification window, the alarm will
    // be suspended and when the pointer leaves, resumed
    // again with the remaining time.

    signal(SIGALRM, terminate);
    alarm(TIMEOUT);
    int r=0; // remaining time, if alarm is suspended

    // enter event loop
    XEvent e;

    while (1) {
        XNextEvent(x.d, &e);

        switch (e.type) {
            case Expose:
                if ( draw_text(sbj, msg) != 0 )
                    break;
                fade_in(OPACITY_AVG);
                continue;
            // click-release: cancel alarm and fade-out
            case ButtonRelease:
                alarm(0);
                break;
            // mouse pointer hovers over window: suspend
            // alarm and decrease opacity
            case EnterNotify:
                r = alarm(0);
                fade_in(OPACITY_MAX);
                continue;
            // mouse pointer leaves window: resume alarm
            // if there was remaining time.
            case LeaveNotify:
                fade_out(OPACITY_AVG);
                if (r == 0)
                    break;
                alarm(r);
                continue;
        }
        break;
    }

    // back to minimal opacity
    fade_out(OPACITY_MIN);

    // close window and X display
    XUnmapWindow(x.d, x.w);
    XDestroyWindow(x.d, x.w);
    XCloseDisplay(x.d);

    return 0;
}

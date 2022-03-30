
#include <locale.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

#if ORIENTATION == TOP_LEFT
    #define X_COORD MARGIN_X
    #define Y_COORD MARGIN_Y + n * (MARGIN + WINDOW_H)
#elif ORIENTATION == TOP_RIGHT
    #define X_COORD xscreen.w - (MARGIN_X + WINDOW_W)
    #define Y_COORD MARGIN_Y + n * (MARGIN + WINDOW_H)
#elif ORIENTATION == BOTTOM_LEFT
    #define X_COORD MARGIN_X
    #define Y_COORD xscreen.h - (MARGIN_Y + (n+1) * (MARGIN + WINDOW_H))
#else
    #define X_COORD xscreen.w - (MARGIN_X + WINDOW_W)
    #define Y_COORD xscreen.h - (MARGIN_Y + (n+1) * (MARGIN + WINDOW_H))
#endif


static struct {
    Display         *dpy;
    int             screen;
    Window          id;
    Window          parent;
    unsigned int    x;
    unsigned int    y;
    unsigned int    opacity;
    XftFont         *bfont;
    XftFont         *tfont;
    XftDraw         *draw;
    XftColor        color;
    XRenderColor    rcolor;
} xwin;


static struct {
    int id;
    int w;
    int h;
} xscreen;


int position_xwin(int n) {
    xscreen.id = XDefaultScreen(xwin.dpy);
    xscreen.w = XDisplayWidth(xwin.dpy, xscreen.id);
    xscreen.h = XDisplayHeight(xwin.dpy, xscreen.id);

    xwin.x = X_COORD;
    xwin.y = Y_COORD;

    if ( xwin.y == 0 || xwin.y >= xscreen.h )
        return 1;
    return 0;
}

int create_xwin() {

    XSizeHints              hints;
    XSetWindowAttributes    attrs;

    attrs.background_pixel = BACKGROUND;
    attrs.override_redirect = True;

    xwin.parent = RootWindow(xwin.dpy, xscreen.id);

    // create window
    xwin.id = XCreateWindow(xwin.dpy, xwin.parent, xwin.x, xwin.y, WINDOW_W,
        WINDOW_H, WINDOW_B, DefaultDepth(xwin.dpy, xwin.screen), InputOutput,
        DefaultVisual(xwin.dpy, xwin.screen), CWBackPixel | CWOverrideRedirect, &attrs);

    if ( xwin.id == 0 ) return 1;
    XStoreName(xwin.dpy, xwin.id, "new notification");

    // ask window manager to keep fixed size
    hints.min_width = hints.max_width = WINDOW_W;
    hints.min_height = hints.max_height = WINDOW_H;
    hints.flags = PMinSize | PMaxSize;
    XSetWMNormalHints(xwin.dpy, xwin.id, &hints);

    // set initial opacity
    xwin.opacity = OPACITY_MIN;
    XChangeProperty(xwin.dpy, xwin.id, XInternAtom(xwin.dpy, OPACITY_XPROP, False),
        XA_CARDINAL, 32, PropModeReplace, (unsigned char*) &xwin.opacity, 1L);

    // subscribe for events
    XSelectInput(xwin.dpy, xwin.id, ExposureMask | ButtonPressMask | ButtonReleaseMask);

    XMapWindow(xwin.dpy, xwin.id);
    return 0;
}

// load Xft resources
int load_xft() {
    xwin.bfont = XftFontOpenName(xwin.dpy, xscreen.id, FONT_BODY);
    if ( ! xwin.bfont ) {
        fprintf(stderr, "failed to load Xft font [%s]\n", FONT_BODY);
        return 1;
    }

    xwin.tfont = XftFontOpenName(xwin.dpy, xscreen.id, FONT_TITLE);
    if ( ! xwin.tfont ) {
        fprintf(stderr, "failed to load Xft font [%s]\n", FONT_TITLE);
        return 1;
    }

    Colormap cm = DefaultColormap(xwin.dpy, xscreen.id);
    Visual *vis = DefaultVisual(xwin.dpy, xscreen.id);

    xwin.draw = XftDrawCreate(xwin.dpy, xwin.id, vis, cm);

    xwin.rcolor.red = 65535;
    xwin.rcolor.green = 0;
    xwin.rcolor.blue = 0;
    xwin.rcolor.alpha = 65535;

    XftColorAllocValue(xwin.dpy, vis, cm, &xwin.rcolor, &xwin.color);

    return 0;
}

void draw_text() {
    XClearWindow(xwin.dpy, xwin.id);
    if ( args.title ) {
        printf("drawing title [%s]\n", args.title);
        XftDrawStringUtf8(xwin.draw, &xwin.color, xwin.tfont,
            20, 20, (XftChar8 *) args.title, strlen(args.title));
    }

    printf("drawing body [%s]\n", args.msg);
    XftDrawStringUtf8(xwin.draw, &xwin.color, xwin.bfont,
        20, 40, (XftChar8 *) args.msg, strlen(args.msg));

    //XFlush(xwin.dpy);
}

void fade_in() {
    unsigned int value = OPACITY_MIN;
    Atom xa = XInternAtom(xwin.dpy, OPACITY_XPROP, False);

    while ( value < OPACITY_MAX ) {
        value += OPACITY_STEP;
        XChangeProperty(xwin.dpy, xwin.id, xa, XA_CARDINAL, 32,
            PropModeReplace, (unsigned char*) &value, 1L);
        XSync(xwin.dpy, False);
        usleep(10000);
    }
}

void fade_out() {
    unsigned int value = OPACITY_MAX;
    Atom xa = XInternAtom(xwin.dpy, OPACITY_XPROP, False);

    while ( value > OPACITY_MIN ) {
        value -= OPACITY_STEP;
        XChangeProperty(xwin.dpy, xwin.id, xa, XA_CARDINAL, 32,
            PropModeReplace, (unsigned char*) &value, 1L);
        XSync(xwin.dpy, False);
        usleep(5000);
    }
}

int run() {

    printf("started run\n");
    setlocale(LC_ALL, "en_US.UTF-8");

    if ( ! (xwin.dpy = XOpenDisplay(NULL)) ) {
        fprintf(stderr, "failed to open X display");
        return 1;
    }
    printf("opened X disp\n");

    // calcuate x, y coordinates of window
    if ( position_xwin(args.pending) != 0 ) {
        fprintf(stderr, "not enough space on screen");
        return 1;
    }
    printf("calculated x, y\n");
    printf("       xwin x=%d, y=%d\n", xwin.x, xwin.y);
    printf("screen (%d) w=%d, h=%d\n", xscreen.id, xscreen.w, xscreen.h);

    if ( create_xwin() != 0 ) {
        fprintf(stderr, "failed to create X window");
        return 1;
    }
    printf("created window\n");

    if ( load_xft() != 0 ) {
        fprintf(stderr, "failed to load Xft fonts");
        return 1;
    }
    printf("loaded Xft\n");

    // enter event loop
    XEvent e;

    printf("event loop...\n");
    while (1) {
        XEvent e;
        XNextEvent(xwin.dpy, &e);

        if (e.type == Expose) {
            printf("event - Expose!\n");
            draw_text();
            fade_in();
        } else if (e.type == ButtonRelease) {
            printf("event - ButtonRelease!\n");
            fade_out();
            break;
        } else if (e.type == ButtonPress) {
            printf("event - ButtonPress\n");
            fade_in();
        } else {
            printf("event - else...\n");
            fade_out();
            break;
        }
    }

    // TODO: release resources
    return 0;
}

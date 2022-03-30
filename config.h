
// orientation of the pop-up window on the screen, possible values:
// TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT
#define ORIENTATION TOP_LEFT

// window width
#define WINDOW_W 300
// window height
#define WINDOW_H 70
#define WINDOW_B 0

// background color
#define BCOLOR "#fafafa"
// text color
#define FCOLOR "#606060"

// horizontal margin between window and screen edge
#define MARGIN_X 60
// vertical margin between window and screen edge
#define MARGIN_Y 40
// margin between windows (multiple notifications)
#define MARGIN 30

// maximum number of notifications that can appear
// on screen simultaneously (if there is enough space).
#define XNOTIFY_MAX 5

// timeout before the notification will disappear
#define TIMEOUT 5

#define SLEEP_FADEIN 2500
#define SLEEP_FADEOUT 15000

// window transparency properties
#define OPACITY_MAX 2500000000
#define OPACITY_MIN  100000000
#define OPACITY_STEP  10000000
#define OPACITY_XPROP "_NET_WM_WINDOW_OPACITY"

// Xft font names
#define FONT_BODY "Bitstream Vera Sans Mono:pixelsize=16:antialias=true:autohint=true"
#define FONT_TITLE "Bitstream Vera Sans Mono:style=bold:pixelsize=16:antialias=true:autohint=true"

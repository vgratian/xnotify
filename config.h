
// Orientation of the pop-up window on the screen, possible values:
// TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT
#define ORIENTATION TOP_LEFT

// RGB colors for background and foreground (text)
#define BCOLOR "#fafafa" // white
#define FCOLOR "#606060" // grey-black

// XFT fonts for the text displayed, for the XFT naming convention, see:
// https://keithp.com/~keithp/talks/xtc2001/paper/xft.html#sec-xft-name
// Note that if font is not available or has invalid syntax, XFT will
// load some other font, that you wont like (e.g. ASCII only).
#define FONT_BODY "Bitstream Vera Sans Mono:pixelsize=14:antialias=true:autohint=true"
#define FONT_TITLE "Bitstream Vera Sans Mono:style=bold:pixelsize=14:antialias=true:autohint=true"

// Maximum number of notifications that may appear on the screen
// simultaneously (if there is enough space).
#define XNOTIFY_MAX 5

// Duration of the notification in seconds. Note that if you hover
// over the pop-up window, it may last longer. Conversely, if you
// click-release, it may disappear sooner.
#define TIMEOUT 5

// Geometry of the pop-up window.

// Window height and width, border width.
#define WINDOW_W 300
#define WINDOW_H 70
#define WINDOW_B 0

// Horizontal and vertical space between window and screen edge.
#define MARGIN_X 60
#define MARGIN_Y 40
// Space between pop-up windows (in case of multiple notifications).
#define MARGIN_Z 30

// Horizontal and vertical space between text and window edge.
#define PAD_X 20
#define PAD_Y 30
// Space between text lines.
#define PAD_Z 5

// Fade-in and fade-out properties.
#define OPACITY_MIN 0x10000000
#define OPACITY_AVG 0x80000000
#define OPACITY_MAX 0xd0000000
#define FADEIN_NSECS  500
#define FADEOUT_NSECS 500
#define FADEIN_DELTA  0x300000
#define FADEOUT_DELTA 0x300000


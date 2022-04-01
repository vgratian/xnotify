#include <locale.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

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


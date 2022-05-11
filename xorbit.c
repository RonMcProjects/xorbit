/* 
 * gcc -Wall xorbit.c -lm -lX11 -o xorbit
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * Constants
 */
char WINDOW_NAME[] = __FILE__;
char ICON_NAME[] = __FILE__;
#define SCALE_FACTOR 512
#define WINDOW_WIDTH (SCALE_FACTOR*2+2)
#define WINDOW_HEIGHT (SCALE_FACTOR*2+2)
#define PI 3.1415926536
#define NUMPOINTS 60
#define NUMORBITS 15

/*
 * Globals
 */
Display *display = NULL;
Window xwindow;
int screen;
XColor RGB_color, hardware_color; /* added for color. */
Colormap color_map;               /* added for color. */
unsigned long foreground, background, earthcolor, marscolor, suncolor, linecolor;

int draw_orbits()
{
    double point;
    double numpoints = (double)NUMPOINTS;
    double earth_theta, mars_theta;
    double x_earth, y_earth;
    double x_mars, y_mars;
    double x_earth_scaled, y_earth_scaled;
    double x_mars_scaled, y_mars_scaled;
    double fraction_earth_mars = 149598023.0 * SCALE_FACTOR / 227939366.0;
    double x_earth_prior=5, y_earth_prior=5, x_mars_prior=5, y_mars_prior=5;
    for (point = 0.0; point <= numpoints*(double)NUMORBITS; point++)
    {
        if ((int)point % NUMPOINTS == 0)
        {
            XSetForeground(display, DefaultGC(display, screen), suncolor);
            XDrawArc(display, xwindow, DefaultGC(display, screen), SCALE_FACTOR-1, SCALE_FACTOR-1, 4, 4, 0, 360*64);
            XDrawArc(display, xwindow, DefaultGC(display, screen), SCALE_FACTOR, SCALE_FACTOR, 2, 2, 0, 360*64);
            XDrawPoint(display, xwindow, DefaultGC(display, screen), SCALE_FACTOR+1, SCALE_FACTOR+1);
        }
        earth_theta= 2.0*PI/numpoints * point;
        mars_theta = earth_theta / (687.0 / 365.25);
        x_earth = cos(earth_theta);
        y_earth = sin(earth_theta);
        x_earth_scaled = x_earth * fraction_earth_mars + SCALE_FACTOR+1;
        y_earth_scaled = -1.0 * y_earth * fraction_earth_mars + SCALE_FACTOR+1;
        x_mars = cos(mars_theta);
        y_mars = sin(mars_theta);
        x_mars_scaled = x_mars * SCALE_FACTOR + SCALE_FACTOR+1;
        y_mars_scaled = -1.0 * y_mars * SCALE_FACTOR + SCALE_FACTOR+1;
        XSetForeground(display, DefaultGC(display, screen), BlackPixel(display, screen));
        XDrawArc(display, xwindow, DefaultGC(display, screen), (int)x_earth_prior-1, (int)y_earth_prior-1, 2, 2, 0, 360*64);
        XSetForeground(display, DefaultGC(display, screen), linecolor);
        XDrawPoint(display, xwindow, DefaultGC(display, screen), (int)x_earth_prior, (int)y_earth_prior);
        XSetForeground(display, DefaultGC(display, screen), BlackPixel(display, screen));
        XDrawArc(display, xwindow, DefaultGC(display, screen), (int)x_mars_prior-1, (int)y_mars_prior-1, 2, 2, 0, 360*64);
        XSetForeground(display, DefaultGC(display, screen), linecolor);
        XDrawPoint(display, xwindow, DefaultGC(display, screen), (int)x_mars_prior, (int)y_mars_prior);
        XSetForeground(display, DefaultGC(display, screen), linecolor);
        XDrawLine(display, xwindow, DefaultGC(display,screen), (int)x_earth_scaled, (int)y_earth_scaled, (int)x_mars_scaled, (int)y_mars_scaled);
        XSetForeground(display, DefaultGC(display, screen), earthcolor);
        XDrawArc(display, xwindow, DefaultGC(display, screen), (int)x_earth_scaled-1, (int)y_earth_scaled-1, 2, 2, 0, 360*64);
        XDrawPoint(display, xwindow, DefaultGC(display, screen), (int)x_earth_scaled, (int)y_earth_scaled);
        XSetForeground(display, DefaultGC(display, screen), marscolor);
        XDrawArc(display, xwindow, DefaultGC(display, screen), (int)x_mars_scaled-1, (int)y_mars_scaled-1, 2, 2, 0, 360*64);
        XDrawPoint(display, xwindow, DefaultGC(display, screen), (int)x_mars_scaled, (int)y_mars_scaled);
        usleep(50000000/(NUMPOINTS*NUMORBITS));
        x_earth_prior = x_earth_scaled;
        y_earth_prior = y_earth_scaled;
        x_mars_prior = x_mars_scaled;
        y_mars_prior = y_mars_scaled;
        XFlush(display);
    }

    return (0);
}

void setup_xwindow(int argc, char **argv)
{
    XSizeHints size_hints;
    int depth;

    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    screen = DefaultScreen(display);

    foreground = WhitePixel(display, screen);
    background = BlackPixel(display, screen);
    depth = DefaultDepth(display, screen);
    if (depth > 1)              /* not monochrome */
    {
        earthcolor = 0x87ceeb;
        marscolor = 0x9c2e35;
        linecolor = 0x404040;
        suncolor = 0xfce570;
    }
    xwindow = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, WINDOW_WIDTH, WINDOW_HEIGHT, 1,
                                   foreground, background);
    XSetForeground(display, DefaultGC(display, screen), foreground);
    XSetBackground(display, DefaultGC(display, screen), background);
    /* set up the size hints for the window manager. */
    size_hints.x = 10;
    size_hints.y = 10;
    size_hints.width = WINDOW_WIDTH;
    size_hints.height = WINDOW_HEIGHT;
    size_hints.flags = PPosition | PSize;
    /* and state what hints are included. */
    XSetStandardProperties(display, xwindow, WINDOW_NAME, ICON_NAME, None,   /* no icon map */
                           argv, argc, &size_hints);
    XSelectInput(display, xwindow, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(display, xwindow);
}

int main(argc, argv)
int argc;
char **argv;
{
    XEvent event;

    setup_xwindow(argc, argv);

    while(1)
    {
        XNextEvent(display, &event);
        if (event.type == Expose)
        {
            draw_orbits();
        }
        if ((event.type == KeyPress) || (event.type == ButtonPress))
            break;
    }

    XCloseDisplay(display);
    return(0);
}

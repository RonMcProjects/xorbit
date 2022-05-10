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

/*
 * Globals
 */
Display *display = NULL;
Window xwindow;
int screen;

int draw_orbits()
{
    double point;
    double numpoints = 60.0;
    double earth_theta, mars_theta;
    double x_earth, y_earth;
    double x_mars, y_mars;
    double x_earth_scaled, y_earth_scaled;
    double x_mars_scaled, y_mars_scaled;
    double fraction_earth_mars = 149598023.0 * SCALE_FACTOR / 227939366.0;
    //length_str = sprintf(factor_str, "%d", factor);
    //XDrawArc(display, main_window, gc, 0, 0, SCALE_FACTOR*2, SCALE_FACTOR*2, 0, 360*64);
    //XDrawString(display, main_window, gc, SCALE_FACTOR*2 - 30, SCALE_FACTOR*2 - 30, factor_str, length_str);
    for (point = 0.0; point <= numpoints*5.0; point++)
    {
        earth_theta= 2.0*PI/numpoints * point;
        mars_theta = earth_theta / 2.1354;
        x_earth = cos(earth_theta);
        y_earth = sin(earth_theta);
        x_earth_scaled = x_earth * fraction_earth_mars + SCALE_FACTOR+1;
        y_earth_scaled = -1.0 * y_earth * fraction_earth_mars + SCALE_FACTOR+1;
        XDrawPoint(display, xwindow, DefaultGC(display,screen), (int)x_earth_scaled, (int)y_earth_scaled);
        x_mars = cos(mars_theta);
        y_mars = sin(mars_theta);
        x_mars_scaled = x_mars * SCALE_FACTOR + SCALE_FACTOR+1;
        y_mars_scaled = -1.0 * y_mars * SCALE_FACTOR + SCALE_FACTOR+1;
        XDrawPoint(display, xwindow, DefaultGC(display,screen), (int)x_mars_scaled, (int)y_mars_scaled);
        XDrawLine(display, xwindow, DefaultGC(display,screen), (int)x_earth_scaled, (int)y_earth_scaled, (int)x_mars_scaled, (int)y_mars_scaled);
        usleep(250000);
        XFlush(display);
    }

    return (0);
}

void setup_xwindow(int argc, char **argv)
{
    XSizeHints size_hints;

    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    screen = DefaultScreen(display);
    xwindow = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, WINDOW_WIDTH, WINDOW_HEIGHT, 1,
                                   WhitePixel(display, screen), BlackPixel(display, screen));
    XSetBackground(display, DefaultGC(display, screen), BlackPixel(display, screen));
    XSetForeground(display, DefaultGC(display, screen), WhitePixel(display, screen));
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

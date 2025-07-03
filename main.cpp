#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <string>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
int main() {
    Display *d = XOpenDisplay(NULL);
    int s = DefaultScreen(d);
    
    // Get screen dimensions
    int screen_width = DisplayWidth(d, s);
    int screen_height = DisplayHeight(d, s);
    // Create a full-screen window
    Window w = XCreateSimpleWindow(
            d, RootWindow(d, s), 0, 0,
            screen_width, screen_height,
            0,                  // size of borders
            BlackPixel(d, s),   // border color
            WhitePixel(d, s)    // background or BlackPixel(d,s) if want black background
            );
    XStoreName(d, w, "Statistica");
    Atom wm_state = XInternAtom(d, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(d, "_NET_WM_STATE_FULLSCREEN", False);
    
    XEvent xev;
    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = w;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;  
    xev.xclient.data.l[1] = fullscreen;
    xev.xclient.data.l[2] = 0;
    
    XSendEvent(d, DefaultRootWindow(d), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    
    XFontStruct *font = XLoadQueryFont(d, "monospace-bold");
    if (!font) font = XLoadQueryFont(d, "10x20"); 
    
    GC gc = XCreateGC(d, w, 0, NULL);
    if (font) XSetFont(d, gc, font->fid);
    
    XSelectInput(d, w, ExposureMask | ButtonPressMask);
    XMapWindow(d, w);
    XEvent e;
    
    struct utsname data;
    struct sysinfo info;
    if (uname(&data) == 0 && sysinfo(&info) == 0) {
        std::string os = "OS: " + std::string(data.sysname);
        std::string hostname = "Hostname: " + std::string(data.nodename);
        std::string arch = "Cpu Architecture: " + std::string(data.machine);
        std::string ram = "Total Ram(Mib): " + std::to_string(info.totalram / 1048576); // info.totalram will be by default in bytes
        std::string freeram = "Free Ram(Mib): " + std::to_string(info.freeram /1048576);
        while (true) {
            XNextEvent(d, &e);
            int rect_x = (screen_width - 40) / 2;
            int rect_y = (screen_height - 25) / 2;
            XDrawString(d, w, gc, rect_x - 45, rect_y - 430, "Statistica", 10);

            XDrawString(d, w, gc, rect_x - 750, rect_y - 350, os.c_str(), os.size());
            XDrawString(d, w, gc, rect_x - 750, rect_y - 300, hostname.c_str(), hostname.size());
            XDrawString(d, w, gc, rect_x - 750, rect_y - 250, arch.c_str(), arch.size());
            XDrawString(d, w, gc, rect_x - 750, rect_y - 200, ram.c_str(), ram.size());
            XDrawString(d, w, gc, rect_x - 750, rect_y - 150, freeram.c_str(), freeram.size());
        }
    }
    XCloseDisplay(d);
    return 0;
}

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
int main() {
    std::string os, hostname, arch, ram, freeram;
    #ifdef _WIN32
        OSVERSIONINFOEX osInfo;
        osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        GetVersionEx((OSVERSIONINFO*)&osInfo);
        os = "OS: Windows " + std::to_string(osInfo.dwMajorVersion) + "." + std::to_string(osInfo.dwMinorVersion);

        char computerName[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(computerName);
        GetComputerNameA(computerName, &size);
        hostname = "Hostname: " + std::string(computerName);

        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        arch = "Cpu Architecture: ";
        switch (sysInfo.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_AMD64: arch += "x86_64"; break;
            case PROCESSOR_ARCHITECTURE_ARM: arch += "ARM"; break;
            case PROCESSOR_ARCHITECTURE_IA64: arch += "IA-64"; break;
            case PROCESSOR_ARCHITECTURE_INTEL: arch += "x86"; break;
            default: arch += "Unknown"; break;
        }

        MEMORYSTATUSEX memoryStatus;
        memoryStatus.dwLength = sizeof(memoryStatus);
        GlobalMemoryStatusEx(&memoryStatus);
        ram = "Total Ram(Mib): " + std::to_string(memoryStatus.ullTotalPhys / 1048576);
        freeram = "Free Ram(Mib): " + std::to_string(memoryStatus.ullAvailPhys / 1048576);

        // Sorry Windows users not now graphical display
        printf("%s\n%s\n%s\n%s\n%s\n", os.c_str(), hostname.c_str(), arch.c_str(), ram.c_str(), freeram.c_str());
  
    #elif __linux__


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
    
        XSendEvent(d, DefaultRootWindow(d), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    
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
            os = "OS: " + std::string(data.sysname);
            hostname = "Hostname: " + std::string(data.nodename);
            arch = "Cpu Architecture: " + std::string(data.machine);
            ram = "Total Ram(Mib): " + std::to_string(info.totalram / 1048576); // info.totalram will be by default in bytes
            freeram = "Free Ram(Mib): " + std::to_string(info.freeram /1048576);
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
    #endif
    return 0;
}

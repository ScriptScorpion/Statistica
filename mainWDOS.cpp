// Sorry Windows users i currently writing this script on Linux and cannot make executable because of this.
// to do executable of this file yourself you need to install gcc and run: g++ mainWDOS.cpp -o WDOS.exe -lgdi32 -mwindows
#include <windows.h>
#include <string>

std::string os, hostname, arch, ram, freeram;

void GetSystemInfo() {
    OSVERSIONINFOEX osInfo = { sizeof(OSVERSIONINFOEX) };
    GetVersionEx((OSVERSIONINFO*)&osInfo);
    os = "OS: Windows " + std::to_string(osInfo.dwMajorVersion);

    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    GetComputerNameA(computerName, &size);
    hostname = "Hostname: " + std::string(computerName);

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    arch = "Cpu Architecture: " + std::string(sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "x86_64" : "x86");

    MEMORYSTATUSEX memoryStatus = { sizeof(MEMORYSTATUSEX) };
    GlobalMemoryStatusEx(&memoryStatus);
    ram = "Total Ram(Mib): " + std::to_string(memoryStatus.ullTotalPhys / 1048576);
    freeram = "Free Ram(Mib): " + std::to_string(memoryStatus.ullAvailPhys / 1048576);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
     switch (msg) {
          case WM_PAINT: {
               PAINTSTRUCT ps;
               HDC hdc = BeginPaint(hwnd, &ps);
               
               SetTextColor(hdc, RGB(0, 0, 0));
               SetBkColor(hdc, RGB(255, 255, 255));
               
               HFONT hFont = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,FIXED_PITCH, "Courier New");
               SelectObject(hdc, hFont);
               
               int screen_width = GetSystemMetrics(SM_CXSCREEN);
               int screen_height = GetSystemMetrics(SM_CYSCREEN);
               
               int rect_x = (screen_width - 40) / 2;
               int rect_y = (screen_height - 25) / 2;
               
               TextOutA(hdc, rect_x - 45, rect_y - 430, "Statistica", 10);
               TextOutA(hdc, rect_x - 750, rect_y - 350, os.c_str(), os.length());
               TextOutA(hdc, rect_x - 750, rect_y - 300, hostname.c_str(), hostname.length());
               TextOutA(hdc, rect_x - 750, rect_y - 250, arch.c_str(), arch.length());
               TextOutA(hdc, rect_x - 750, rect_y - 200, ram.c_str(), ram.length());
               TextOutA(hdc, rect_x - 750, rect_y - 150, freeram.c_str(), freeram.length());
               
               DeleteObject(hFont);
               EndPaint(hwnd, &ps);
               break;
          }
          case WM_DESTROY:
               PostQuitMessage(0);
               break;
          default:
               return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GetSystemInfo();
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "Statistica";
    wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    RegisterClass(&wc);
    
    HWND hwnd = CreateWindowEx(
        0,
        "Statistica",
        "Statistica",
        WS_POPUP | WS_VISIBLE,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL
    );
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }
    DeleteObject(wc.hbrBackground);
    return (int)msg.wParam;
}
// window_tracker.h
#ifndef WINDOW_TRACKER_H
#define WINDOW_TRACKER_H

#include <windows.h>
#include <string>
#include <vector>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

class WindowTracker {
private:
    std::string lastWindow;
    
public:
    std::string GetActiveWindowTitle() {
        char windowTitle[256];
        HWND foregroundWindow = GetForegroundWindow();
        GetWindowTextA(foregroundWindow, windowTitle, sizeof(windowTitle));
        return std::string(windowTitle);
    }
    
    std::string GetActiveWindowProcess() {
        char processPath[MAX_PATH];
        HWND foregroundWindow = GetForegroundWindow();
        DWORD processId;
        GetWindowThreadProcessId(foregroundWindow, &processId);
        
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | 
                                          PROCESS_VM_READ, FALSE, processId);
        if (processHandle) {
            GetModuleFileNameExA(processHandle, NULL, processPath, MAX_PATH);
            CloseHandle(processHandle);
            return std::string(processPath);
        }
        return "Unknown";
    }
    
    bool WindowChanged() {
        std::string current = GetActiveWindowTitle();
        if (current != lastWindow) {
            lastWindow = current;
            return true;
        }
        return false;
    }
};

#endif
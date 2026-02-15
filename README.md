# Advanced C++ Keylogger for Windows

![C++](https://img.shields.io/badge/language-C%2B%2B-blue)
![Platform](https://img.shields.io/badge/platform-Windows-yellow)
![License](https://img.shields.io/badge/license-MIT-green)

## ⚠️ IMPORTANT LEGAL DISCLAIMER

**THIS SOFTWARE IS FOR EDUCATIONAL PURPOSES ONLY**

- Use ONLY on systems you own or have explicit written permission
- Use ONLY in isolated lab environments (VMs, test machines)
- UNAUTHORIZED USE IS ILLEGAL AND UNETHICAL
- Author not responsible for misuse [citation:1][citation:3]

## 🎯 Project Overview

This C++ keylogger demonstrates Windows API programming, system hooks, and security concepts. Developed for cybersecurity education.

## ✨ Features

| Version | Features | Educational Value |
|---------|----------|-------------------|
| **Basic** | GetAsyncKeyState polling, file logging | Understanding key states, CPU management [citation:5] |
| **Hook** | SetWindowsHookEx, event-driven | Windows hook mechanism, DLL concepts [citation:2][citation:8] |
| **Advanced** | Window tracking, screenshots, stealth | Multi-threading, GDI+, process management [citation:1][citation:3] |

## 🚀 Compilation

### Prerequisites
- Windows OS (7/10/11)
- MinGW-w64 or Visual Studio
- Windows SDK

### Using MinGW
```bash
# Clone repository
git clone https://github.com/yourusername/cpp-keylogger
cd cpp-keylogger

# Compile all versions
compile.bat

# Or compile individually
g++ basic_keylogger.cpp -o keylogger.exe -lwinmm -mwindows



## 🎯 Key Differences from Python Version

| Aspect | Python | C++ |
|--------|--------|-----|
| **Performance** | Slower, interpreted | Faster, compiled |
| **Stealth** | Requires interpreter | Native executable [citation:5] |
| **API Access** | Wrapper libraries | Direct WinAPI [citation:2] |
| **File Size** | Large (needs Python) | Small standalone EXE |
| **Detection** | Easier to detect | Harder with proper compilation [citation:1] |

## ✅ Testing on Your Windows Machine

1. **Transfer files** using USB (as we discussed earlier)
2. **Install MinGW** if not present:
   ```powershell
   winget install mingw
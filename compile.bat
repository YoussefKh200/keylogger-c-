@echo off
echo Compiling C++ Keylogger for Windows...

:: Check for MinGW
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo MinGW not found! Please install MinGW-w64
    echo You can install via: winget install mingw
    exit /b 1
)

:: Compile basic version [citation:5]
echo Compiling basic_keylogger.cpp...
g++ basic_keylogger.cpp -o basic_keylogger.exe -lwinmm -mwindows

:: Compile hook version [citation:1]
echo Compiling hook_keylogger.cpp...
g++ hook_keylogger.cpp -o hook_keylogger.exe -luser32 -lgdi32 -mwindows

:: Compile advanced version [citation:1][citation:3]
echo Compiling advanced_keylogger.cpp...
g++ advanced_keylogger.cpp -o advanced_keylogger.exe -lgdiplus -lpsapi -luser32 -lgdi32 -mwindows

echo Done! Executables created:
dir *.exe
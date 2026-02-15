# Makefile for cross-compiling Windows keylogger
CXX = x86_64-w64-mingw32-g++
CXXFLAGS = -static -O2 -mwindows
LDFLAGS = -lgdi32 -luser32 -lgdiplus -lpsapi -lwinmm

TARGETS = basic_keylogger.exe hook_keylogger.exe advanced_keylogger.exe
SOURCES_basic = basic_keylogger.cpp
SOURCES_hook = hook_keylogger.cpp
SOURCES_advanced = advanced_keylogger.cpp

all: $(TARGETS)

basic_keylogger.exe: $(SOURCES_basic)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

hook_keylogger.exe: $(SOURCES_hook)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

advanced_keylogger.exe: $(SOURCES_advanced)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.exe *.o *.log *.txt

.PHONY: all clean
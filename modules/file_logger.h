// file_logger.h
#ifndef FILE_LOGGER_H
#define FILE_LOGGER_H

#include <fstream>
#include <string>
#include <ctime>

class FileLogger {
private:
    std::ofstream logFile;
    std::string filename;
    
public:
    FileLogger(const std::string& fname) : filename(fname) {}
    
    void Write(const std::string& data) {
        logFile.open(filename, std::ios::app);
        logFile << GetTimestamp() << " " << data;
        logFile.close();
    }
    
    std::string GetTimestamp() {
        time_t now = time(0);
        char buffer[80];
        struct tm tstruct;
        localtime_s(&tstruct, &now);
        strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &tstruct);
        return std::string(buffer);
    }
};

#endif
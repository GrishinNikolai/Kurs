#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream> // Добавлен заголовок для std::cout

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    Logger(const std::string& filename);
    ~Logger();
    
    void log(LogLevel level, const std::string& message);
    void info(const std::string& message) { log(LogLevel::INFO, message); }
    void warning(const std::string& message) { log(LogLevel::WARNING, message); }
    void error(const std::string& message) { log(LogLevel::ERROR, message); }

private:
    std::ofstream logFile;
    std::mutex logMutex;
    
    std::string getCurrentTime();
    std::string levelToString(LogLevel level);
};

#endif
#include "Logger.h"
#include <iostream>
#include <ctime>

Logger::Logger(const std::string& filename) {
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Cannot open log file: " + filename);
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::string logEntry = getCurrentTime() + " [" + levelToString(level) + "] " + message + "\n";

    logFile << logEntry;
    logFile.flush();

    // Also output to console
    std::cout << logEntry;
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;

    // Безопасная альтернатива: используем статический буфер с мьютексом
    static std::mutex time_mutex;
    std::tm tm_buffer;

    {
        std::lock_guard<std::mutex> lock(time_mutex);
        tm_buffer = *std::localtime(&time_t);
    }

    ss << std::put_time(&tm_buffer, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

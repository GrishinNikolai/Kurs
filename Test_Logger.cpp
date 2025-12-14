#include <UnitTest++/UnitTest++.h>
#include "Logger.h"
#include <fstream>
#include <sstream>

struct LoggerFixture {
    std::string testLogFile = "test_log.txt";

    LoggerFixture() {
        remove(testLogFile.c_str());
    }

    ~LoggerFixture() {
        remove(testLogFile.c_str());
    }

    std::string readLogFile() {
        std::ifstream file(testLogFile);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};

SUITE(LoggerTest)
{
    //Тест 5.1
    TEST_FIXTURE(LoggerFixture, Constructor) {
        bool success = false;
        try {
            Logger logger(testLogFile);
            success = true;
        } catch (...) {
            success = false;
        }
        CHECK(success);
    }

    //Тест 5.2
    TEST_FIXTURE(LoggerFixture, LogInfo) {
        Logger logger(testLogFile);
        logger.info("Test info message");

        std::string logContent = readLogFile();
        CHECK(logContent.find("INFO") != std::string::npos);
        CHECK(logContent.find("Test info message") != std::string::npos);
    }

    //Тест 5.3
    TEST_FIXTURE(LoggerFixture, LogWarning) {
        Logger logger(testLogFile);
        logger.warning("Test warning message");

        std::string logContent = readLogFile();
        CHECK(logContent.find("WARNING") != std::string::npos);
        CHECK(logContent.find("Test warning message") != std::string::npos);
    }

    //Тест 5.4
    TEST_FIXTURE(LoggerFixture, LogError) {
        Logger logger(testLogFile);
        logger.error("Test error message");

        std::string logContent = readLogFile();
        CHECK(logContent.find("ERROR") != std::string::npos);
        CHECK(logContent.find("Test error message") != std::string::npos);
    }

    //Тест 5.5
    TEST_FIXTURE(LoggerFixture, MultipleLogs) {
        Logger logger(testLogFile);
        logger.info("Message 1");
        logger.warning("Message 2");
        logger.error("Message 3");

        std::string logContent = readLogFile();
        CHECK(logContent.find("Message 1") != std::string::npos);
        CHECK(logContent.find("Message 2") != std::string::npos);
        CHECK(logContent.find("Message 3") != std::string::npos);
    }
}

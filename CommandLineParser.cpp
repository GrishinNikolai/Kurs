#include "CommandLineParser.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

CommandLineParser::CommandLineParser() {
    config.port = 0;
    config.clientBaseFile = "";
    config.logFile = "";

    std::ostringstream oss;
    oss << "Server usage:\n"
        << "  -b, --base <file>    Client database file (REQUIRED)\n"
        << "  -l, --log <file>     Log file (REQUIRED)\n"
        << "  -p, --port <port>    Port to listen on (REQUIRED)\n"
        << "  -h, --help           Show this help\n"
        << "Example: ./server -b clients.txt -l server.log -p 33333\n";
    helpText = oss.str();
}

bool CommandLineParser::parse(int argc, char* argv[]) {
    // Всегда сбрасываем конфиг перед парсингом
    config.port = 0;
    config.clientBaseFile = "";
    config.logFile = "";

    // Ручной парсинг аргументов
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            std::cout << helpText;
            return false;
        }
        else if (arg == "-b" || arg == "--base") {
            if (i + 1 < argc) {
                config.clientBaseFile = argv[++i];
            } else {
                std::cerr << "Error: Missing value for " << arg << std::endl;
                return false;
            }
        }
        else if (arg == "-l" || arg == "--log") {
            if (i + 1 < argc) {
                config.logFile = argv[++i];
            } else {
                std::cerr << "Error: Missing value for " << arg << std::endl;
                return false;
            }
        }
        else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                try {
                    config.port = std::stoi(argv[++i]);
                } catch (const std::exception&) {
                    std::cerr << "Error: Invalid port number" << std::endl;
                    return false;
                }
            } else {
                std::cerr << "Error: Missing value for " << arg << std::endl;
                return false;
            }
        }
        else {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            return false;
        }
    }

    // Проверяем обязательные параметры
    if (config.clientBaseFile.empty() || config.logFile.empty() || config.port == 0) {
        std::cerr << "Error: All parameters are required!\n\n";
        std::cerr << helpText;
        return false;
    }

    if (config.port < 1 || config.port > 65535) {
        std::cerr << "Error: Port must be between 1 and 65535\n";
        return false;
    }

    return true;
}

std::string CommandLineParser::getHelp() const {
    return helpText;
}

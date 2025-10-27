#include "CommandLineParser.h"
#include <iostream>
#include <getopt.h>
#include <sstream>

CommandLineParser::CommandLineParser() {
    config.port = 0;  // 0 - невалидное значение
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
    static struct option long_options[] = {
        {"base", required_argument, 0, 'b'},
        {"log", required_argument, 0, 'l'},
        {"port", required_argument, 0, 'p'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "b:l:p:h", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'b':
                config.clientBaseFile = optarg;
                break;
            case 'l':
                config.logFile = optarg;
                break;
            case 'p':
                config.port = std::stoi(optarg);
                break;
            case 'h':
                std::cout << helpText;
                return false;
            default:
                std::cerr << "Unknown option. Use -h for help.\n";
                return false;
        }
    }

    // ПРОВЕРЯЕМ, что все обязательные параметры указаны
    if (config.clientBaseFile.empty() || config.logFile.empty() || config.port == 0) {
        std::cerr << "Error: All parameters are required!\n\n";
        std::cerr << helpText;
        return false;
    }

    // Дополнительная проверка порта
    if (config.port < 1 || config.port > 65535) {
        std::cerr << "Error: Port must be between 1 and 65535\n";
        return false;
    }

    return true;
}

std::string CommandLineParser::getHelp() const {
    return helpText;
}

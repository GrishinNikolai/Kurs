#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <string>

struct ServerConfig {
    std::string clientBaseFile;
    std::string logFile;
    int port;
};

class CommandLineParser {
public:
    CommandLineParser();
    bool parse(int argc, char* argv[]);
    ServerConfig getConfig() const { return config; }
    std::string getHelp() const;

private:
    ServerConfig config;
    std::string helpText;
};

#endif
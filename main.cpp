#include "CommandLineParser.h"
#include "Server.h"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<Server> server;

void signalHandler(int signal) {
    if (server) {
        std::cout << "\nShutting down server..." << std::endl;
        server->stop();
    }
}

int main(int argc, char* argv[]) {
    // Set up signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Parse command line arguments
    CommandLineParser parser;
    if (!parser.parse(argc, argv)) {
        return 0;
    }
    
    ServerConfig config = parser.getConfig();
    
    try {
        // Create and initialize server
        server = std::make_unique<Server>(config.clientBaseFile, config.logFile, config.port);
        
        if (!server->initialize()) {
            std::cerr << "Failed to initialize server" << std::endl;
            return 1;
        }
        
        // Run server
        server->run();
        
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
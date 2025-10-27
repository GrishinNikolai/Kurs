#include "Server.h"
#include <iostream>
#include <signal.h>
#include <system_error>

Server::Server(const std::string& clientBaseFile, const std::string& logFile, int port)
    : clientBaseFile(clientBaseFile), logFile(logFile), port(port), 
      serverSocket(-1), running(false) {}

Server::~Server() {
    cleanup();
}

bool Server::initialize() {
    // Initialize client base
    clientBase = std::make_unique<ClientBase>(clientBaseFile);
    if (!clientBase->load()) {
        std::cerr << "Failed to load client database: " << clientBaseFile << std::endl;
        return false;
    }

    // ДОБАВИТЬ: проверим что база загрузилась
    std::cout << "DEBUG: Client database loaded successfully" << std::endl;
    
    // Initialize logger
    try {
        logger = std::make_unique<Logger>(logFile);
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize logger: " << e.what() << std::endl;
        return false;
    }
    
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        logger->error("Failed to create socket");
        return false;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        logger->error("Failed to set socket options");
        return false;
    }
    
    // Bind socket
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        logger->error("Failed to bind socket to port " + std::to_string(port));
        return false;
    }
    
    // Listen for connections
    if (listen(serverSocket, 10) == -1) {
        logger->error("Failed to listen on socket");
        return false;
    }
    
    logger->info("Server initialized successfully on port " + std::to_string(port));
    return true;
}

void Server::run() {
    running = true;
    logger->info("Server started and listening for connections");
    
    while (running) {
        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            if (running) {
                logger->error("Failed to accept client connection");
            }
            continue;
        }
        
        // Handle client in the same thread (single-threaded as required)
        ClientHandler handler(clientSocket, clientAddr, *clientBase, *logger);
        handler.handle();
    }
}

void Server::stop() {
    running = false;
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
    logger->info("Server stopped");
}

void Server::cleanup() {
    stop();
}

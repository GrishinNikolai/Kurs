#include "Server.h"
#include <iostream>
#include <signal.h>
#include <system_error>

/**
 * @brief Конструктор класса Server
 * @param clientBaseFile Файл базы клиентов
 * @param logFile Файл журнала
 * @param port Порт для прослушивания
 */
Server::Server(const std::string& clientBaseFile, const std::string& logFile, int port)
    : clientBaseFile(clientBaseFile), logFile(logFile), port(port), 
      serverSocket(-1), running(false) {}

/**
 * @brief Деструктор класса Server
 */
Server::~Server() {
    cleanup();
}

/**
 * @brief Инициализация сервера
 * @return true если инициализация успешна, false в противном случае
 */
bool Server::initialize() {
    // Инициализация базы клиентов
    clientBase = std::make_unique<ClientBase>(clientBaseFile);
    if (!clientBase->load()) {
        std::cerr << "Failed to load client database: " << clientBaseFile << std::endl;
        return false;
    }

    // ДОБАВИТЬ: проверим что база загрузилась
    std::cout << "DEBUG: Client database loaded successfully" << std::endl;
    
    // Инициализация Логера
    try {
        logger = std::make_unique<Logger>(logFile);
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize logger: " << e.what() << std::endl;
        return false;
    }
    
    // Создание сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        logger->error("Failed to create socket");
        return false;
    }
    
    // Установка параметров сокета
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        logger->error("Failed to set socket options");
        return false;
    }
    
    // Привязка сокета
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

/**
 * @brief Запуск основного цикла сервера
 */
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
        
        ClientHandler handler(clientSocket, clientAddr, *clientBase, *logger);
        handler.handle();
    }
}

/**
 * @brief Остановка сервера
 */
void Server::stop() {
    running = false;
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
    logger->info("Server stopped");
}

/**
 * @brief Очистка ресурсов сервера
 */
void Server::cleanup() {
    stop();
}

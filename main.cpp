#include "CommandLineParser.h"
#include "Server.h"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<Server> server; ///< Глобальный указатель на сервер для обработки сигналов

/**
 * @brief Обработчик сигналов для graceful shutdown
 * @param signal Номер сигнала
 */
void signalHandler(int signal) {
    if (server) {
        std::cout << "\nShutting down server..." << std::endl;
        server->stop();
    }
}

/**
 * @brief Главная функция сервера
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код завершения программы
 * @author Гришин Николай
 * @version 1.0
 * @date 24.11.2025
 * @copyright ИБСТ ПГУ
 */
int main(int argc, char* argv[]) {
    // Настройка обработчиковсигналов
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Разбор аргументов командной строки
    CommandLineParser parser;
    if (!parser.parse(argc, argv)) {
        return 0;
    }
    
    ServerConfig config = parser.getConfig();
    
    try {
        // Создание и инициализация сервера
        server = std::make_unique<Server>(config.clientBaseFile, config.logFile, config.port);
        
        if (!server->initialize()) {
            std::cerr << "Failed to initialize server" << std::endl;
            return 1;
        }
        
        // Запуск сервера
        server->run();
        
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

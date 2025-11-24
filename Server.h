#ifndef SERVER_H
#define SERVER_H

#include "ClientBase.h"
#include "Logger.h"
#include "ClientHandler.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <memory>
#include <thread>

/**
 * @brief Основной класс сервера
 * @author Гришин Николай
 * @version 1.0
 * @date 24.11.2025
 * @copyright ИБСТ ПГУ
 */
class Server {
public:
    /**
     * @brief Конструктор класса Server
     * @param clientBaseFile Файл базы клиентов
     * @param logFile Файл журнала
     * @param port Порт для прослушивания
     */
    Server(const std::string& clientBaseFile, const std::string& logFile, int port);
    
    /**
     * @brief Деструктор класса Server
     */
    ~Server();
    
    /**
     * @brief Инициализация сервера
     * @return true если инициализация успешна, false в противном случае
     */
    bool initialize();
    
    /**
     * @brief Запуск основного цикла сервера
     */
    void run();
    
    /**
     * @brief Остановка сервера
     */
    void stop();

private:
    std::string clientBaseFile; ///< Имя файла базы клиентов
    std::string logFile; ///< Имя файла журнала
    int port; ///< Порт сервера
    int serverSocket; ///< Сокет сервера
    bool running; ///< Флаг работы сервера
    
    std::unique_ptr<ClientBase> clientBase; ///< Указатель на базу клиентов
    std::unique_ptr<Logger> logger; ///< Указатель на логгер
    
    /**
     * @brief Очистка ресурсов сервера
     */
    void cleanup();
};

#endif

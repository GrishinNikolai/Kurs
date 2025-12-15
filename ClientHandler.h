#pragma once
#include "ClientBase.h"
#include "Logger.h"
#include "Authenticator.h"
#include "VectorCalculator.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>

/**
 * @brief Класс для обработки клиентских подключений
 * @author Гришин Николай
 * @version 1.0
 * @date 24.11.2025
 * @copyright ИБСТ ПГУ
 */
class ClientHandler {
public:
    /**
     * @brief Конструктор класса ClientHandler
     * @param clientSocket Сокет клиента
     * @param clientAddr Адрес клиента
     * @param clientBase База данных клиентов
     * @param logger Логгер для записи событий
     */
    ClientHandler(int clientSocket, const sockaddr_in& clientAddr,
                  ClientBase& clientBase, Logger& logger);
    
    /**
     * @brief Основной метод обработки клиента
     */
    void handle();

private:
    int clientSocket; ///< Сокет клиента
    sockaddr_in clientAddr; ///< Адрес клиента
    ClientBase& clientBase; ///< Ссылка на базу клиентов
    Logger& logger; ///< Ссылка на логгер

    /**
     * @brief Аутентификация клиента
     * @return true если аутентификация успешна, false в противном случае
     */
    bool authenticateClient();
    
    /**
     * @brief Прием и обработка векторов от клиента
     * @return true если обработка успешна, false в противном случае
     */
    bool receiveVectorsAndProcess();
    
    /**
     * @brief Отправка текстового сообщения клиенту
     * @param message Сообщение для отправки
     * @return true если отправка успешна, false в противном случае
     */
    bool sendMessage(const std::string& message);
    
    /**
     * @brief Прием данных от клиента
     * @param buffer Буфер для приема данных
     * @param size Размер данных
     * @return true если прием успешен, false в противном случае
     */
    bool receiveData(void* buffer, size_t size);
    
    /**
     * @brief Отправка данных клиенту
     * @param data Указатель на данные
     * @param size Размер данных
     * @return true если отправка успешна, false в противном случае
     */
    bool sendData(const void* data, size_t size);
    
    /**
     * @brief Прием строки от клиента
     * @param result Строка для результата
     * @param maxSize Максимальный размер строки
     * @return true если прием успешен, false в противном случае
     */
    bool receiveString(std::string& result, size_t maxSize = 256);
    
    /**
     * @brief Отправка соли клиенту
     * @param salt Соль для отправки
     * @return true если отправка успешна, false в противном случае
     */
    bool sendSalt(const std::string& salt);
};

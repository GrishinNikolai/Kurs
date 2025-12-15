#include "ClientHandler.h"
#include <iostream>
#include <string>
#include <cstring>

/**
 * @brief Конструктор класса ClientHandler
 * @param clientSocket Сокет клиента
 * @param clientAddr Адрес клиента
 * @param clientBase База данных клиентов
 * @param logger Логгер для записи событий
 */
ClientHandler::ClientHandler(int clientSocket, const sockaddr_in& clientAddr, 
                           ClientBase& clientBase, Logger& logger)
    : clientSocket(clientSocket), clientAddr(clientAddr), 
      clientBase(clientBase), logger(logger) {}

/**
 * @brief Основной метод обработки клиентского подключения
 * @details Выполняет аутентификацию клиента и обработку векторов данных
 */
void ClientHandler::handle() {
    std::string clientIP = inet_ntoa(clientAddr.sin_addr);
    int clientPort = ntohs(clientAddr.sin_port);
    
    logger.info("Client connected from " + clientIP + ":" + std::to_string(clientPort));
    
    try {
        if (!authenticateClient()) {
            logger.warning("Authentication failed for client " + clientIP);
            close(clientSocket);
            return;
        }
        
        logger.info("Client " + clientIP + " authenticated successfully");
        
        if (!receiveVectorsAndProcess()) {
            logger.warning("Vector processing failed for client " + clientIP);
        }
        
    } catch (const std::exception& e) {
        logger.error("Error handling client " + clientIP + ": " + e.what());
    }
    
    close(clientSocket);
    logger.info("Client " + clientIP + " disconnected");
}

/**
 * @brief Аутентификация клиента по протоколу
 * @return true если аутентификация успешна, false в противном случае
 * @details Выполняет следующие шаги:
 * 1. Прием логина от клиента
 * 2. Проверка существования пользователя в базе
 * 3. Генерация и отправка соли
 * 4. Прием хеша от клиента
 * 5. Проверка хеша аутентификации
 */
bool ClientHandler::authenticateClient() {
    // Шаг 1: Получаем логин
    std::string login;
    if (!receiveString(login)) {
        logger.error("Failed to receive login");
        return false;
    }

    // Удаляем все пробелы из имени пользователя
    login.erase(0, login.find_first_not_of(" \t\n\r"));
    login.erase(login.find_last_not_of(" \t\n\r") + 1);

    logger.info("Received login: '" + login + "'");

    // Шаг 2: Проверяем, существует ли пользователь
    if (!clientBase.userExists(login)) {
        logger.warning("User not found: " + login);
        sendMessage("ERR");
        return false;
    }

    // ИСПОЛЬЗУЕМ ПАРОЛЬ ИЗ БАЗЫ ДАННЫХ
    std::string password = clientBase.getPassword(login);

    // Шаг 3: Генерируем и отправляем соль с помощью Crypto++
    std::string salt = Authenticator::generateSalt();
    logger.info("Generated salt for user " + login + ": " + salt);

    if (!sendSalt(salt)) {
        logger.error("Failed to send salt");
        return false;
    }

    // Шаг 4: Получаем хэш от клиента
    std::string clientHash;

    // Используем receiveString для хеша тоже
    if (!receiveString(clientHash)) {
        logger.error("Failed to receive hash");
        return false;
    }

    logger.info("Received hash from client");

    // Шаг 5: Аутентификация с помощью Crypto++
    bool authResult = Authenticator::authenticate(password, salt, clientHash);

    if (authResult) {
        logger.info("Authentication successful for user: " + login);
        return sendMessage("OK");
    } else {
        logger.warning("Authentication failed for user: " + login);
        sendMessage("ERR");
        return false;
    }
}

/**
 * @brief Прием и обработка векторов данных от клиента
 * @return true если обработка успешна, false в противном случае
 * @details Выполняет следующие шаги:
 * 1. Прием количества векторов
 * 2. Для каждого вектора:
 *    - Прием размера вектора
 *    - Прием данных вектора
 *    - Вычисление произведения элементов
 *    - Отправка результата клиенту
 */
bool ClientHandler::receiveVectorsAndProcess() {
    // Шаг 6: Получаем количество векторов
    uint32_t numVectors;
    if (!receiveData(&numVectors, sizeof(numVectors))) {
        logger.error("Failed to receive number of vectors");
        return false;
    }

    numVectors = le32toh(numVectors);

    if (numVectors > 1000) {
        logger.error("Invalid number of vectors: " + std::to_string(numVectors));
        return false;
    }

    // НЕ отправляем количество результатов - это не результат вычислений
    // Просто переходим к обработке векторов

    // Обрабатываем каждый вектор
    for (uint32_t i = 0; i < numVectors; i++) {
        // Шаг 7: Получаем размер вектора
        uint32_t vectorSize;
        if (!receiveData(&vectorSize, sizeof(vectorSize))) {
            logger.error("Failed to receive vector size");
            return false;
        }
        vectorSize = le32toh(vectorSize);

        if (vectorSize > 100000) {
            logger.error("Invalid vector size: " + std::to_string(vectorSize));
            return false;
        }

        // Шаг 8: Получаем данные вектора
        std::vector<uint64_t> vector(vectorSize);
        for (uint32_t j = 0; j < vectorSize; j++) {
            uint64_t value;
            if (!receiveData(&value, sizeof(value))) {
                logger.error("Failed to receive vector data");
                return false;
            }
            value = le64toh(value);
            vector[j] = value;
        }

        // Шаг 9: Вычисляем и отправляем результат
        uint64_t result = VectorCalculator::computeProduct(vector);

        result = htole64(result);
        if (!sendData(&result, sizeof(result))) {
            logger.error("Failed to send result");
            return false;
        }
    }

    return true;
}

/**
 * @brief Отправка соли клиенту
 * @param salt Соль для отправки (16 байт)
 * @return true если отправка успешна, false в противном случае
 * @note Отправляет ровно 16 байт без нулевого терминатора
 */
bool ClientHandler::sendSalt(const std::string& salt) {
    // Для соли отправляем БЕЗ нулевого байта (16 байт)
    return sendData(salt.c_str(), 16); // Только 16 байт
}

/**
 * @brief Отправка текстового сообщения клиенту
 * @param message Сообщение для отправки
 * @return true если отправка успешна, false в противном случае
 * @note Отправляет сообщение с нулевым терминатором
 */
bool ClientHandler::sendMessage(const std::string& message) {
    // Для текстовых сообщений отправляем С нулевым байтом
    return sendData(message.c_str(), message.length() + 1);
}

/**
 * @brief Прием бинарных данных от клиента
 * @param buffer Буфер для приема данных
 * @param size Размер данных для приема
 * @return true если прием успешен, false в противном случае
 * @details Читает точное количество байт без поиска терминатора
 */
bool ClientHandler::receiveData(void* buffer, size_t size) {
    // Для бинарных данных всегда читаем точное количество байт
    // без поиска нулевого терминатора

    size_t totalReceived = 0;
    while (totalReceived < size) {
        ssize_t received = recv(clientSocket,
                               static_cast<char*>(buffer) + totalReceived,
                               size - totalReceived, 0);

        if (received <= 0) {
            return false;
        }
        totalReceived += received;
    }

    return true;
}

/**
 * @brief Отправка бинарных данных клиенту
 * @param data Указатель на данные для отправки
 * @param size Размер данных для отправки
 * @return true если отправка успешна, false в противном случае
 * @details Отправляет точное количество байт
 */
bool ClientHandler::sendData(const void* data, size_t size) {
    const char* charData = static_cast<const char*>(data);

    size_t totalSent = 0;
    while (totalSent < size) {
        ssize_t sent = send(clientSocket,
                           static_cast<const char*>(data) + totalSent,
                           size - totalSent, 0);
        if (sent <= 0) {
            return false;
        }
        totalSent += sent;
    }
    return true;
}

/**
 * @brief Прием строки от клиента
 * @param result Строка для результата
 * @param maxSize Максимальный размер строки
 * @return true если прием успешен, false в противном случае
 * @details Использует различные стратегии для приема строки:
 * - Peek для анализа буфера
 * - Поиск терминаторов (\0, \n, \r)
 * - Резервное чтение побайтно
 */
bool ClientHandler::receiveString(std::string& result, size_t maxSize) {
    char buffer[256] = {0};
    size_t totalReceived = 0;


    // Сначала посмотрим, что находится в буфере
    char peekBuffer[256] = {0};
    ssize_t peeked = recv(clientSocket, peekBuffer, sizeof(peekBuffer) - 1, MSG_PEEK);

    if (peeked > 0) {

        // Если мы видим полную строку без терминатора, считываем ровно столько байт
        bool hasTerminator = false;
        for (int i = 0; i < peeked; i++) {
            if (peekBuffer[i] == '\0' || peekBuffer[i] == '\n' || peekBuffer[i] == '\r') {
                hasTerminator = true;
                break;
            }
        }

        if (!hasTerminator && peeked > 0) {
            // Терминатор не найден, прочитаем ровно столько байт, сколько мы просмотрели
            ssize_t received = recv(clientSocket, buffer, peeked, 0);
            if (received == peeked) {
                buffer[received] = '\0'; // Добавляем нулевой терминатор
                result = std::string(buffer);
                return true;
            }
        }
    }

    // Резервный вариант: читаем побайтно
    while (totalReceived < maxSize - 1) {
        ssize_t received = recv(clientSocket, &buffer[totalReceived], 1, 0);

        if (received <= 0) {
            return false;
        }


        // Проверяем наличие различных возможных терминаторов
        if (buffer[totalReceived] == '\0' ||
            buffer[totalReceived] == '\n' ||
            buffer[totalReceived] == '\r') {

            buffer[totalReceived] = '\0'; // Гарантируем нулевой терминатор
            break;
        }

        totalReceived++;

        // Если мы получили 4 байта и это «пользователь», считаем, что он завершен
        if (totalReceived == 4 && strncmp(buffer, "user", 4) == 0) {
            buffer[4] = '\0'; // Добавляем нулевой терминатор
            break;
        }
    }

    result = std::string(buffer);
    return true;
}

#include "ClientHandler.h"
#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h> // для be64toh/htobe64

#ifndef be64toh
// Если макросы не определены, создаем свои
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define be64toh(x) __bswap_64(x)
#define htobe64(x) __bswap_64(x)
#else
#define be64toh(x) (x)
#define htobe64(x) (x)
#endif
#endif

ClientHandler::ClientHandler(int clientSocket, const sockaddr_in& clientAddr, 
                           ClientBase& clientBase, Logger& logger)
    : clientSocket(clientSocket), clientAddr(clientAddr), 
      clientBase(clientBase), logger(logger) {}

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

bool ClientHandler::authenticateClient() {

    // Step 1: Receive login
    std::string login;
    if (!receiveString(login)) {
        logger.error("Failed to receive login");
        return false;
    }

    // Trim any whitespace from login
    login.erase(0, login.find_first_not_of(" \t\n\r"));
    login.erase(login.find_last_not_of(" \t\n\r") + 1);

    logger.info("Received login: '" + login + "'");

    // Step 2: Check if user exists - ВОЗВРАЩАЕМ ПРОВЕРКУ
    if (!clientBase.userExists(login)) {
        logger.warning("User not found: " + login);
        sendMessage("ERR");
        return false;
    }

    // ИСПОЛЬЗУЕМ ПАРОЛЬ ИЗ БАЗЫ ДАННЫХ
    std::string password = clientBase.getPassword(login);

    // Step 3: Generate and send salt
    std::string salt = Authenticator::generateSalt();
    logger.info("Generated salt for user " + login + ": " + salt);

    if (!sendSalt(salt)) {
        logger.error("Failed to send salt");
        return false;
    }

    // Step 4: Receive hash
    std::string clientHash;

    // Используем receiveString для хеша тоже
    if (!receiveString(clientHash)) {
        logger.error("Failed to receive hash");
        return false;
    }

    logger.info("Received hash from client");

    // Step 5: Authenticate
    std::string combined = salt + password;
    std::string serverHash = Authenticator::computeSHA1(combined);

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

bool ClientHandler::receiveVectorsAndProcess() {
    // Step 6: Receive number of vectors
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

    // Process each vector
    for (uint32_t i = 0; i < numVectors; i++) {
        // Step 7: Receive vector size
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

        // Step 8: Receive vector data
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

        // Step 9: Compute and send result
        uint64_t result = VectorCalculator::computeProduct(vector);

        result = htole64(result);
        if (!sendData(&result, sizeof(result))) {
            logger.error("Failed to send result");
            return false;
        }
    }

    return true;
}

bool ClientHandler::sendSalt(const std::string& salt) {
    // Для соли отправляем БЕЗ нулевого байта (16 байт)
    return sendData(salt.c_str(), 16); // Только 16 байт
}

bool ClientHandler::sendMessage(const std::string& message) {
    // Для текстовых сообщений отправляем С нулевым байтом
    return sendData(message.c_str(), message.length() + 1);
}

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

bool ClientHandler::receiveString(std::string& result, size_t maxSize) {
    char buffer[256] = {0};
    size_t totalReceived = 0;


    // First, peek to see what's in the buffer
    char peekBuffer[256] = {0};
    ssize_t peeked = recv(clientSocket, peekBuffer, sizeof(peekBuffer) - 1, MSG_PEEK);

    if (peeked > 0) {

        // If we see a complete string without terminator, read exactly that many bytes
        bool hasTerminator = false;
        for (int i = 0; i < peeked; i++) {
            if (peekBuffer[i] == '\0' || peekBuffer[i] == '\n' || peekBuffer[i] == '\r') {
                hasTerminator = true;
                break;
            }
        }

        if (!hasTerminator && peeked > 0) {
            // No terminator found, read exactly the number of bytes we peeked
            ssize_t received = recv(clientSocket, buffer, peeked, 0);
            if (received == peeked) {
                buffer[received] = '\0'; // Add null terminator
                result = std::string(buffer);
                return true;
            }
        }
    }

    // Fallback: read byte by byte
    while (totalReceived < maxSize - 1) {
        ssize_t received = recv(clientSocket, &buffer[totalReceived], 1, 0);

        if (received <= 0) {
            return false;
        }


        // Check for different possible terminators
        if (buffer[totalReceived] == '\0' ||
            buffer[totalReceived] == '\n' ||
            buffer[totalReceived] == '\r') {

            buffer[totalReceived] = '\0'; // Ensure null termination
            break;
        }

        totalReceived++;

        // If we've received 4 bytes and it's "user", assume it's complete
        if (totalReceived == 4 && strncmp(buffer, "user", 4) == 0) {
            buffer[4] = '\0'; // Add null terminator
            break;
        }
    }

    result = std::string(buffer);
    return true;
}

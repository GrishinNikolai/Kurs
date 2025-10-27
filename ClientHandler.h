#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "ClientBase.h"
#include "Logger.h"
#include "Authenticator.h"
#include "VectorCalculator.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>

class ClientHandler {
public:
    ClientHandler(int clientSocket, const sockaddr_in& clientAddr,
                  ClientBase& clientBase, Logger& logger);
    void handle();

private:
    int clientSocket;
    sockaddr_in clientAddr;
    ClientBase& clientBase;
    Logger& logger;

    bool authenticateClient();
    bool receiveVectorsAndProcess();
    bool sendMessage(const std::string& message);
    bool receiveData(void* buffer, size_t size);
    bool sendData(const void* data, size_t size);
    bool receiveString(std::string& result, size_t maxSize = 256);
    bool sendSalt(const std::string& salt); // ДОБАВИТЬ этот метод
};

#endif

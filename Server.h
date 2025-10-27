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

class Server {
public:
    Server(const std::string& clientBaseFile, const std::string& logFile, int port);
    ~Server();
    
    bool initialize();
    void run();
    void stop();

private:
    std::string clientBaseFile;
    std::string logFile;
    int port;
    int serverSocket;
    bool running;
    
    std::unique_ptr<ClientBase> clientBase;
    std::unique_ptr<Logger> logger;
    
    void cleanup();
};

#endif
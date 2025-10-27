#include "ClientBase.h"
#include <fstream>
#include <sstream>
#include <iostream>

ClientBase::ClientBase(const std::string& filename) : filename(filename) {}

bool ClientBase::load() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string login = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            clients[login] = password;
        }
    }
    
    file.close();
    return true;
}

bool ClientBase::userExists(const std::string& login) const {
    return clients.find(login) != clients.end();
}

std::string ClientBase::getPassword(const std::string& login) const {
    auto it = clients.find(login);
    return (it != clients.end()) ? it->second : "";
}
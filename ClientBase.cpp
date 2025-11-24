#include "ClientBase.h"
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * @brief Конструктор класса ClientBase
 * @param filename Имя файла с базой данных клиентов
 */
ClientBase::ClientBase(const std::string& filename) : filename(filename) {}

/**
 * @brief Загрузка базы данных клиентов из файла
 * @return true если загрузка успешна, false в противном случае
 */
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

/**
 * @brief Проверка существования пользователя в базе
 * @param login Логин пользователя
 * @return true если пользователь существует, false в противном случае
 */
bool ClientBase::userExists(const std::string& login) const {
    return clients.find(login) != clients.end();
}

/**
 * @brief Получение пароля пользователя
 * @param login Логин пользователя
 * @return Пароль пользователя или пустую строку если пользователь не найден
 */
std::string ClientBase::getPassword(const std::string& login) const {
    auto it = clients.find(login);
    return (it != clients.end()) ? it->second : "";
}
#include "Authenticator.h"
#include <random>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <cstring>
#include <iostream>

/**
 * @brief Генерация случайной соли
 * @return Соль в виде строки из 16 шестнадцатеричных цифр
 */
std::string Authenticator::generateSalt() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    uint64_t salt = dis(gen);

    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << salt;
    std::string result = ss.str();

    return result;
}

/**
 * @brief Вычисление SHA1 хеша от данных
 * @param data Данные для хеширования
 * @return Хеш в виде строки шестнадцатеричных цифр
 */
std::string Authenticator::computeSHA1(const std::string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(hash[i]);
    }
    
    return ss.str();
}

/**
 * @brief Аутентификация клиента
 * @param password Пароль пользователя
 * @param salt Соль для аутентификации
 * @param clientHash Хеш от клиента
 * @return true если аутентификация успешна, false в противном случае
 */
bool Authenticator::authenticate(const std::string& password, const std::string& salt, const std::string& clientHash) {
    std::string combined = salt + password;
    std::string serverHash = computeSHA1(combined);

    bool match = true;
    for (size_t i = 0; i < serverHash.length(); i++) {
        char serverChar = std::tolower(serverHash[i]);
        char clientChar = std::tolower(clientHash[i]);

        if (serverChar != clientChar) {
            match = false;
            break;
        }
    }

    return match;
}
#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <string>
#include <random>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <algorithm>
#include <cctype>

/**
 * @brief Класс для аутентификации клиентов
 * @author Гришин Николай
 * @version 1.0
 * @date 24.11.2025
 * @copyright ИБСТ ПГУ
 */
class Authenticator {
public:
    /**
     * @brief Генерация случайной соли
     * @return Соль в виде строки из 16 шестнадцатеричных цифр
     */
    static std::string generateSalt();
    
    /**
     * @brief Вычисление SHA1 хеша от данных
     * @param data Данные для хеширования
     * @return Хеш в виде строки шестнадцатеричных цифр
     */
    static std::string computeSHA1(const std::string& data);
    
    /**
     * @brief Аутентификация клиента
     * @param password Пароль пользователя
     * @param salt Соль для аутентификации
     * @param clientHash Хеш от клиента
     * @return true если аутентификация успешна, false в противном случае
     */
    static bool authenticate(const std::string& password, const std::string& salt, const std::string& clientHash);
};

#endif

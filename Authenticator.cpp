#include "Authenticator.h"
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/filters.h>
#include <cryptopp/secblock.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>

using namespace CryptoPP;

/**
 * @brief Генерация случайной соли с помощью Crypto++
 * @return Соль в виде строки из 16 шестнадцатеричных цифр
 */
std::string Authenticator::generateSalt() {
    // Используем генератор псевдослучайных чисел из Crypto++
    AutoSeededRandomPool prng;

    // Генерируем 8 байт (64 бита) для соли
    byte saltBytes[8];
    prng.GenerateBlock(saltBytes, sizeof(saltBytes));

    // Преобразуем в шестнадцатеричную строку
    std::string saltHex;
    HexEncoder encoder;
    encoder.Attach(new StringSink(saltHex));
    encoder.Put(saltBytes, sizeof(saltBytes));
    encoder.MessageEnd();

    // Дополняем до 16 символов если нужно
    while (saltHex.length() < 16) {
        saltHex = "0" + saltHex;
    }

    // Обрезаем до 16 символов если длиннее
    if (saltHex.length() > 16) {
        saltHex = saltHex.substr(0, 16);
    }

    return saltHex;
}

/**
 * @brief Вычисление SHA1 хеша от данных с помощью Crypto++
 * @param data Данные для хеширования
 * @return Хеш в виде строки шестнадцатеричных цифр
 */
std::string Authenticator::computeSHA1(const std::string& data) {
    SHA1 hash;
    std::string digest;

    // Используем цепочку фильтров для вычисления хеша
    StringSource(data, true,
        new HashFilter(hash,
            new HexEncoder(
                new StringSink(digest))
        )
    );

    return digest;
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

    if (serverHash.empty()) {
        return false;
    }

    // Сравниваем хеши (без учета регистра)
    bool match = true;
    for (size_t i = 0; i < serverHash.length() && i < clientHash.length(); i++) {
        char serverChar = std::tolower(serverHash[i]);
        char clientChar = std::tolower(clientHash[i]);

        if (serverChar != clientChar) {
            match = false;
            break;
        }
    }

    return match;
}

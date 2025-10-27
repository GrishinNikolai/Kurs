#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <string>
#include <random>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <algorithm> // ДОБАВИТЬ этот заголовок
#include <cctype>    // ДОБАВИТЬ этот заголовок для ::tolower

class Authenticator {
public:
    static std::string generateSalt();
    static std::string computeSHA1(const std::string& data);
    static bool authenticate(const std::string& password, const std::string& salt, const std::string& clientHash);
};

#endif

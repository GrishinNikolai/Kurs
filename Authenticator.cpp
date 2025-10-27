#include "Authenticator.h"
#include <random>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <cstring> // для memcpy
#include <iostream>

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

std::string Authenticator::computeSHA1(const std::string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(hash[i]);
    }
    
    return ss.str();
}

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

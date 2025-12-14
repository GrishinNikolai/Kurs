#include <UnitTest++/UnitTest++.h>
#include "Authenticator.h"

SUITE(AuthenticatorTest)
{
    //Тест 3.1
    TEST(GenerateSalt) {
        std::string salt = Authenticator::generateSalt();
        
        CHECK_EQUAL(16, salt.length()); // 16 hex digits
        for (char c : salt) {
            CHECK(isxdigit(c)); // All characters should be hex digits
        }
    }
    
    //Тест 3.2
    TEST(ComputeSHA1) {
        std::string testData = "test123";
        std::string hash = Authenticator::computeSHA1(testData);
        
        CHECK_EQUAL(40, hash.length()); // SHA-1 produces 40 hex chars
        for (char c : hash) {
            CHECK(isxdigit(c));
        }
    }
    
    //Тест 3.3
    TEST(AuthenticationSuccess) {
        std::string password = "P@ssW0rd";
        std::string salt = "1234567890ABCDEF";
        
        std::string combined = salt + password;
        std::string clientHash = Authenticator::computeSHA1(combined);
        
        bool result = Authenticator::authenticate(password, salt, clientHash);
        CHECK(result);
    }
    
    //Тест 3.4
    TEST(AuthenticationFailure) {
        std::string password = "P@ssW0rd";
        std::string salt = "1234567890ABCDEF";
        std::string wrongHash = "1234567890abcdef1234567890abcdef";
        
        bool result = Authenticator::authenticate(password, salt, wrongHash);
        CHECK(!result);
    }
}

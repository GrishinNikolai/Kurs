#include <UnitTest++/UnitTest++.h>
#include "ClientBase.h"
#include "Authenticator.h"
#include "VectorCalculator.h"

SUITE(ServerIntegrationTest)
{
    //Тест 6.1
    TEST(FullAuthenticationFlow) {
        // Setup
        std::string login = "testuser";
        std::string password = "testpass";
        std::string salt = Authenticator::generateSalt();
        
        // Client side: compute hash
        std::string combined = salt + password;
        std::string clientHash = Authenticator::computeSHA1(combined);
        
        // Server side: verify
        bool authResult = Authenticator::authenticate(password, salt, clientHash);
        CHECK(authResult);
    }
    
    //Тест 6.2
    TEST(VectorProcessingIntegration) {
        // Test data
        std::vector<uint64_t> testVector = {2, 3, 5, 7};
        
        // Process vector
        uint64_t result = VectorCalculator::computeProduct(testVector);
        
        // Verify result
        CHECK_EQUAL(210, result); // 2*3*5*7 = 210
    }
}

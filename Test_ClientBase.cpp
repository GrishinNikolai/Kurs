#include <UnitTest++/UnitTest++.h>
#include "ClientBase.h"
#include <fstream>

struct ClientBaseFixture {
    std::string testFilename = "test_clients.txt";
    
    ClientBaseFixture() {
        // Create test file
        std::ofstream file(testFilename);
        file << "user1:pass1\n";
        file << "user2:pass2\n";
        file << "user3:pass3\n";
        file.close();
    }
    
    ~ClientBaseFixture() {
        // Remove test file
        remove(testFilename.c_str());
    }
};

SUITE(ClientBaseTest)
{
    TEST_FIXTURE(ClientBaseFixture, LoadSuccess) {
        ClientBase clientBase(testFilename);
        CHECK(clientBase.load());
    }
    
    TEST_FIXTURE(ClientBaseFixture, UserExists) {
        ClientBase clientBase(testFilename);
        clientBase.load();
        
        CHECK(clientBase.userExists("user1"));
        CHECK(clientBase.userExists("user2"));
        CHECK(!clientBase.userExists("nonexistent"));
    }
    
    TEST_FIXTURE(ClientBaseFixture, GetPassword) {
        ClientBase clientBase(testFilename);
        clientBase.load();
        
        CHECK_EQUAL("pass1", clientBase.getPassword("user1"));
        CHECK_EQUAL("pass2", clientBase.getPassword("user2"));
        CHECK_EQUAL("", clientBase.getPassword("nonexistent"));
    }
    
    TEST(LoadFailure) {
        ClientBase clientBase("nonexistent_file.txt");
        CHECK(!clientBase.load());
    }
}

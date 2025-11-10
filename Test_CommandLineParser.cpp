#include <UnitTest++/UnitTest++.h>
#include "CommandLineParser.h"
#include <sstream>
#include <cstring>
#include <iostream>

class OutputRedirect {
    std::stringstream buffer;
    std::streambuf* old_cout;
    std::streambuf* old_cerr;

public:
    OutputRedirect() {
        old_cout = std::cout.rdbuf(buffer.rdbuf());
        old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    }

    ~OutputRedirect() {
        std::cout.rdbuf(old_cout);
        std::cerr.rdbuf(old_cerr);
    }

    std::string getOutput() const {
        return buffer.str();
    }

    void clear() {
        buffer.str("");
        buffer.clear();
    }
};

SUITE(CommandLineParserTest)
{
    TEST(ValidParameters) {
        OutputRedirect redirect;
        CommandLineParser parser;

        const char* argv[] = {"server", "-b", "clients.txt", "-l", "server.log", "-p", "33333", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        CHECK(parser.parse(argc, const_cast<char**>(argv)));

        ServerConfig config = parser.getConfig();
        CHECK_EQUAL("clients.txt", config.clientBaseFile);
        CHECK_EQUAL("server.log", config.logFile);
        CHECK_EQUAL(33333, config.port);
    }

    TEST(HelpParameter) {
        OutputRedirect redirect;
        CommandLineParser parser;

        const char* argv[] = {"server", "-h", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        // parse должен вернуть false для -h, но без исключений
        bool result = parser.parse(argc, const_cast<char**>(argv));
        CHECK(!result);

        // Проверяем что вывод содержит справку
        std::string output = redirect.getOutput();
        CHECK(output.find("Server usage") != std::string::npos);
    }

    TEST(MissingRequiredParameters) {
        OutputRedirect redirect;
        CommandLineParser parser;

        const char* argv[] = {"server", "-b", "clients.txt", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        bool result = parser.parse(argc, const_cast<char**>(argv));
        CHECK(!result);

        std::string output = redirect.getOutput();
        CHECK(output.find("Error: All parameters are required") != std::string::npos);
    }

    TEST(InvalidPort) {
        OutputRedirect redirect;
        CommandLineParser parser;

        const char* argv[] = {"server", "-b", "clients.txt", "-l", "server.log", "-p", "70000", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        bool result = parser.parse(argc, const_cast<char**>(argv));
        CHECK(!result);

        std::string output = redirect.getOutput();
        CHECK(output.find("Port must be between 1 and 65535") != std::string::npos);
    }

    TEST(LongOptions) {
        OutputRedirect redirect;
        CommandLineParser parser;

        const char* argv[] = {"server", "--base", "clients.txt", "--log", "server.log", "--port", "44444", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        CHECK(parser.parse(argc, const_cast<char**>(argv)));

        ServerConfig config = parser.getConfig();
        CHECK_EQUAL("clients.txt", config.clientBaseFile);
        CHECK_EQUAL("server.log", config.logFile);
        CHECK_EQUAL(44444, config.port);
    }

    TEST(MixedOptions) {
        OutputRedirect redirect;
        CommandLineParser parser;

        const char* argv[] = {"server", "-b", "clients.txt", "--log", "server.log", "-p", "55555", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        CHECK(parser.parse(argc, const_cast<char**>(argv)));

        ServerConfig config = parser.getConfig();
        CHECK_EQUAL("clients.txt", config.clientBaseFile);
        CHECK_EQUAL("server.log", config.logFile);
        CHECK_EQUAL(55555, config.port);
    }

    TEST(InvalidPortFormat) {
        OutputRedirect redirect;
        CommandLineParser parser;

        const char* argv[] = {"server", "-b", "clients.txt", "-l", "server.log", "-p", "not_a_number", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;

        bool result = parser.parse(argc, const_cast<char**>(argv));
        CHECK(!result);

        std::string output = redirect.getOutput();
        CHECK(output.find("Invalid port number") != std::string::npos);
    }
}

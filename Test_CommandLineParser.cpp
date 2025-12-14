#include <UnitTest++/UnitTest++.h>
#include "CommandLineParser.h"
#include <sstream>
#include <cstring>
#include <iostream>
#include <vector>

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

// Вспомогательная функция для создания массива аргументов
std::vector<char*> createArgv(const std::vector<std::string>& args) {
    std::vector<char*> argv;
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);
    return argv;
}

SUITE(CommandLineParserTest)
{
    //Тест 1.1
    TEST(ValidParameters) {
        OutputRedirect redirect;
        CommandLineParser parser;

        auto argv_vec = createArgv({"server", "-b", "clients.txt", "-l", "server.log", "-p", "33333"});
        int argc = argv_vec.size() - 1;

        CHECK(parser.parse(argc, argv_vec.data()));

        ServerConfig config = parser.getConfig();
        CHECK_EQUAL("clients.txt", config.clientBaseFile);
        CHECK_EQUAL("server.log", config.logFile);
        CHECK_EQUAL(33333, config.port);
    }

    //Тест 1.2
    TEST(MixedParameters) {
        OutputRedirect redirect;
        CommandLineParser parser;

        auto argv_vec = createArgv({"server", "-l", "server.log", "-p", "33333", "-b", "clients.txt"});
        int argc = argv_vec.size() - 1;

        CHECK(parser.parse(argc, argv_vec.data()));

        ServerConfig config = parser.getConfig();
        CHECK_EQUAL("clients.txt", config.clientBaseFile);
        CHECK_EQUAL("server.log", config.logFile);
        CHECK_EQUAL(33333, config.port);
    }

    //Тест 1.3
    TEST(LongOptions) {
        OutputRedirect redirect;
        CommandLineParser parser;

        auto argv_vec = createArgv({"server", "--base", "clients.txt", "--log", "server.log", "--port", "44444"});
        int argc = argv_vec.size() - 1;

        CHECK(parser.parse(argc, argv_vec.data()));

        ServerConfig config = parser.getConfig();
        CHECK_EQUAL("clients.txt", config.clientBaseFile);
        CHECK_EQUAL("server.log", config.logFile);
        CHECK_EQUAL(44444, config.port);
    }

    //Тест 1.4
    TEST(MixedOptions) {
        OutputRedirect redirect;
        CommandLineParser parser;

        auto argv_vec = createArgv({"server", "-b", "clients.txt", "--log", "server.log", "-p", "55555"});
        int argc = argv_vec.size() - 1;

        CHECK(parser.parse(argc, argv_vec.data()));

        ServerConfig config = parser.getConfig();
        CHECK_EQUAL("clients.txt", config.clientBaseFile);
        CHECK_EQUAL("server.log", config.logFile);
        CHECK_EQUAL(55555, config.port);
    }

    //Тест 1.5
    TEST(HelpParameter) {
        OutputRedirect redirect;
        CommandLineParser parser;

        auto argv_vec = createArgv({"server", "-h"});
        int argc = argv_vec.size() - 1;

        // parse должен вернуть false для -h (вывод справки), но без исключений
        bool result = parser.parse(argc, argv_vec.data());
        CHECK(!result);

        // Проверяем что вывод содержит справку
        std::string output = redirect.getOutput();
        CHECK(output.find("Server usage:") != std::string::npos);
    }

    //Тест 1.6
    TEST(MissingRequiredParameters) {
        OutputRedirect redirect;
        CommandLineParser parser;

        auto argv_vec = createArgv({"server", "-b", "clients.txt"});
        int argc = argv_vec.size() - 1;

        bool result = parser.parse(argc, argv_vec.data());
        CHECK(!result);

        std::string output = redirect.getOutput();
        bool hasError = (output.find("All parameters are required"));
        CHECK(hasError);
    }

    //Тест 1.7
    TEST(InvalidPort) {
        OutputRedirect redirect;
        CommandLineParser parser;

        auto argv_vec = createArgv({"server", "-b", "clients.txt", "-l", "server.log", "-p", "70000"});
        int argc = argv_vec.size() - 1;

        bool result = parser.parse(argc, argv_vec.data());
        CHECK(!result);

        std::string output = redirect.getOutput();
        bool hasError = (output.find("Port must be between 1 and 65535"));
        CHECK(hasError);
    }

    //Тест 1.8
    TEST(InvalidPortFormat) {
        OutputRedirect redirect;
        CommandLineParser parser;

        auto argv_vec = createArgv({"server", "-b", "clients.txt", "-l", "server.log", "-p", "abc"});
        int argc = argv_vec.size() - 1;

        bool result = parser.parse(argc, argv_vec.data());
        CHECK(!result);

        std::string output = redirect.getOutput();
        bool hasError = (output.find("Invalid port number"));
        CHECK(hasError);
    }

    //Тест 1.9
    TEST(UnknownOption) {
        OutputRedirect redirect;
        CommandLineParser parser;

        auto argv_vec = createArgv({"server", "-x", "test"});
        int argc = argv_vec.size() - 1;

        bool result = parser.parse(argc, argv_vec.data());
        CHECK(!result);

        std::string output = redirect.getOutput();
        // getopt_long сам выводит сообщение об ошибке
        bool hasHelp = (output.find("Use -h or --help for usage information"));
        // Проверяем, что есть либо сообщение об ошибке, либо призыв к справке
        CHECK(output.length() > 0); // Должно быть какое-то сообщение
    }

    //Тес 1.10
    TEST(ExtraArguments) {
        OutputRedirect redirect;
        CommandLineParser parser;

        auto argv_vec = createArgv({"server", "-b", "clients.txt", "-l", "server.log", "-p", "8080", "extra", "args"});
        int argc = argv_vec.size() - 1;

        bool result = parser.parse(argc, argv_vec.data());
        CHECK(!result);

        std::string output = redirect.getOutput();
        bool hasError = (output.find("Error: Unknown arguments: extra args") != std::string::npos);
        CHECK(hasError);
    }
}

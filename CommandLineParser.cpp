#include "CommandLineParser.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <getopt.h>
#include <cstdlib>

using namespace std;

/**
 * @brief Конструктор класса CommandLineParser
 */
CommandLineParser::CommandLineParser() {
    config.port = 0;
    config.clientBaseFile = "";
    config.logFile = "";

    std::ostringstream oss;
    oss << "Server usage:\n"
        << "  -b, --base <file>    Client database file (REQUIRED)\n"
        << "  -l, --log <file>     Log file (REQUIRED)\n"
        << "  -p, --port <port>    Port to listen on (REQUIRED)\n"
        << "  -h, --help           Show this help\n"
        << "Example: ./server -b clients.txt -l server.log -p 33333\n";
    helpText = oss.str();
}

/**
 * @brief Проверка обязательных параметров
 * @return true если все обязательные параметры заданы, false в противном случае
 */
bool CommandLineParser::validateRequiredParams() const {
    if (config.clientBaseFile.empty()) {
        cerr << "Error: Client database file is required (-b, --base)" << endl;
        return false;
    }

    if (config.logFile.empty()) {
        cerr << "Error: Log file is required (-l, --log)" << endl;
        return false;
    }

    if (config.port == 0) {
        cerr << "Error: Port is required (-p, --port)" << endl;
        return false;
    }

    return true;
}

/**
 * @brief Разбор аргументов командной строки с использованием getopt_long
 * @param argc Количество аргументов
 * @param argv Массив аргументов
 * @return true если разбор успешен, false в противном случае
 */
bool CommandLineParser::parse(int argc, char* argv[]) {
    // Сбрасываем конфиг перед парсингом
    config.port = 0;
    config.clientBaseFile = "";
    config.logFile = "";

    // Сбрасываем optind для повторных вызовов getopt_long
    optind = 1;

    // Структура для длинных опций getopt_long
    static struct option long_options[] = {
        {"base",    required_argument, 0, 'b'},   // Файл базы клиентов
        {"log",     required_argument, 0, 'l'},   // Файл журнала
        {"port",    required_argument, 0, 'p'},   // Порт сервера
        {"help",    no_argument,       0, 'h'},   // Справка
        {0, 0, 0, 0}                             // Конец массива
    };

    // Строка коротких опций для getopt
    const char* short_options = "b:l:p:h";

    // Разбор опций с помощью getopt_long
    int opt;
    int option_index = 0;

    // Используем цикл для обработки всех опций
    while ((opt = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (opt) {
            case 'b': // Файл базы клиентов
                config.clientBaseFile = optarg;
                break;

            case 'l': // Файл журнала
                config.logFile = optarg;
                break;

            case 'p': // Порт сервера
                try {
                    config.port = stoi(string(optarg));

                    // Проверяем допустимость порта
                    if (config.port < 1 || config.port > 65535) {
                        cerr << "Error: Port must be between 1 and 65535" << endl;
                        return false;
                    }
                } catch (const exception&) {
                    cerr << "Error: Invalid port number: " << optarg << endl;
                    return false;
                }
                break;

            case 'h': // Справка
                cout << helpText;
                return false; // Завершаем выполнение после вывода справки

            case '?': // Неизвестная опция
                // getopt_long уже выводит сообщение об ошибке
                cerr << "\nUse -h or --help for usage information" << endl;
                return false;

            case ':': // Отсутствует аргумент для опции
                cerr << "Error: Missing argument for option '"
                     << static_cast<char>(optopt) << "'" << endl;
                cerr << "Use -h or --help for usage information" << endl;
                return false;

            default:
                cerr << "Error: Unknown error parsing parameters" << endl;
                return false;
        }
    }

    // Проверяем наличие лишних аргументов (не опций)
    if (optind < argc) {
        cerr << "Error: Unknown arguments: ";
        for (int i = optind; i < argc; i++) {
            cerr << argv[i] << " ";
        }
        cerr << "\nUse -h or --help for usage information" << endl;
        return false;
    }

    // Проверяем обязательные параметры
    if (!validateRequiredParams()) {
        cerr << "Use -h or --help for usage information" << endl;
        return false;
    }

    return true;
}

/**
 * @brief Получение текста справки
 * @return Строка с текстом справки
 */
std::string CommandLineParser::getHelp() const {
    return helpText;
}

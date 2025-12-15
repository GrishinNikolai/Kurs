#pragma once
#include <string>

/**
 * @brief Структура для хранения конфигурации сервера
 */
struct ServerConfig {
    std::string clientBaseFile; ///< Файл базы клиентов
    std::string logFile; ///< Файл журнала
    int port; ///< Порт сервера
};

/**
 * @brief Класс для разбора командной строки с использованием getopt/getopt_long
 * @author Гришин Николай
 * @version 1.0
 * @date 24.11.2025
 * @copyright ИБСТ ПГУ
 */
class CommandLineParser {
public:
    /**
     * @brief Конструктор класса CommandLineParser
     */
    CommandLineParser();

    /**
     * @brief Разбор аргументов командной строки с использованием getopt_long
     * @param argc Количество аргументов
     * @param argv Массив аргументов
     * @return true если разбор успешен, false в противном случае
     */
    bool parse(int argc, char* argv[]);

    /**
     * @brief Получение конфигурации сервера
     * @return Структура ServerConfig с настройками
     */
    ServerConfig getConfig() const { return config; }

    /**
     * @brief Получение текста справки
     * @return Строка с текстом справки
     */
    std::string getHelp() const;

private:
    ServerConfig config; ///< Конфигурация сервера
    std::string helpText; ///< Текст справки

    /**
     * @brief Проверка обязательных параметров
     * @return true если все обязательные параметры заданы, false в противном случае
     */
    bool validateRequiredParams() const;
};

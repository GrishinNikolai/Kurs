#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

/**
 * @brief Перечисление уровней логирования
 */
enum class LogLevel {
    INFO,     ///< Информационное сообщение
    WARNING,  ///< Предупреждение
    ERROR     ///< Ошибка
};

/**
 * @brief Класс для логирования событий сервера
 * @author Гришин Николай
 * @version 1.0
 * @date 24.11.2025
 * @copyright ИБСТ ПГУ
 */
class Logger {
public:
    /**
     * @brief Конструктор класса Logger
     * @param filename Имя файла для логирования
     * @throw std::runtime_error если файл не может быть открыт
     */
    Logger(const std::string& filename);
    
    /**
     * @brief Деструктор класса Logger
     */
    ~Logger();
    
    /**
     * @brief Основной метод логирования
     * @param level Уровень логирования
     * @param message Сообщение для записи
     */
    void log(LogLevel level, const std::string& message);
    
    /**
     * @brief Логирование информационного сообщения
     * @param message Сообщение для записи
     */
    void info(const std::string& message) { log(LogLevel::INFO, message); }
    
    /**
     * @brief Логирование предупреждения
     * @param message Сообщение для записи
     */
    void warning(const std::string& message) { log(LogLevel::WARNING, message); }
    
    /**
     * @brief Логирование ошибки
     * @param message Сообщение для записи
     */
    void error(const std::string& message) { log(LogLevel::ERROR, message); }

private:
    std::ofstream logFile; ///< Файл для записи логов
    std::mutex logMutex; ///< Мьютекс для синхронизации доступа к файлу
    
    /**
     * @brief Получение текущего времени в формате строки
     * @return Строка с текущим временем
     */
    std::string getCurrentTime();
    
    /**
     * @brief Преобразование уровня логирования в строку
     * @param level Уровень логирования
     * @return Строковое представление уровня
     */
    std::string levelToString(LogLevel level);
};

#pragma once
#include <string>
#include <unordered_map>

/**
 * @brief Класс для работы с базой данных клиентов
 * @author Гришин Николай
 * @version 1.0
 * @date 24.11.2025
 * @copyright ИБСТ ПГУ
 */
class ClientBase {
public:
    /**
     * @brief Конструктор класса ClientBase
     * @param filename Имя файла с базой данных клиентов
     */
    ClientBase(const std::string& filename);
    
    /**
     * @brief Загрузка базы данных клиентов из файла
     * @return true если загрузка успешна, false в противном случае
     */
    bool load();
    
    /**
     * @brief Проверка существования пользователя в базе
     * @param login Логин пользователя
     * @return true если пользователь существует, false в противном случае
     */
    bool userExists(const std::string& login) const;
    
    /**
     * @brief Получение пароля пользователя
     * @param login Логин пользователя
     * @return Пароль пользователя или пустую строку если пользователь не найден
     */
    std::string getPassword(const std::string& login) const;

private:
    std::string filename; ///< Имя файла с базой данных
    std::unordered_map<std::string, std::string> clients; ///< Контейнер для хранения клиентов
};

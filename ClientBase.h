#ifndef CLIENTBASE_H
#define CLIENTBASE_H

#include <string>
#include <unordered_map>

class ClientBase {
public:
    ClientBase(const std::string& filename);
    
    bool load();
    bool userExists(const std::string& login) const;
    std::string getPassword(const std::string& login) const;

private:
    std::string filename;
    std::unordered_map<std::string, std::string> clients;
};

#endif
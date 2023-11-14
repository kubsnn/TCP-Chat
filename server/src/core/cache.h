#pragma once
#ifndef __CACHE_H__
#define __CACHE_H__

#include "client.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <shared_mutex>

// Thread-safe Cache class
class Cache {
public:
    Cache() = default;

    bool isUserOnline(const std::string& username);

    void addUserOnline(Client client);

    void removeUserOnline(const std::string& username);

    Client getUser(const std::string& username);

    std::vector<Client> usersOnline();
 
private:
    std::unordered_map<std::string, Client> users_online_;
    std::shared_mutex users_online_mutex_;
};

#endif //!__CACHE_H__



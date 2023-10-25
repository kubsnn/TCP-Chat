#include "cache.h"

bool Cache::isUserOnline(const std::string& username) {
    std::shared_lock<std::shared_mutex> lock(users_online_mutex_);
    return users_online_.contains(username);
}

void Cache::addUserOnline(Client client) {
    std::unique_lock<std::shared_mutex> lock(users_online_mutex_);
    users_online_.insert({client.username(), client});
}

void Cache::removeUserOnline(const std::string& username) {
    std::unique_lock<std::shared_mutex> lock(users_online_mutex_);
    users_online_.erase(username);
}

Client Cache::getUser(const std::string& username) {
    std::shared_lock<std::shared_mutex> lock(users_online_mutex_);
    if (users_online_.contains(username)) {
        return users_online_.at(username);
    }
    return Client(-1, sockaddr_in{});
}

std::vector<Client> Cache::usersOnline() {
    std::vector<Client> users;
    std::shared_lock<std::shared_mutex> lock(users_online_mutex_);
    users.reserve(users_online_.size());

    for (const auto& [_, client] : users_online_) {
        users.push_back(client);
    }

    return users;
}

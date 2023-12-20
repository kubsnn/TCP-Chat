#include "user.h"

const std::vector<User> User::empty_ = std::vector<User>();

User::User(std::string username, std::string pswd_hash, std::string salt, int id, std::vector<User> friends)
    : id_(id), username_(std::move(username)), password_hash_(std::move(pswd_hash)), salt_(std::move(salt))
    , friends_(std::make_unique<std::vector<User>>(std::move(friends)))
{ }

User::User(std::string username, std::string pswd_hash, std::string salt, int id)
    : id_(id), username_(std::move(username))
    , password_hash_(std::move(pswd_hash)), salt_(std::move(salt))
{ }

int User::id() const {
    return id_;
}

const std::string& User::username() const {
    return username_;
}

const std::string& User::passwordHash() const {
    return password_hash_;
}

const std::string& User::salt() const {
    return salt_;
}

const std::vector<User>& User::friends() const {
    if (friends_ == nullptr) {
        return empty_;
    }
    return *friends_;
}

bool User::friendsIncluded() const {
    return friends_ != nullptr;
}

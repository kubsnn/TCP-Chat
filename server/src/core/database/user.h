#pragma once
#ifndef __USER_H__
#define __USER_H__

#include <string>
#include <vector>
#include <memory>

class User {
    static const std::vector<User> empty_;
public:
    User(std::string username, std::string pswd_hash, int id, std::vector<User> friends);
    User(std::string username, std::string pswd_hash, int id = -1);
    int id() const;
    const std::string& username() const;
    const std::string& passwordHash() const;
    const std::vector<User>& friends() const;
    bool friendsIncluded() const;

private:
    int id_;
    std::string username_;
    std::string password_hash_;
    std::unique_ptr<std::vector<User>> friends_ = nullptr;
};

#endif //!__USER_H__
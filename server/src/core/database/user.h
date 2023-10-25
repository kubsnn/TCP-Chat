#pragma once
#ifndef __USER_H__
#define __USER_H__

#include <string>
#include <vector>

class User {
public:
    User(std::string username, std::string pswd_hash, int id = -1);
    int id() const;
    const std::string& username() const;
    const std::string& passwordHash() const;
private:
    int id_;
    std::string username_;
    std::string password_hash_;
};

#endif //!__USER_H__
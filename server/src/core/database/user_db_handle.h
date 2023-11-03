#pragma once
#ifndef __USER_DATABASE_H__
#define __USER_DATABASE_H__

#include "user.h"
#include "../utils/globals.h"
#include <sqlite/sqlite3.h>

void configure_databases();

class UserDbHandle {
public:
    inline static const auto database_path = executable_dir / "users.db";

    UserDbHandle();
    UserDbHandle(const UserDbHandle&) = delete;
    UserDbHandle(UserDbHandle&& other) noexcept;
    ~UserDbHandle();
    UserDbHandle& operator=(const UserDbHandle&) = delete;
    UserDbHandle& operator=(UserDbHandle&& other) noexcept;

    bool add(const User& user) const;
    bool exists(const std::string& username) const;
    const User getById(int id) const;
    const User getByName(const std::string& username) const;
    bool verify(const User& credentials) const;

    void drop() const; 
private:
    void tryInitializeTables();

    sqlite3* db_;
};


#endif //!__USER_DATABASE_H__
#pragma once
#ifndef __USER_DATABASE_H__
#define __USER_DATABASE_H__

#include "user.h"
#include <sqlite/sqlite3.h>

void configure_databases();

class UserDatabase {
public:
    inline static std::string database_name = "users.db";

    UserDatabase();
    UserDatabase(const UserDatabase&) = delete;
    UserDatabase(UserDatabase&& other) noexcept;
    ~UserDatabase();
    UserDatabase& operator=(const UserDatabase&) = delete;
    UserDatabase& operator=(UserDatabase&& other) noexcept;

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
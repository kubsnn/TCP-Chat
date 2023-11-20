#pragma once
#ifndef __USER_DATABASE_H__
#define __USER_DATABASE_H__

#include "user.h"
#include "../utils/globals.h"
#include <sqlite3.h>
#include <vector>

void configure_databases();

class UserDbHandle {
public:
    inline static const auto database_path = globals::executable_dir / "users.db";

    UserDbHandle();
    UserDbHandle(const UserDbHandle&) = delete;
    UserDbHandle(UserDbHandle&& other) noexcept;
    ~UserDbHandle();
    UserDbHandle& operator=(const UserDbHandle&) = delete;
    UserDbHandle& operator=(UserDbHandle&& other) noexcept;

    bool add(const User& user) const;
    bool exists(const std::string& username) const;
    const User getById(int id) const;
    const User getById(int id, bool include_friends) const;
    const User getByName(const std::string& username) const;
    std::vector<User> getByNameLike(const std::string& username) const;
    const User getByName(const std::string& username, bool include_friends) const;
    bool verify(const User& credentials) const;

    bool addFriend(int id, const std::string& friend_name) const;
    bool acceptInvitation(int id, const std::string& friend_name) const;
    std::vector<User> getFriends(int id) const;
    std::vector<User> getInvitations(int id) const;

    void drop() const; 
private:
    bool isInvitedBy(int id, int friend_id) const;
    void addFriendForce(int id, int friend_id) const;

    void tryInitializeTables();

    sqlite3* db_;
};


#endif //!__USER_DATABASE_H__
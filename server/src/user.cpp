#include "user.h"

User::User(std::string username, std::string pswd_hash, int id)
    : id_(id)
    , username_(std::move(username))
    , password_hash_(std::move(pswd_hash))
{ }

int User::id() const
{
    return id_;
}

const std::string &User::username() const
{
    return username_;
}

const std::string& User::passwordHash() const
{
    return password_hash_;
}

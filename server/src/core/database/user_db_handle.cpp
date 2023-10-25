#include "user_db_handle.h"
#include <stdexcept>

UserDbHandle::UserDbHandle()
    : db_(nullptr) 
{
    if (sqlite3_open(database_name.c_str(), &db_) != SQLITE_OK) {
        throw std::runtime_error("cannot open database");
    }

    tryInitializeTables();
}

UserDbHandle::UserDbHandle(UserDbHandle&& other) noexcept {
    db_ = other.db_;
    other.db_ = nullptr;
}

UserDbHandle::~UserDbHandle() {
    if (db_ == nullptr) return;

    sqlite3_close(db_);
    db_ = nullptr;
}

UserDbHandle& UserDbHandle::operator=(UserDbHandle&& other) noexcept {
    if (this == &other) return *this;
    db_ = other.db_;
    other.db_ = nullptr;

    return *this;
}

bool UserDbHandle::add(const User& user) const {
    static constexpr auto query = "INSERT INTO USERS (USERNAME, PASSWORDHASH) VALUES (?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot prepare insert new user query");
    }

    sqlite3_bind_text(stmt, 1, user.username().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.passwordHash().c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        return false;
    }
    return true;
}

bool UserDbHandle::exists(const std::string& username) const {
    static constexpr auto query = "SELECT COUNT(*) FROM USERS WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot check if users exist");
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return (count > 0); // User exists if count is greater than 0
    }

    sqlite3_finalize(stmt);

    return false;
}

const User UserDbHandle::getById(int id) const {
    static constexpr auto query = "SELECT ID, USERNAME, PASSWORDHASH FROM USERS WHERE ID = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot run query; getById");
    }
    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
        auto username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        auto password_hash = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));

        sqlite3_finalize(stmt);
        return User(std::move(username), std::move(password_hash), id);
    }

    sqlite3_finalize(stmt);
    return User("", "", -1);
}

const User UserDbHandle::getByName(const std::string &username) const {
    static constexpr auto query = "SELECT ID, USERNAME, PASSWORDHASH FROM USERS WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw std::runtime_error("cannot run query; getByName");
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        auto id = sqlite3_column_int(stmt, 0);
        auto username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        auto password_hash = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        User user = {username, password_hash, id};
        sqlite3_finalize(stmt);
        return user;
    }

    sqlite3_finalize(stmt);
    throw std::runtime_error("user does not exist");

    return {"", "", -1};
}

bool UserDbHandle::verify(const User &credentials) const
{
    return false;
}

void UserDbHandle::drop() const {
    static constexpr auto query = "DROP TABLE IF EXISTS USERS;";
    if (sqlite3_exec(db_, query, nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot drop users table");
    }
}

void UserDbHandle::tryInitializeTables() {
    static constexpr auto query = "CREATE TABLE IF NOT EXISTS USERS (\
ID INTEGER PRIMARY KEY AUTOINCREMENT,\
USERNAME TEXT UNIQUE NOT NULL,\
PASSWORDHASH TEXT NOT NULL);";

    if (sqlite3_exec(db_, query, nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot create table");
    }
}


void configure_databases() {
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
}

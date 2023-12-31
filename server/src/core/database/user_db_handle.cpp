#include "user_db_handle.h"
#include "../utils/crypto.h"
#include <stdexcept>

UserDbHandle::UserDbHandle()
    : db_(nullptr) 
{
    if (sqlite3_open(database_path.c_str(), &db_) != SQLITE_OK) {
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
    static constexpr auto query = "INSERT INTO users (username, passwordhash, salt) VALUES (?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot prepare insert new user query");
    }

    sqlite3_bind_text(stmt, 1, user.username().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.passwordHash().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user.salt().c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        return false;
    }
    return true;
}

bool UserDbHandle::exists(const std::string& username) const {
    static constexpr auto query = "SELECT COUNT(*) FROM users WHERE username = ?;";
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
    return getById(id, false);
}

const User UserDbHandle::getById(int id, bool include_friends) const {
    static constexpr auto query = "SELECT id, username, passwordhash, salt FROM users WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot run query; getById");
    }
    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
        auto username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        auto password_hash = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        auto salt = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));

        sqlite3_finalize(stmt);
        if (!include_friends) {
            return User(std::move(username), std::move(password_hash), std::move(salt), id);
        }
        return User(std::move(username), std::move(password_hash), std::move(salt), id, getFriends(id));
    }

    sqlite3_finalize(stmt);
    return User("", "", "", -1);
}

const User UserDbHandle::getByName(const std::string& username) const {
    return getByName(username, false);
}

std::vector<User> UserDbHandle::getByNameLike(const std::string& username) const {
    static constexpr auto query = "SELECT id, username FROM users WHERE username LIKE ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot run query; getByNameLike");
    }
    sqlite3_bind_text(stmt, 1, (username + "%").c_str(), -1, SQLITE_STATIC);

    std::vector<User> users;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto id = sqlite3_column_int(stmt, 0);
        auto username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        users.emplace_back(std::move(username), "", "", id);
    }

    sqlite3_finalize(stmt);

    return users;
}

const User UserDbHandle::getByName(const std::string& username, bool include_friends) const {
    static constexpr auto query = "SELECT id, username, passwordhash, salt FROM users WHERE username = ?;";
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
        auto salt = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));

        sqlite3_finalize(stmt);
        if (!include_friends) {
            return User(std::move(username), std::move(password_hash), std::move(salt), id);
        }
        return User(std::move(username), std::move(password_hash), std::move(salt), id, getFriends(id));
    }

    sqlite3_finalize(stmt);
    throw std::runtime_error("user does not exist");

    return {"", "", "", -1};
}

bool UserDbHandle::verify(const User& credentials) const
{
    return false;
}

bool UserDbHandle::addFriend(int who_id, const std::string& friend_name) const {
    static constexpr auto query = "INSERT INTO friends (user_id, friend_id, status) VALUES (?, ?, 0);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot prepare insert new friend query");
    }

    sqlite3_bind_int(stmt, 1, who_id);
    sqlite3_bind_int(stmt, 2, getByName(friend_name).id());

    int rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        return false;
    }
    return true;
}

bool UserDbHandle::acceptInvitation(int id, const std::string& friend_name) const {
    int friend_id = getByName(friend_name).id();
    if (!isInvitedBy(id, friend_id)) {
        return false;
    }
    
    static constexpr auto query = "UPDATE friends SET status = 1 WHERE user_id = ? AND friend_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot prepare accept invitation query");
    }
    sqlite3_bind_int(stmt, 1, friend_id);
    sqlite3_bind_int(stmt, 2, id);

    int rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        return false;
    }

    addFriendForce(id, friend_id);

    return true;
}

bool UserDbHandle::removeFriend(int id, const std::string& friend_name) const {
    int friend_id = getByName(friend_name).id();
    
    removeFriendForce(id, friend_id);
    removeFriendForce(friend_id, id);

    return true;
}


bool UserDbHandle::isFriend(const std::string& username, const std::string& friend_name) const {
    static constexpr auto query = "SELECT COUNT(*) FROM friends JOIN users ON friend_id = id WHERE user_id = ? AND username = ? AND status = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot prepare is friend query");
    }

    sqlite3_bind_int(stmt, 1, getByName(username).id());
    sqlite3_bind_text(stmt, 2, friend_name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return (count > 0); // User exists if count is greater than 0
    }

    sqlite3_finalize(stmt);

    return false;
}

std::vector<User> UserDbHandle::getFriends(int id) const {
    static constexpr auto query = "SELECT friend_id, username FROM friends JOIN users ON friend_id = id WHERE user_id = ? AND status = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot prepare get friends query");
    }

    sqlite3_bind_int(stmt, 1, id);

    std::vector<User> friends;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto friend_id = sqlite3_column_int(stmt, 0);
        auto friend_name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        friends.emplace_back(friend_name, "", "", friend_id);
    }

    sqlite3_finalize(stmt);

    return friends;
}

std::vector<User> UserDbHandle::getInvitations(int id) const {
    static constexpr auto query = "SELECT user_id, username FROM friends JOIN users ON user_id = id WHERE friend_id = ? AND status = 0;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot prepare get invitations query");
    }

    sqlite3_bind_int(stmt, 1, id);

    std::vector<User> invitations;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto user_id = sqlite3_column_int(stmt, 0);
        auto username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        invitations.emplace_back(username, "", "", user_id);
    }

    sqlite3_finalize(stmt);

    return invitations;
}

void UserDbHandle::drop() const {
    static constexpr auto drop_users_table_query = "DROP TABLE IF EXISTS users;";
    if (sqlite3_exec(db_, drop_users_table_query, nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot drop users table");
    }

    static constexpr auto drop_friends_table_query = "DROP TABLE IF EXISTS friends;";
    if (sqlite3_exec(db_, drop_friends_table_query, nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot drop friends table");
    }
}

bool UserDbHandle::isInvitedBy(int id, int friend_id) const {
    static constexpr auto query = "SELECT COUNT(*) FROM friends WHERE user_id = ? AND friend_id = ? AND status = 0;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot prepare is invited by query");
    }

    sqlite3_bind_int(stmt, 1, friend_id);
    sqlite3_bind_int(stmt, 2, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return (count > 0); // User exists if count is greater than 0
    }

    sqlite3_finalize(stmt);

    return false;
}

void UserDbHandle::addFriendForce(int id, int friend_id) const {
    static constexpr auto query = "INSERT INTO friends (user_id, friend_id, status) VALUES (?, ?, 1);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot prepare insert new friend query");
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, friend_id);

    int rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw std::runtime_error("cannot add friend");
    }
}

void UserDbHandle::removeFriendForce(int id, int friend_id) const {
    static constexpr auto query = "DELETE FROM friends WHERE user_id = ? AND friend_id = ? AND status = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot prepare remove friend query");
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, friend_id);

    int rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw std::runtime_error("cannot remove friend");
    }
}

void UserDbHandle::tryInitializeTables() {
    static constexpr auto create_users_table_query = "CREATE TABLE IF NOT EXISTS users (\
id INTEGER PRIMARY KEY AUTOINCREMENT,\
username TEXT UNIQUE NOT NULL,\
passwordhash TEXT NOT NULL,\
salt TEXT NOT NULL);";

    if (sqlite3_exec(db_, create_users_table_query, nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot create table");
    }

    static constexpr auto create_friends_table_query = "CREATE TABLE IF NOT EXISTS friends (\
user_id INTEGER NOT NULL,\
friend_id INTEGER NOT NULL,\
status INTEGER NOT NULL,\
FOREIGN KEY(user_id) REFERENCES users(id),\
FOREIGN KEY(friend_id) REFERENCES users(id),\
UNIQUE(user_id, friend_id));";

    if (sqlite3_exec(db_, create_friends_table_query, nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw std::runtime_error("cannot create table");
    }
}


void configure_databases() {
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
}

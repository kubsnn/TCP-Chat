#include "account_controller.h"
#include <core/database/user_db_handle.h>
#include <logger.h>

AccountController::AccountController(Client& client, Cache& cache)
    : client_(client), cache_(cache)
{
    addMethod("register", [this](const json& data) {
        if (!verifySignupRequest(data)) return fail("invalid request format");
        return this->signup(data);
    });
    addMethod("login", [this](const json& data) {
        if (!verifySigninRequest(data)) return fail("invalid request format");
        return this->signin(data);
    });
    addMethod("logout", [this](const json& data) {
        if (!verifySignoutRequest(data)) return fail("You are not logged in.");
        return this->signout(data);
    });
}

// register user method
json AccountController::signup(const json& data) const {
    const auto& creds = data["creds"].get<json::dictionary>();
    const auto& username = creds["username"].get<std::string>();
    const auto& password = creds["password"].get<std::string>();

    UserDbHandle db;
    if (db.exists(username)) {
        return fail("Username is taken.");
    }
    auto salt = Crypto::salt();
    auto hashed = hash({ username, password, salt });
    User user(username, std::move(hashed), std::move(salt));
    if (!db.add(user)) {
        return fatal();
    }

    return ok();
}

// login user method
json AccountController::signin(const json& data) const {
    const auto& creds = data["creds"].get<json::dictionary>();
    const auto& username = creds["username"].get<std::string>();
    const auto& password = creds["password"].get<std::string>();

    UserDbHandle db;
    if (!db.exists(username)) {
        return fail("This username doesn't exist.");
    }

    auto user = db.getByName(username);
    if (user.passwordHash() != hash({ username, password, user.salt() })) {
        return fail("Invalid password.");
    }

    if (cache_.isUserOnline(username)) {
        return fail("You are already logged in.");
    }

    if (client_.logged()) {
        return fail("You are already logged in. Logout first.");
    }

    client_.setUsername(username);

    cache_.addUserOnline(client_);

    logger.info() << "User from socket " << client_.socket().fd() << " logged in as `" << username << "`." << std::endl;

    return ok();
}

// logout user method
json AccountController::signout(const json& data) const {
    cache_.removeUserOnline(client_.username());

    logger.info() << '`' << client_.username() << "` logged out." << std::endl;

    client_.setUsername("");

    return ok();
}

bool AccountController::verifySignupRequest(const json& j) const {
    const auto& data = j.get<json::dictionary>();
    if (!data.contains("creds")) return false;
    if (!data["creds"].is<json::dictionary>()) return false;
    const auto& creds = data["creds"].get<json::dictionary>();
    if (!creds.contains("username")) return false;
    if (!creds["username"].is<std::string>()) return false;
    if (!creds.contains("password")) return false;
    if (!creds["password"].is<std::string>()) return false;
    return true;
}

bool AccountController::verifySigninRequest(const json& j) const {
    const auto& data = j.get<json::dictionary>();
    if (!data.contains("creds")) return false;
    if (!data["creds"].is<json::dictionary>()) return false;
    const auto& creds = data["creds"].get<json::dictionary>();
    if (!creds.contains("username")) return false;
    if (!creds["username"].is<std::string>()) return false;
    if (!creds.contains("password")) return false;
    if (!creds["password"].is<std::string>()) return false;
    return true;
}

bool AccountController::verifySignoutRequest(const json& j) const {
    return client_.logged();
}
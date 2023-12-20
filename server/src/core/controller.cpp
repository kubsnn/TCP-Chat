#include "controller.h"
#include "database/user_db_handle.h"

Controller::Controller(Client& client, Cache& cache)
    : client_(client), cache_(cache)
{ 
    addMethod("register", [this](const json& data) {
        return this->registerUser(data);
    });
    addMethod("login", [this](const json& data) {
        return this->loginUser(data);
    });
    addMethod("sendto", [this](const json& data) {
        return this->sendTo(data);
    });
    addMethod("usersOnline", [this](const json& data) {
        return this->usersOnline(data);
    });
}

json Controller::registerUser(const json& data) const {
    json response = json::dictionary();
    const auto& creds = data["creds"].get<json::dictionary>();
    const auto& username = creds["username"].get<std::string>();
    const auto& password_hash = creds["password"].get<std::string>();

    UserDbHandle db;
    if (db.exists(username)) {
        return fail("username is taken");
    }

    if (!db.add({username, password_hash})) {
        return fail("internal server error");
    }

    return ok();
}

json Controller::loginUser(const json& data) const {
    json response = json::dictionary();
    const auto& creds = data["creds"].get<json::dictionary>();
    const auto& username = creds["username"].get<std::string>();
    const auto& password_hash = creds["password"].get<std::string>();

    UserDbHandle db;
    if (!db.exists(username)) {
        return fail("username doesn't exist");
    }

    auto user = db.getByName(username);
    if (user.passwordHash() != password_hash) {
        return fail("invalid password");
    }

    if (cache_.isUserOnline(username)) {
        return fail("you are already logged in");
    }

    client_.setUsername(username);

    cache_.addUserOnline(client_);

    return ok();
}

json Controller::sendTo(const json& data) const {
    const auto& receiver_name = data["who"].get<std::string>();
    
    if (!cache_.isUserOnline(receiver_name)) {
        return fail("user is offline");
    }

    auto user = cache_.getUser(receiver_name);

    bool success = user.socket().write(data["message"].get<std::string>());
    
    
    return ok();
}

json Controller::usersOnline(const json& data) const {
    auto users = cache_.usersOnline();
    json::array response;
    response.reserve(users.size());
    for (const auto& user : users) {
        response.push_back(user.username());
    } 

    return ok(response);
}
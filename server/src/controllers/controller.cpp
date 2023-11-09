#include "controller.h"
#include <core/database/user_db_handle.h>
#include <logger.h>

Controller::Controller(Client& client, Cache& cache)
    : client_(client), cache_(cache)
{ 
    addMethod("register", [this](const json& data) {
        return this->signup(data);
    });
    addMethod("login", [this](const json& data) {
        return this->signin(data);
    });
    addMethod("logout", [this](const json& data) {
        return this->signout(data);
    });
    addMethod("sendto", [this](const json& data) {
        return this->sendTo(data);
    });
    addMethod("usersOnline", [this](const json& data) {
        return this->usersOnline(data);
    });
}

// register user method
json Controller::signup(const json& data) const {
    json response = json::dictionary();
    const auto& creds = data["creds"].get<json::dictionary>();
    const auto& username = creds["username"].get<std::string>();
    const auto& password = creds["password"].get<std::string>();

    UserDbHandle db;
    if (db.exists(username)) {
        return fail("username is taken");
    }

    if (!db.add({ username, hash(password) })) {
        return fatal();
    }

    return ok();
}

// login user method
json Controller::signin(const json& data) const {
    json response = json::dictionary();
    const auto& creds = data["creds"].get<json::dictionary>();
    const auto& username = creds["username"].get<std::string>();
    const auto& password = creds["password"].get<std::string>();

    UserDbHandle db;
    if (!db.exists(username)) {
        return fail("username doesn't exist");
    }

    auto user = db.getByName(username);
    if (user.passwordHash() != hash(password)) {
        return fail("invalid password");
    }

    if (cache_.isUserOnline(username)) {
        return fail("you are already logged in");
    }

    if (client_.logged()) {
        return fail("you are already logged in; logout first");
    }

    client_.setUsername(username);

    cache_.addUserOnline(client_);

    return ok();
}

// logout user method
json Controller::signout(const json& data) const {
    if (!client_.logged()) {
        return fail("you are not logged in");
    }

    cache_.removeUserOnline(client_.username());
    client_.setUsername("");

    return ok();
}

json Controller::sendTo(const json& data) const {
    if (!client_.logged()) {
        return fail("you are not logged in");
    }

    const auto& receiver_name = data["who"].get<std::string>();
    
    if (!cache_.isUserOnline(receiver_name)) {
        return fail("user is offline");
    }

    json message = json::dictionary();
    message["from"] = client_.username();
    message["to"] = receiver_name;
    message["message"] = data["message"].get<std::string>();
    message["type"] = "message";

    auto user = cache_.getUser(receiver_name);
    user.writeEncrypted(message.to_string());

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
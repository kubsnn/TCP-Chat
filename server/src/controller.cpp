#include "controller.h"
#include "user_db_handle.h"

Controller::Controller(Client& client, Cache& cache)
    : client_(client), cache_(cache)
{ }

bool Controller::containsMethod(const std::string& method) const {
    static constexpr std::string_view methods[] = { "usersOnline", "register", "login", "sendto" };
    for (auto& m : methods) {
        if (method == m) return true;
    }
    return false;
}

json Controller::execute(const json& data) const {
    const auto& action = data["action"].get<std::string>();

    if (action == "register") return registerUser(data);
    if (action == "login") return loginUser(data);
    if (action == "sendto") return sendTo(data);
    if (action == "usersOnline") return usersOnline(data);

    json response = resultFail("method does not exist");
    response["error"] = true;
    return response;
}

json Controller::registerUser(const json& data) const {
    json response = json::dictionary();
    const auto& creds = data["creds"].get<json::dictionary>();
    const auto& username = creds["username"].get<std::string>();
    const auto& password_hash = creds["password"].get<std::string>();

    UserDbHandle db;
    if (db.exists(username)) {
        return resultFail("username is taken");
    }

    if (!db.add({username, password_hash})) {
        return resultFail("internal server error");
    }

    return resultOk();
}

json Controller::loginUser(const json& data) const {
    json response = json::dictionary();
    const auto& creds = data["creds"].get<json::dictionary>();
    const auto& username = creds["username"].get<std::string>();
    const auto& password_hash = creds["password"].get<std::string>();

    UserDbHandle db;
    if (!db.exists(username)) {
        return resultFail("username doesn't exist");
    }

    auto user = db.getByName(username);
    if (user.passwordHash() != password_hash) {
        return resultFail("invalid password");
    }

    if (cache_.isUserOnline(username)) {
        return resultFail("you are already logged in");
    }

    client_.setUsername(username);

    cache_.addUserOnline(client_);

    return resultOk();
}

json Controller::sendTo(const json& data) const {
    const auto& receiver_name = data["who"].get<std::string>();
    
    if (!cache_.isUserOnline(receiver_name)) {
        return resultFail("user is offline");
    }

    auto user = cache_.getUser(receiver_name);

    bool success = user.socket().write(data["message"].get<std::string>());
    
    
    return resultOk();
}

json Controller::usersOnline(const json& data) const {
    auto users = cache_.usersOnline();
    json::array response;
    response.reserve(users.size());
    for (const auto& user : users) {
        response.push_back(user.username());
    } 

    return resultOk(response);
}

json Controller::resultOk(json&& result) const {
    json response = resultOk();
    response["values"] = std::move(result);
    return response;
}

json Controller::resultOk() const {
    json response = json::dictionary();
    response["result"] = "ok";
    return response;
}

json Controller::resultFail(std::string message) const {
    json response = json::dictionary();
    response["result"] = "fail";
    response["message"] = std::move(message);
    return response;
}

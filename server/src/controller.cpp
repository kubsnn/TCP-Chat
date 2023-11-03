#include "controller.h"
#include "user_db_handle.h"

Controller::Controller(Client& client)
    : client_(client)
{ }

bool Controller::containsMethod(const std::string& method) const {
    static constexpr std::string_view methods[] = { "register", "login", "sendto" };
    for (auto& m : methods) {
        if (method == m) return true;
    }
    return false;
}

json Controller::execute(const json &data) const {
    const auto& action = data["action"].get<std::string>();

    if (action == "register") return registerUser(data);
    if (action == "login") return loginUser(data);
    if (action == "sendto") return sendTo(data);

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

json Controller::loginUser(const json &data) const {
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

    return resultOk();
}

json Controller::sendTo(const json &data) const {
    int fd = data["who"].get<json::number>();
    bool success = Client(fd, sockaddr_in{}).socket().write(data["message"].get<json::string>());
    
    
    return resultOk();
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

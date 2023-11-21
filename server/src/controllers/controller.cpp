#include "controller.h"
#include <core/database/user_db_handle.h>
#include <logger.h>

Controller::Controller(Client& client, Cache& cache)
    : client_(client), cache_(cache)
{ 
    addMethod("sendto", [this](const json& data) {
        if (!verifySendToRequest(data)) return fail("invalid request format");
        return this->sendTo(data);
    });
    addMethod("usersOnline", [this](const json& data) {
        if (!verifyUsersOnlineRequest(data)) return fail("invalid request format");
        return this->usersOnline(data);
    });
    addMethod("search", [this](const json& data) {
        if (!verifySearchRequest(data)) return fail("invalid request format");
        return this->search(data);
    });
    addMethod("invite", [this](const json& data) {
        if (!verifyAddFriendRequest(data)) return fail("invalid request format");
        return this->addFriend(data);
    });
    addMethod("friends", [this](const json& data) {
        if (!verifyGetFriendsRequest(data)) return fail("invalid request format");
        return this->getFriends(data);
    });
    addMethod("accept", [this](const json& data) {
        if (!verifyAcceptInvitationRequest(data)) return fail("invalid request format");
        return this->acceptInvitation(data);
    });
    addMethod("invitations", [this](const json& data) {
        if (!verifyGetInvitationsRequest(data)) return fail("invalid request format");
        return this->getInvitations(data);
    });
}

json Controller::invoke(const std::string& method, const json& data) const {
    if (!client_.logged()) {
        return fail("you are not logged in");
    }

    return IController::invoke(method, data);
}

json Controller::sendTo(const json& data) const {
    const auto& receiver_name = data["who"].get<std::string>();
    
    if (!cache_.isUserOnline(receiver_name)) {
        return fail("user is offline");
    }

    json message = {
        { "from", client_.username() },
        { "to", receiver_name },
        { "message", data["message"].get<std::string>() },
        { "type", "message" },
        { "action", "received" }
    };

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

    return ok(std::move(response));
}

json Controller::search(const json& data) const {
    const auto& username = data["who"].get<std::string>();

    UserDbHandle db;
    auto users = db.getByNameLike(username);

    json::array response;
    response.reserve(users.size());
    for (const auto& user : users) {
        json user_dto = {
            { "username", user.username() },
            { "online", cache_.isUserOnline(user.username()) }
        };
        response.push_back(std::move(user_dto));
    }

    return ok(std::move(response));
}

json Controller::addFriend(const json& data) const {
    const auto& friend_name = data["who"].get<std::string>();

    UserDbHandle db;
    if (!db.exists(friend_name)) {
        return fail("user doesn't exist");
    }

    auto user = db.getByName(client_.username());
    auto invitations = db.getInvitations(user.id());

    for (const auto& inv : invitations) {
        if (inv.username() == friend_name) {
            return fail("you are already invited");
        }
    }

    if (!db.addFriend(user.id(), friend_name)) {
        return fail("already friends");
    }

    return ok();
}

json Controller::getFriends(const json& data) const {
    UserDbHandle db;
    auto user = db.getByName(client_.username(), true);
    auto online_users = cache_.usersOnline();

    json::array response;
    response.reserve(user.friends().size());
    
    for (const auto& f : user.friends()) {
        json json_dto = {
            { "username", f.username() },
            { "online", cache_.isUserOnline(f.username()) }
        };
        response.push_back(std::move(json_dto));
    }

    return ok(std::move(response));
}

json Controller::acceptInvitation(const json& data) const {
    const auto& friend_name = data["who"].get<std::string>();

    UserDbHandle db;
    if (!db.exists(friend_name)) {
        return fail("user doesn't exist");
    }

    auto user = db.getByName(client_.username());
    if (!db.acceptInvitation(user.id(), friend_name)) {
        return fail("already friends");
    }

    return ok();
}

json Controller::getInvitations(const json& data) const {
    UserDbHandle db;
    auto user = db.getByName(client_.username());
    auto invitations = db.getInvitations(user.id());

    json::array response;
    response.reserve(invitations.size());
    for (const auto& inv : invitations) {
        json invite_dto = {
            { "username", inv.username() }
        };
        response.push_back(std::move(invite_dto));
    }

    return ok(std::move(response));
}

bool Controller::verifySendToRequest(const json& j) const {
    const auto& data = j.get<json::dictionary>();
    if (!data.contains("who")) return false;
    if (!data["who"].is<std::string>()) return false;
    if (!data.contains("message")) return false;
    if (!data["message"].is<std::string>()) return false;
    return true;
}

bool Controller::verifyUsersOnlineRequest(const json& j) const {
    return true;
}

bool Controller::verifySearchRequest(const json& j) const {
    const auto& data = j.get<json::dictionary>();
    if (!data.contains("who")) return false;
    if (!data["who"].is<std::string>()) return false;
    return true;
}

bool Controller::verifyAddFriendRequest(const json& j) const {
    const auto& data = j.get<json::dictionary>();
    if (!data.contains("who")) return false;
    if (!data["who"].is<std::string>()) return false;
    return true;
}

bool Controller::verifyGetFriendsRequest(const json& j) const {
    return true;
}

bool Controller::verifyAcceptInvitationRequest(const json& j) const {
    const auto& data = j.get<json::dictionary>();
    if (!data.contains("who")) return false;
    if (!data["who"].is<std::string>()) return false;
    return true;
}

bool Controller::verifyGetInvitationsRequest(const json& j) const {
    return true;
}

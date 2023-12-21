#include "controller.h"
#include <core/database/user_db_handle.h>
#include <logger.h>

Controller::Controller(Client& client, Cache& cache)
    : client_(client), cache_(cache)
{ 
    addMethod("sendto", [this](const json& data) {
        if (!verifySendToRequest(data)) return invalid_format_;
        return this->sendTo(data);
    });
    addMethod("usersOnline", [this](const json& data) {
        if (!verifyUsersOnlineRequest(data)) return invalid_format_;
        return this->usersOnline(data);
    });
    addMethod("search", [this](const json& data) {
        if (!verifySearchRequest(data)) return invalid_format_;
        return this->search(data);
    });
    addMethod("invite", [this](const json& data) {
        if (!verifyAddFriendRequest(data)) return invalid_format_;
        return this->addFriend(data);
    });
    addMethod("friends", [this](const json& data) {
        if (!verifyGetFriendsRequest(data)) return invalid_format_;
        return this->getFriends(data);
    });
    addMethod("accept", [this](const json& data) {
        if (!verifyAcceptInvitationRequest(data)) return invalid_format_;
        return this->acceptInvitation(data);
    });
    addMethod("invitations", [this](const json& data) {
        if (!verifyGetInvitationsRequest(data)) return invalid_format_;
        return this->getInvitations(data);
    });
    addMethod("removeFriend", [this](const json& data) {
        if (!verifyRemoveFriendRequest(data)) return invalid_format_;
        return this->removeFriend(data);
    });
}

json Controller::invoke(const std::string& method, const json& data) const {
    if (!client_.logged()) {
        return fail("You must be logged in!");
    }

    return IController::invoke(method, data);
}

json Controller::sendTo(const json& data) const {
    const auto& receiver_name = data["who"].get<std::string>();
    
    if (!cache_.isUserOnline(receiver_name)) {
        return fail("User is offline.");
    }

    UserDbHandle db;
    if (!db.isFriend(client_.username(), receiver_name)) {
        return fail("You are not friends.");
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
        return fail("User doesn't exist.");
    }

    auto user = db.getByName(client_.username());
    auto invitations = db.getInvitations(user.id());

    for (const auto& inv : invitations) {
        if (inv.username() == friend_name) {
            return fail("You are already invited.");
        }
    }

    if (!db.addFriend(user.id(), friend_name)) {
        return fail("You are already friends!");
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
        return fail("User does not exist.");
    }

    auto user = db.getByName(client_.username());
    if (!db.acceptInvitation(user.id(), friend_name)) {
        return fail("You are already friends!");
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

json Controller::removeFriend(const json& data) const {
    const auto& friend_name = data["who"].get<std::string>();

    UserDbHandle db;
    
    if (!db.exists(friend_name)) {
        return fail("Friend does not exist.");
    }

    if (!db.isFriend(client_.username(), friend_name)) {
        return fail("You are not friends.");
    }

    auto user = db.getByName(client_.username());
    if (!db.removeFriend(user.id(), friend_name)) {
        return fatal();
    }

    return ok();    
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

bool Controller::verifyRemoveFriendRequest(const json &data) const {
    const auto& dict = data.get<json::dictionary>();
    if (!dict.contains("who")) return false;
    if (!dict["who"].is<std::string>()) return false;
    return true;
}

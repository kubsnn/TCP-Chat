#pragma once
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <core/icontroller.h>
#include <core/client.h>
#include <core/cache.h>
#include <json.hpp>

class Controller : public IController {
public:
    Controller(Client& client, Cache& cache);

    json invoke(const std::string& method, const json& data) const override;
private:

    json sendTo(const json& data) const;
    json usersOnline(const json& data) const;
    json search(const json& data) const;
    json addFriend(const json& data) const;
    json getFriends(const json& data) const;
    json acceptInvitation(const json& data) const;
    json getInvitations(const json& data) const;

    bool verifySendToRequest(const json& data) const;
    bool verifyUsersOnlineRequest(const json& data) const;
    bool verifySearchRequest(const json& data) const;
    bool verifyAddFriendRequest(const json& data) const;
    bool verifyGetFriendsRequest(const json& data) const;
    bool verifyAcceptInvitationRequest(const json& data) const;
    bool verifyGetInvitationsRequest(const json& data) const;
    
    Client& client_;
    Cache& cache_;
};

#endif //!__CONTROLLER_H__
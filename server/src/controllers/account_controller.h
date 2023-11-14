#pragma once
#ifndef __ACCOUNT_CONTROLLER_H__
#define __ACCOUNT_CONTROLLER_H__

#include <core/icontroller.h>
#include <core/cache.h>
#include <core/client.h>

class AccountController : public IController {
public:
    AccountController(Client& client, Cache& cache);

private:
    json signup(const json& data) const;
    json signin(const json& data) const;
    json signout(const json& data) const;

    bool verifySignupRequest(const json& data) const;
    bool verifySigninRequest(const json& data) const;
    bool verifySignoutRequest(const json& data) const;

    Client& client_;
    Cache& cache_;
};


#endif //!__ACCOUNT_CONTROLLER_H__
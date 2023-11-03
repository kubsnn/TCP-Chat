#pragma once
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <core/icontroller.h>
#include <core/client.h>
#include <core/cache.h>
#include <jaszyk/json.hpp>

class Controller : public IController {
public:
    Controller(Client& client, Cache& cache);

private:
    json signup(const json& data) const;
    json signin(const json& data) const;
    json signout(const json& data) const;
    json sendTo(const json& data) const;
    json usersOnline(const json& data) const;

    Client& client_;
    Cache& cache_;
};

#endif //!__CONTROLLER_H__
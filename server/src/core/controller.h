#pragma once
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "icontroller.h"
#include "client.h"
#include "cache.h"
#include <jaszyk/json.hpp>

class Controller : public IController {
public:
    Controller(Client& client, Cache& cache);

private:
    json registerUser(const json& data) const;
    json loginUser(const json& data) const;
    json sendTo(const json& data) const;
    json usersOnline(const json& data) const;

    Client& client_;
    Cache& cache_;
};

#endif //!__CONTROLLER_H__
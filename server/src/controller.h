#pragma once
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "client.h"
#include "cache.h"
#include <jaszyk/json.hpp>

class Controller {
public:
    Controller(Client& client, Cache& cache);
    bool containsMethod(const std::string& method) const;
    json execute(const json& data) const;

    json registerUser(const json& data) const;
    json loginUser(const json& data) const;
    json sendTo(const json& data) const;
    json usersOnline(const json& data) const;
private:
    json resultOk(json&& result) const;
    json resultOk() const;
    json resultFail(std::string message) const;

    Client& client_;
    Cache& cache_;
};

#endif //!__CONTROLLER_H__
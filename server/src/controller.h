#pragma once
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "client.h"
#include <jaszyk/json.hpp>

class Controller {
public:
    Controller(Client& client);
    bool containsMethod(const std::string& method) const;
    json execute(const json& data) const;

    json registerUser(const json& data) const;
    json loginUser(const json& data) const;
    json sendTo(const json& data) const;
private:
    json resultOk() const;
    json resultFail(std::string message) const;

    Client& client_;
};

#endif //!__CONTROLLER_H__
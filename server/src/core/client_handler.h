#pragma once
#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include "client.h"
#include "controller.h"
#include "cache.h"
#include <jaszyk/json.hpp>
#include <string>

using json = jaszyk::json;

class ClientHandler {

    static const json invalidInputJsonError();
public:
    ClientHandler(Client client, Cache& cache);

    void run();
private:
    bool isValidRequest(const json& data);
    json execute(const json& query);

    Client client_;
    Cache& cache_;
};

#endif //!__CLIENT_HANDLER_H__
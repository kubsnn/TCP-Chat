#pragma once
#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include "client.h"
#include "../controllers/controller.h"
#include "cache.h"
#include "utils/crypto.h"
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
    bool sendResponse(const json& response);
    json receiveRequest();     
    bool initializeClientPublicKey();

    Client client_;
    Crypto crypto_;
    Cache& cache_;
};

#endif //!__CLIENT_HANDLER_H__
#include "client_handler.h"
#include <iostream>
#include "../logger.h"

const json ClientHandler::invalidInputJsonError() {
    json data = json::dictionary();
    data["error"] = true;
    data["message"] = std::string("Received json doesn't match server's requirements.");
    return data;
}

ClientHandler::ClientHandler(Client client, Cache& cache)
    : client_(std::move(client)), cache_(cache)
{ }

void ClientHandler::run() {
    logger.info() << client_.ip() << " connected on socket " << client_.socket().fd() << '.' << std::endl;

    while (true) {
        try {
            json request = client_.socket().read();
            if (!isValidRequest(request)) {
                if (!client_.socket().write(invalidInputJsonError())) {
                    break;
                }
                continue;
            }

            auto result = execute(request);

            if (result["result"].get<std::string>() == "fatal") {
                break;
            }
            result["response"] = true;
            client_.socket().write(result);

        } catch (const std::exception& ex) { // socket closed
            break;
        }
    }

    logger.info() << client_.ip() << " disconnected from socket " << client_.socket().fd() << '.' << std::endl;
    if (client_.username() != "" && cache_.isUserOnline(client_.username())) { 
        cache_.removeUserOnline(client_.username());
    }
    client_.disconnect();
}

bool ClientHandler::isValidRequest(const json& data) {
    if (!data.is<json::dictionary>()) return false;
    const auto& dict = data.get<json::dictionary>();
    return dict.contains("action") && Controller(client_, cache_).hasMethod(dict["action"].get<std::string>());
}

json ClientHandler::execute(const json& query) {
    return Controller(client_, cache_).invoke(query["action"].get<std::string>(), query);
}

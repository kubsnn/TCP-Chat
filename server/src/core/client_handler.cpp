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
{ 
    crypto_.create();
}

void ClientHandler::run() {
    logger.info() << client_.ip() << " connected on socket " << client_.socket().fd() << '.' << std::endl;

    if (!initializeClientPublicKey()) {
        logger.info() << client_.ip() << " disconnected from socket " << client_.socket().fd() << '.' << std::endl;
        client_.disconnect();
        return;
    }

    while (true) {
        try {
            json request = receiveRequest();
            if (!isValidRequest(request)) {
                if (!sendResponse(invalidInputJsonError())) {
                    break;
                }
                continue;
            }

            auto result = execute(request);

            if (result["result"].get<std::string>() == "fatal") {
                break;
            }
            
            result["response"] = true;
            result["action"] = std::move(request["action"]);
            sendResponse(result);

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
    try {
        return Controller(client_, cache_).invoke(query["action"].get<std::string>(), query);
    } catch (const std::exception& ex) {
        logger.warning() << "Exception occured: " << ex.what() << std::endl;
        return IController().fatal();
    } catch (...) {
        logger.error() << "Unknown exception occured." << std::endl;
        return IController().fatal();
    }
}

bool ClientHandler::sendResponse(const json& response) {
    return client_.writeEncrypted(response.to_string());
}

json ClientHandler::receiveRequest() {
    return json::parse(client_.socket().read(crypto_));
}

bool ClientHandler::initializeClientPublicKey() {
    json message = json::dictionary();
    message["public_key"] = crypto_.public_key();
    message["message"] = "Waiting for your public key...";
    message["action"] = "init";

    const std::string msg = message.to_string();
    message = false; // free json

    while (true) {
        try {
            if (!client_.socket().write(msg)) {
                return false;
            }

            auto response = json::parse((client_.socket().read(crypto_)));

            if (!response.is<json::dictionary>()) continue;
            
            const auto& dict = response.get<json::dictionary>();
            if (!dict.contains("public_key")) continue;

            if (!Crypto::verifyKey(dict["public_key"].get<std::string>())) continue;

            client_.setPublicKey(dict["public_key"].get<std::string>());
            return true;
            
        } catch (const std::exception& ex) {
            return false;
        }
    }
    return false;
}

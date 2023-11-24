#include "client_handler.h"
#include <iostream>

const json ClientHandler::invalidInputJsonError() {
    json data = json::dictionary();
    data["error"] = true;
    data["message"] = std::string("Received json doesn't match requirements.");
    return data;
}

ClientHandler::ClientHandler(Client client)
    : client_(std::move(client))
{ }

void ClientHandler::run() {
    std::cout << "client: " << client_.socket().fd() << ", ip: " << client_.ip() << " connected" << std::endl;
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

    std::cout << "client: " << client_.socket().fd() << " disconnected" << std::endl;
    client_.disconnect();
}

bool ClientHandler::isValidRequest(const json& data) {
    if (!data.is<json::dictionary>()) return false;
    const auto& dict = data.get<json::dictionary>();
    return dict.contains("action") && Controller(client_).containsMethod(dict["action"].get<std::string>());
}

json ClientHandler::execute(const json& query) {
    return Controller(client_).execute(query);
}

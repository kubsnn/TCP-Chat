#include "icontroller.h"
#include "utils/crypto.h"
#include <stdexcept>
#include <thread>
#include <chrono>

bool IController::hasMethod(const std::string& method) const {
    return methods_.contains(method);
}

json IController::invoke(const std::string& method, const json& data) const {
    if (!methods_.contains(method)) {
        throw std::runtime_error("Method does not exist.");
    }

    return methods_[method](data);
}

void IController::addMethod(std::string name, std::function<json(const json &)> method) {
    methods_.insert(std::move(name), std::move(method));
}

std::string IController::hash(const std::string& data) const {
    char salted[128] = "^H3lloW0rld!@#";
    std::fill(std::begin(salted) + 14, std::end(salted), '\0');
    std::copy(data.begin(), data.end(), std::begin(salted) + 14);
    std::this_thread::sleep_for(std::chrono::milliseconds(Crypto::random(1, 10)));    
    return Crypto::sha256(salted, data.size() + 14);
}

json IController::ok(json&& result) const {
    json response = ok();
    response["values"] = std::move(result);
    return response;
}

json IController::ok() const {
    json response = json::dictionary();
    response["result"] = "ok";
    return response;
}

json IController::fail(std::string message) const {
    json response = json::dictionary();
    response["result"] = "fail";
    response["message"] = std::move(message);
    return response;
}

json IController::fatal() const {
    json response = json::dictionary();
    response["result"] = "fatal";
    response["message"] = "internal server error";
    return response;
}

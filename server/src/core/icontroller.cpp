#include "icontroller.h"
#include <stdexcept>

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
#pragma once
#ifndef __I_CONTROLLER_H__
#define __I_CONTROLLER_H__

#include <jaszyk/json.hpp>
#include <functional>
#include <string>

using json = jaszyk::json;

class IController {
    template <typename T1, typename T2>
    using map = jaszyk::flatmap<T1, T2>;
public:
    bool hasMethod(const std::string& method) const;
    json invoke(const std::string& method, const json& data) const;
    
protected:
    void addMethod(std::string name, std::function<json(const json&)> method);

    json ok(json&& result) const;
    json ok() const;
    json fail(std::string message) const;

private:
    map<std::string, std::function<json(const json&)>> methods_;
};

#endif //!__I_CONTROLLER_H__
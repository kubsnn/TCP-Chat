#pragma once
#ifndef __WRAPPERS_H__
#define __WRAPPERS_H__

#include "utils/constants.h"
#include <jaszyk/json.hpp>
#include <functional>
#include <mutex>

using jaszyk::json;

void register_signal(int signum, std::function<void(int)> handler);

class Socket {
    inline static std::mutex mutexes[max_clients];
public:
    Socket(int fd);
    Socket(Socket&& other) noexcept;
    Socket(const Socket& other);
    ~Socket();
    json read() const;
    bool write(const json& data) const;
    void close();
    int fd() const;

    Socket& operator=(Socket&& other) noexcept;
    Socket& operator=(const Socket& other);
private:
    int sockfd_;
};



#endif // !__WRAPPERS_H__
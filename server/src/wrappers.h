#pragma once
#ifndef __WRAPPERS_H__
#define __WRAPPERS_H__

#include "constants.h"
#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <functional>
#include <exception>
#include <iostream>
#include <jaszyk/json.hpp>
#include <mutex>
#include <unordered_map>

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
    void ping() const;
    int fd() const;

    Socket& operator=(Socket&& other) noexcept;
    Socket& operator=(const Socket& other);
private:
    int sockfd_;
};



#endif // !__WRAPPERS_H__
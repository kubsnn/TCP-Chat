#pragma once

#include <cstring>
#include <netinet/in.h>
#include "wrappers.h" // You need to include the appropriate header for the Socket class.

class Client {
    static constexpr size_t kIpSize = sizeof("xxx.xxx.xxx.xxx");

public:
    Client(int sockfd, sockaddr_in addr);
    Client(Client&& other) noexcept;
    Client(const Client& other);

    const Socket& socket() const;
    const char* ip() const;

    Client& operator=(Client&& other) noexcept;
    Client& operator=(const Client& other);
    
private:
    char* ip_;
    Socket socket_;
};

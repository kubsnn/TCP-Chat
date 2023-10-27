#pragma once

#include "socket.h"
#include "utils/crypto.h"
#include <netinet/in.h>
#include <string>

class Client {
    static constexpr size_t kIpSize = sizeof("xxx.xxx.xxx.xxx");

public:
    Client(int sockfd, sockaddr_in addr);
    Client(Client&& other) noexcept;
    Client(const Client& other);

    bool writeEncrypted(const std::string& data) const;

    const Socket& socket() const;
    const char* ip() const;
    const std::string& username() const;
    const Crypto& crypto() const;
    
    void setPublicKey(const std::string& key);
    void setUsername(std::string username);

    void disconnect();

    Client& operator=(Client&& other) noexcept;
    Client& operator=(const Client& other);
    
private:
    char* ip_;
    Socket socket_;
    std::string username_;
    Crypto crypto_;
};

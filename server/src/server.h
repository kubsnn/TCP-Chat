#pragma once
#ifndef __SERVER_H__
#define __SERVER_H__

#include "core/client.h"
#include "core/cache.h"
#include <netinet/in.h>
#include <string>

class Server {
public:
    Server(const char* ip, int port);
    ~Server();

    void run();

private:
    Client createClient() const;
    void buildServer();
    void close();

    Cache cache_;
    std::string ip_;
    int sockfd_;
    int port_;
    sockaddr_in addr_;
};

#endif //!__SERVER_H__
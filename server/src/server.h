#pragma once
#ifndef __SERVER_H__
#define __SERVER_H__

#include "core/database/user_db_handle.h"
#include "core/client.h"
#include "core/socket.h"
#include "core/cache.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


class Server {
public:
    Server(const char* ip, int port);
    ~Server();

    void run();

private:
    Client createClient() const;
    void buildServer();
    void onInterrupt(int signum);

    Cache cache_;
    std::string ip_;
    int sockfd_;
    int port_;
    sockaddr_in addr_;
};

#endif //!__SERVER_H__
#pragma once
#ifndef __SERVER_H__
#define __SERVER_H__

#include "wrappers.h"
#include "user_db_handle.h"
#include "client.h"

#include <jaszyk/json.hpp>
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

    std::unordered_map<std::string, Client> users_online_;
    int sockfd_;
    sockaddr_in addr_;
};

#endif //!__SERVER_H__
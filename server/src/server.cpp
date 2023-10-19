#include "server.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <string>
#include <algorithm>
#include <csignal>

Server::Server(const char* ip, int port) {
    addr_.sin_port = htons(port);
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip);

    register_signal(SIGINT, [this](int signum) {
        std::cout << "server closed due to an interrupt" << std::endl;
            this->onInterrupt(SIGINT);
        exit(signum);
    });
}

Server::~Server() { 
    close(sockfd_);
}

void Server::run() {
    std::cout << "starting server..." << std::endl;
    buildServer();

    while (true) {
        auto client = createClient();
        std::thread(handleClient, client).detach();
    }

    close(sockfd_);
}

Client Server::createClient() const {
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int clientfd = accept(sockfd_, (sockaddr*)&client_addr, &len);
    if (clientfd < 0 && sockfd_ != -1) {
        std::cerr << "accept error" << std::endl;
        throw;
    }
    return Client(clientfd, client_addr);
}

void Server::buildServer() {
    std::cout << "opening socket..." << std::endl;
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0) {
        std::cerr << "socket error" << std::endl;
        return;
    }

    std::cout << "binding..." << std::endl;
    if (bind(sockfd_, (sockaddr*)&addr_, sizeof(addr_)) < 0) {
        std::cerr << "bind error" << std::endl;
        return;
    }

    std::cout << "listening..." << std::endl;
    if (listen(sockfd_, 5) < 0) {
        std::cerr << "listen error" << std::endl;
        return;
    }
}
 
void Server::handleClient(Client client) {
    std::cout << "client: " << client.socket().fd() << ", ip: " << client.ip() << " connected" << std::endl;
    while (true) {
        try {
            json data = client.socket().read();

            if (data["action"].get<json::number>() == 1) {
                // lets forward
                int fd = data["who"].get<json::number>();
                Client(fd, sockaddr_in{}).socket().write(data["message"].get<json::string>());
            }

        } catch (const std::exception& ex) { // socket closed
            break;
        }
    }
}

void Server::onInterrupt(int signum) {
    close(sockfd_);
    sockfd_ = -1;
}
#include "server.h"
#include "client_handler.h"
#include "logger.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <string>
#include <algorithm>
#include <csignal>

Server::Server(const char* ip, int port) 
    : ip_(ip), port_(port)
{
    addr_.sin_port = htons(port);
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip);

    register_signal(SIGINT, [this](int signum) {
        std::cout << "\n";
        logger.warning() << "SIGINT has been called. Closing server..." << std::endl;
        this->onInterrupt(SIGINT);
        logger.info() << "Sockets closed. Exiting..." << std::endl;
        exit(signum);
    });
}

Server::~Server() { 
    close(sockfd_);
}

void Server::run() {
    
    logger.info() << "Building server..." << std::endl;
    buildServer();
    logger.info() << "Application started." << std::endl;
    while (true) {
        try {
            auto client = createClient();
            std::thread([client, this]() {
                ClientHandler(client, this->cache_).run();
            }).detach();
        } catch (std::exception& ex) {
            logger.error() << "[exception] " << ex.what() << std::endl;
        }
    }

    close(sockfd_);
}

Client Server::createClient() const {
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int clientfd = accept(sockfd_, (sockaddr*)&client_addr, &len);
    if (clientfd < 0 && sockfd_ != -1) {
        logger.error() << "Accept failed. Closing application..." << std::endl;
        exit(-1);
    }
    return Client(clientfd, client_addr);
}

void Server::buildServer() {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0) {
        logger.error() << "Opening socket failed. Closing application..." << std::endl;
        exit(-1);
    }
    logger.info() << "Socket OK." << std::endl;

    if (bind(sockfd_, (sockaddr*)&addr_, sizeof(addr_)) < 0) {
        logger.error() << "Binding failed. Closing application..." << std::endl;
        exit(-1);
    }
    logger.info() << "Binding OK." << std::endl;

    if (listen(sockfd_, 5) < 0) {
        logger.error() << "Listening failed. Closing application..." << std::endl;
        exit(-1);
    }
    logger.info() << "Listening on " << ip_ << ':' << port_ << std::endl;
}

void Server::onInterrupt(int signum) {
    auto clients = cache_.usersOnline();
    for (auto& client : clients) {
        close(client.socket().fd());
    }
    close(sockfd_);
    sockfd_ = -1;
}
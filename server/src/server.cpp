#include "server.h"

#include "core/client_handler.h"
#include "core/utils/globals.h"
#include "logger.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <algorithm>
#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

Server::Server(const char* ip, int port) 
    : ip_(ip), port_(port)
{
    addr_.sin_port = htons(port);
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip);

    register_signal(SIGINT, [this](int signum) {
        std::cout << '\n';
        logger.warning() << "SIGINT has been called. Closing server..." << std::endl;
        this->close();
        
        exit(signum);
    });

    logger.info() << "Starting in `" << globals::executable_dir.string() << "`." << std::endl;
}

Server::~Server() { 
    cache_.disconnectAll();

    ::close(sockfd_);
}

void Server::run() {
    
    logger.info() << "Building server..." << std::endl;
    buildServer();
    logger.info() << "Application started." << std::endl;
    logger.debug() << "Debug mode enabled." << std::endl;
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

    ::close(sockfd_);
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

    int opt = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        logger.error() << "Setting socket option failed. Closing application..." << std::endl;
        exit(-1);
    }

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

void Server::close() {
    cache_.disconnectAll();

    ::close(sockfd_);
    sockfd_ = -1;

    logger.info() << "Sockets closed. Exiting..." << std::endl;
}
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <csignal>
#include <jaszyk/json.hpp>
#include "wrappers.h"
#include "user_database.h"

class Client {
    static constexpr size_t kIpSize = sizeof("xxx.xxx.xxx.xxx");
public:
    Client(int sockfd, sockaddr_in addr) 
        : socket_(sockfd) 
    {
        ip_ = new char[kIpSize];
        inet_ntop(AF_INET, &addr.sin_addr, ip_, kIpSize);
    }

    Client(Client&& other) noexcept 
        : socket_(std::move(other.socket_))
    {
        ip_ = other.ip_;
        other.ip_ = nullptr;
    }

    Client(const Client& other)
        : socket_(other.socket_) 
    {
        ip_ = new char[kIpSize];
        std::copy(&other.ip_[0], &other.ip_[kIpSize], &ip_[0]);
    }

    inline const Socket& socket() const {
        return socket_;
    }

    inline const char* ip() const {
        return ip_;
    }

    inline Client& operator=(Client&& other) noexcept {
        socket_ = std::move(other.socket_);
        ip_ = other.ip_;
        other.ip_ = nullptr;

        return *this;
    }

    inline Client& operator=(const Client& other) {
        socket_ = other.socket_;
        ip_ = new char[kIpSize];
        std::copy(&other.ip_[0], &other.ip_[kIpSize], &ip_[0]);

        return *this;
    }

private:
    char* ip_;
    Socket socket_;
};

class Server {
public:
    Server(const char* ip, int port) {
        addr_.sin_port = htons(port);
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = inet_addr(ip);

        register_signal(SIGINT, [this](int signum) {
            std::cout << "server closed due to an interrupt" << std::endl;
            this->onInterrupt(SIGINT);
            exit(signum);
        });
    }

    ~Server() { 
        close(sockfd_);
    }

    void run() {
        std::cout << "starting server..." << std::endl;
        buildServer();

        while (true) {
            auto client = createClient();

            std::thread(handleClient, client).detach();
        }

        close(sockfd_);
    }

private:
    Client createClient() const {
        sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int clientfd = accept(sockfd_, (sockaddr*)&client_addr, &len);
        if (clientfd < 0 && sockfd_ != -1) {
            std::cerr << "accept error" << std::endl;
            throw;
        }
        return Client(clientfd, client_addr);
    }

    void buildServer() {
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

    static void handleClient(Client client) {
        std::cout << "client: " << client.socket().fd() << ", ip: " << client.ip() << " connected" << std::endl;

        while (true) {
            try {
                json data = client.socket().read();
                json obj = json::dictionary();
                obj["success"] = true;
                data.add(obj);
                client.socket().write(data);
            } catch (const std::exception& ex) { // socket closed
                break;
            }
        }
    }

    void onInterrupt(int signum) {
        close(sockfd_);
        sockfd_ = -1;
    }

    int sockfd_;
    sockaddr_in addr_;
};

int main(int, char*[]) {
    configure_databases();
    UserDatabase db;
    db.add({"ziomo", "spermula"});

    Server("127.0.0.1", 1100).run();
}
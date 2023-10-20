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
#include <jaszyk/json.hpp>
#include "wrappers.h"

using jaszyk::json;

class Client {
public:
    Client(const char* ip, int port)
        : socket_(-1) {
        addr_.sin_port = htons(port);
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = inet_addr(ip);

        // on ctrl-v
        register_signal(SIGINT, [this](int signum) {
            socket_.close();
            std::cout << "application closed due to an interrupt" << std::endl;
            exit(signum);
        });
    }

    void run() {
        buildClient();

        std::thread([&]() { // receiver
            while (socket_.fd() != -1) {
                json response = socket_.read();
                std::cout << "received: " << response << std::endl;
            }
            std::cout << "receiver closed" << std::endl;
        }).detach();

        while (true) {
            std::string msg;
            std::cin >> msg;
            if (msg == "exit") {
                break;
            }
            json data = json::dictionary();

            if (msg == "sendto") {
                data["action"] = 1;
                json::number fd = 0;
                std::cin >> fd;
                data["who"] = fd;

                std::string line;
                std::getline(std::cin, line);
                data["message"] = std::move(line);

                socket_.write(data);
            }
        }

        socket_.close();
        socket_ = Socket(-1);
    }
private:
    void buildClient() {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            std::cerr << "socket error" << std::endl;
            return;
        }
        if (connect(sockfd, (sockaddr*)&addr_, sizeof(addr_)) < 0) {
            std::cerr << "connect error" << std::endl;
            return;
        }
        socket_ = Socket(sockfd);
    }

    sockaddr_in addr_;
    Socket socket_;
};

int main(int, char*[]) {
    Client("127.0.0.1", 1100).run();
}
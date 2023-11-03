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
#include "core/socket.h"
#include "logger.h"
#include <csignal>
#include "core/utils/globals.h"
#include "core/utils/crypto.h"
#include <openssl/ssl.h>
#include <openssl/crypto.h>

using jaszyk::json;

class Client {
public:
    Client(const char* ip, int port)
        : socket_(-1) {
        addr_.sin_port = htons(port);
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = inet_addr(ip);
        m_crypto_.create();
        // on ctrl-v
        register_signal(SIGINT, [this](int signum) {
            socket_.close();
            std::cout << "application closed due to an interrupt" << std::endl;
            exit(signum);
        });
    }

    void run() {
        buildClient();
        {
            auto f = socket_.read();
            json init_data = json::parse(f);
            server_crypto_ = Crypto(init_data["public_key"].get<std::string>());
        }
        {
            json init_data = json::dictionary();
            init_data["action"] = "init";
            init_data["public_key"] = m_crypto_.public_key();
            std::cout << "sending public key to server..." << std::endl;
            socket_.write(init_data.to_string(), server_crypto_);
            std::cout << "public key sent" << std::endl;
        }
        std::cout << "client public key:\n" << m_crypto_.public_key() << std::endl;
        std::thread([&, this]() { // receiver
            
            while (socket_.fd() != -1) {
                json response = json::parse(this->socket_.read(this->m_crypto_));
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
                data["action"] = "sendto";
                std::string who;
                std::cin >> who;
                data["who"] = who;

                std::string line;
                std::cin.ignore(1);
                std::getline(std::cin, line);
                data["message"] = std::move(line);

                socket_.write(data.to_string(), server_crypto_);
            }
            if (msg == "register") {
                data["action"] = "register";
                std::cin >> msg;
                data["creds"] = json::dictionary();
                data["creds"]["username"] = msg;
                std::cin >> msg;
                data["creds"]["password"] = msg;

                socket_.write(data.to_string(), server_crypto_);
                msg = "";
            }
            if (msg == "login") {
                data["action"] = "login";
                std::cin >> msg;
                data["creds"] = json::dictionary();
                data["creds"]["username"] = msg;
                std::cin >> msg;
                data["creds"]["password"] = msg;

                socket_.write(data.to_string(), server_crypto_);
                msg = "";
            }
            if (msg == "online") {
                data["action"] = "usersOnline";
                socket_.write(data.to_string(), server_crypto_);
                msg = "";
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
    Crypto m_crypto_;
    Crypto server_crypto_;
};


int main(int argc, char* argv[]) {
    OpenSSL_add_all_algorithms();

    if (argc != 3) {
        Client("127.0.0.1", 42069).run();
    } else {
        const char* ip = argv[1];
        int port = atoi(argv[2]);
        
        logger.info() << "starting on " << ip << ':' << port << std::endl;
        Client(ip, port).run();
    }
}
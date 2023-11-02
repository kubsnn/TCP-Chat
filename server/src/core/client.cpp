#include "client.h"
#include <mutex>
#include <arpa/inet.h>
#include <cstring>

Client::Client(int sockfd, sockaddr_in addr)
    : socket_(sockfd)
    , username_("")
{
    ip_ = new char[kIpSize];
    inet_ntop(AF_INET, &addr.sin_addr, ip_, kIpSize);
}

Client::Client(Client&& other) noexcept
    : socket_(std::move(other.socket_))
    , username_(std::move(other.username_))
    , crypto_(std::move(other.crypto_))
{
    ip_ = other.ip_;
    other.ip_ = nullptr;
}

Client::Client(const Client& other)
    : socket_(other.socket_)
    , username_(other.username_)
    , crypto_(other.crypto_)
{
    ip_ = new char[kIpSize];
    std::copy(&other.ip_[0], &other.ip_[kIpSize], &ip_[0]);
}

bool Client::writeEncrypted(const std::string& data) const {
    return socket_.write(data, crypto_);
}

const Socket& Client::socket() const {
    return socket_;
}

const char* Client::ip() const {
    return ip_;
}

const std::string& Client::username() const {
    return username_;
}

const Crypto& Client::crypto() const {
    return crypto_;
}

bool Client::logged() const {
    return username_ != "";
}

void Client::setPublicKey(const std::string& key) {
    crypto_ = Crypto(key);
}

void Client::setUsername(std::string username) {
    username_ = std::move(username);
}

void Client::disconnect() {
    socket_.close();
}

Client& Client::operator=(Client&& other) noexcept {
    if (this == &other) return *this;
    
    if (ip_ != nullptr) delete[] ip_;

    socket_ = std::move(other.socket_);
    ip_ = other.ip_;
    username_ = std::move(other.username_);
    crypto_ = std::move(other.crypto_);
    other.ip_ = nullptr;

    return *this;
}

Client& Client::operator=(const Client& other) {
    if (this == &other) return *this;

    if (ip_ != nullptr) delete[] ip_;

    socket_ = other.socket_;
    ip_ = new char[kIpSize];
    username_ = other.username_;
    crypto_ = other.crypto_;
    std::copy(&other.ip_[0], &other.ip_[kIpSize], &ip_[0]);

    return *this;
}

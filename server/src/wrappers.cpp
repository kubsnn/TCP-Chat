#include "wrappers.h"

static std::function<void(int)> handler_;

void signal_wrapper(int signum) {
    if (handler_) {
        handler_(signum);
    }
}

void register_signal(int signum, std::function<void(int)> handler) {
    handler_ = std::move(handler);
    signal(signum, signal_wrapper);
}

// Socket

Socket::Socket(int fd) 
    : sockfd_(fd) 
{ }

Socket::Socket(Socket&& other) noexcept {
    sockfd_ = other.sockfd_;
    other.sockfd_ = -1;
}

Socket::Socket(const Socket& other) {
    sockfd_ = other.sockfd_;
}

Socket::~Socket() { }

json Socket::read() const {
    char buff[buffer_size];
    if (::read(sockfd_, reinterpret_cast<void*>(buff), std::size(buff)) <= 0) {
        throw std::runtime_error("connection has been lost");
    }

    return json::parse(buff);
}

bool Socket::write(const json& data) const {
    auto str = data.to_string();
    return ::write(sockfd_, str.c_str(), str.size() + 1) > 0;
}

void Socket::close() {
    ::close(sockfd_);
    sockfd_ = -1;
}

void Socket::ping() const {
    static constexpr auto msg = "PING";
    int count = ::write(sockfd_, msg, 5);
}

int Socket::fd() const {
    return sockfd_;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    sockfd_ = other.sockfd_;
    other.sockfd_ = -1;

    return *this;
}

Socket& Socket::operator=(const Socket &other) {
    sockfd_ = other.sockfd_;

    return *this;
}
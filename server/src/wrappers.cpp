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

namespace {
    inline int read_header(int fd) {
        int size = 0;
        if (::read(fd, &size, sizeof(int)) <= 0) {
            throw std::runtime_error("connection has been lost");
        }
        return size;
    }

    inline json read_data(int fd, const int message_size) {
        char* data = new char[message_size];
        char* buff = data;
        int bytes_left = message_size;
        while (bytes_left) {
            int count = ::read(fd, buff, bytes_left);
            if (count <= 0) {
                throw std::runtime_error("connection has been lost");
            }
            buff += count;
            bytes_left -= count;
        }
        auto message = json::parse({data, (unsigned int)message_size});
        delete[] data;
        return message;
    }
}

json Socket::read() const {
    int size = read_header(sockfd_);
    return read_data(sockfd_, size);
}

namespace {
    inline bool write_header(int fd, const std::string& message) {
        int size = message.size() + 1;
        if (::write(fd, &size, sizeof(int)) <= 0) return false;
        return true;
    }

    inline bool write_data(int fd, const std::string& message) {
        const char* data = message.c_str();
        int size = message.size() + 1; 
        while (size > 0) {
            int count = ::write(fd, data, size);
            if (count <= 0) return false;
            data += count;
            size -= count;
        }
        return true;
    }
}

bool Socket::write(const json& data) const {
    auto str = data.to_string();
    write_header(sockfd_, str);

    return write_data(sockfd_, str);
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
#include "socket.h"
#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <exception>

static std::function<void(int)> handler_;

static void signal_wrapper(int signum) {
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

    inline std::string read_data(int fd, const int message_size) {
        std::string data;
        data.resize(message_size);
        std::cout << "reading " << message_size << " bytes" << std::endl;
        char* buff = data.data();
        int bytes_left = message_size;
        while (bytes_left) {
            int count = ::read(fd, buff, bytes_left);
            if (count <= 0) {
                throw std::runtime_error("connection has been lost");
            }
            buff += count;
            bytes_left -= count;
        }
        return data;
    }

    inline void skip_bytes(int fd, int bytes_left) {
        char buff[512];
        while (bytes_left > 0) {
            int count = ::read(fd, buff, std::min((int)sizeof(buff), bytes_left));
            if (count <= 0) {
                throw std::runtime_error("connection has been lost");
            }
            bytes_left -= count;
        }
    }
}

std::string Socket::read() const {
    int size = read_header(sockfd_);
    if (size > 65536) {
        skip_bytes(sockfd_, size);
        return "{\"error\":true,\"result\":\"fail\",\"message\":\"request too big\"}";
    }
    return read_data(sockfd_, size);
}

std::string Socket::read(const Crypto &crypto) const {
    int size = read_header(sockfd_);
    if (size > 65536) {
        skip_bytes(sockfd_, size);
        return "{\"error\":true,\"result\":\"fail\",\"message\":\"request too big\"}";
    }
    std::cout << "reading " << size << " bytes" << std::endl;
    char buff[256];
    std::string data;
    data.resize(size);
    char* ptr = data.data();
    int bytes_left = size;
    while (bytes_left > 0) {
        int count = ::read(sockfd_, buff, std::min((int)sizeof(buff), bytes_left));
        std::cout << "reading " << count << " bytes" << std::endl;
        if (count <= 0) {
            throw std::runtime_error("connection has been lost");
        }
        crypto.decrypt(buff, count, ptr);
        ptr += 245;
        bytes_left -= count;
    }
    return data;
}

namespace {
    inline bool write_header(int fd, int message_size) {
        if (::write(fd, &message_size, sizeof(int)) <= 0) return false;
        return true;
    }

    inline bool write_header(int fd, const std::string& message) {
        return write_header(fd, message.size() + 1);
    }

    inline bool write_data(int fd, const char* data, int size) {
        while (size > 0) {
            int count = ::write(fd, data, size);
            if (count <= 0) return false;
            data += count;
            size -= count;
        }
        return true;
    }

    inline bool write_data(int fd, const std::string& message) {
        return write_data(fd, message.c_str(), message.size() + 1);
    }
}

bool Socket::write(const std::string& data) const {
    std::lock_guard<std::mutex> lock(mutexes[sockfd_]);
    write_header(sockfd_, data);
    return write_data(sockfd_, data);
}

bool Socket::write(const std::string &data, const Crypto& crypto) const {
    std::lock_guard<std::mutex> lock(mutexes[sockfd_]);

    static constexpr int block_size = 245;
    int size = data.size() + 1;
    int blocks = size / 256 + (size % 256 ? 1 : 0);
    
    // max size of data
    int total_size = blocks * 256;

    std::cout << "to write: " << total_size << " bytes" << std::endl;
    write_header(sockfd_, total_size);

    const char* ptr = data.c_str();

    char buff[256];
    while (size > 0) {
        std::fill(std::begin(buff), std::end(buff), '\0');
        int count = std::min(size, block_size);
        crypto.encrypt(ptr, count, buff); 
        std::cout << "writing " << count << " bytes" << std::endl;

        int r = 0;
        if (!(r = write_data(sockfd_, buff, 256))) {
            return false;
        }

        std::cout << "written " << r << " bytes" << std::endl;

        ptr += block_size;
        size -= count;
    }

    return true;
}

void Socket::close() {
    ::close(sockfd_);
    sockfd_ = -1;
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
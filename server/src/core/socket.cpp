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

    inline std::string read_data(int fd, int bytes_to_read) {
        std::string data;
        data.resize(bytes_to_read);

        char* buff = data.data();
        while (bytes_to_read) {
            int count = ::read(fd, buff, bytes_to_read);
            if (count <= 0) {
                throw std::runtime_error("connection has been lost");
            }
            buff += count;
            bytes_to_read -= count;
        }
        return data;
    }

    inline void read_data(int fd, int bytes_to_read, char* buff) {
        while (bytes_to_read) {
            int count = ::read(fd, buff, bytes_to_read);
            if (count <= 0) {
                throw std::runtime_error("connection has been lost");
            }
            buff += count;
            bytes_to_read -= count;
        }
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

std::string Socket::read(const Crypto& crypto) const {
    int size = read_header(sockfd_);
    if (size > 65536) {
        skip_bytes(sockfd_, size);
        return "{\"error\":true,\"result\":\"fail\",\"message\":\"request too big\"}";
    }
    char buff[Crypto::rsa_size];
    std::string data;
    data.resize(size);
    char* ptr = data.data();
    int bytes_left = size;
    while (bytes_left > 0) {
        std::cout << "reading " << Crypto::rsa_size << " bytes" << std::endl;
        read_data(sockfd_, Crypto::rsa_size, buff);
        crypto.decrypt(buff, Crypto::rsa_size, ptr);
        ptr += Crypto::max_data_size;
        bytes_left -= Crypto::rsa_size;
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

    int size = data.size() + 1;
    int blocks = size / Crypto::rsa_size + (size % Crypto::rsa_size ? 1 : 0);
    
    // max size of encrypted data
    int total_size = blocks * Crypto::rsa_size;

    write_header(sockfd_, total_size);

    const char* ptr = data.c_str();

    char buff[Crypto::rsa_size];
    while (size > 0) {
        std::fill(std::begin(buff), std::end(buff), '\0');
        int count = std::min(size, Crypto::max_data_size);
        crypto.encrypt(ptr, count, buff); 

        if (!write_data(sockfd_, buff, Crypto::rsa_size)) return false;

        ptr += Crypto::max_data_size;
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
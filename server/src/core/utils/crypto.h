#pragma once
#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string>

class Crypto {
public:
    static constexpr int key_size = 2048;
    static constexpr int max_data_size = 245;
    static constexpr int rsa_size = key_size / 8;

    Crypto();
    Crypto(const std::string& public_key);
    Crypto(const Crypto& other) = delete;
    Crypto(Crypto&& other) noexcept;
    ~Crypto();

    void create();

    std::string encrypt(const std::string& data) const;
    std::string decrypt(const std::string& data) const;

    void encrypt(const char* data, int len, char* encrypted) const;
    void decrypt(const char* data, int len, char* decrypted) const;

    std::string public_key() const;

    Crypto& operator=(const Crypto& other);
    Crypto& operator=(Crypto&& other) noexcept;
private:
    RSA* private_key_ = nullptr;
    RSA* public_key_ = nullptr;

public:
    static bool verifyKey(const std::string& key);
};


#endif //!__CRYPTO_H__
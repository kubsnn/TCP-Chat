#include "crypto.h"
#include <filesystem>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <iostream>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <iostream>

Crypto::Crypto() 
    : private_key_(nullptr), public_key_(nullptr)
{ }

Crypto::Crypto(const std::string& public_key) {
    public_key_ = nullptr;
    private_key_ = nullptr;
    BIO* bio = BIO_new(BIO_s_mem());
    BIO_write(bio, public_key.c_str(), public_key.size());
    PEM_read_bio_RSAPublicKey(bio, &public_key_, nullptr, nullptr);

    if (public_key_ == NULL) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    BIO_free(bio);
}

Crypto::Crypto(Crypto&& other) noexcept {
    private_key_ = other.private_key_;
    public_key_ = other.public_key_;
    other.private_key_ = nullptr;
    other.public_key_ = nullptr;
}

Crypto::~Crypto() {
    if (private_key_) RSA_free(private_key_);
    if (public_key_) {
        std::cout << "freeing public key" << std::endl;   
        RSA_free(public_key_);
    }
}

void Crypto::create() {
    RSA* rsa = RSA_new();
    BIGNUM* bne = BN_new();
    BN_set_word(bne, RSA_F4);
    RSA_generate_key_ex(rsa, key_size, bne, nullptr);

    private_key_ = rsa;
    
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(bio, rsa);

    char* buffer;
    size_t length = BIO_get_mem_data(bio, &buffer);

    std::string key(buffer, length);
    BIO_free(bio);

    bio = BIO_new(BIO_s_mem());
    BIO_write(bio, key.c_str(), key.size());
    PEM_read_bio_RSAPublicKey(bio, &public_key_, nullptr, nullptr);

    BIO_free(bio);

    if (public_key_ == NULL) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    BN_free(bne);
}

std::string Crypto::encrypt(const std::string& data) const {
    int encrypted_size = RSA_size(public_key_);
    auto ciphertext = new unsigned char[encrypted_size];
    int result = RSA_public_encrypt(data.size(), reinterpret_cast<const unsigned char*>(data.c_str()), ciphertext, public_key_, RSA_PKCS1_PADDING);

    if (result == -1) {
        ERR_print_errors_fp(stderr);
        delete[] ciphertext;
        return "";
    }

    std::string encrypted(reinterpret_cast<char*>(ciphertext), result);
    delete[] ciphertext;
    return encrypted;
}

std::string Crypto::decrypt(const std::string& data) const {
    if (private_key_ == nullptr) {
        throw std::runtime_error("this is only encrypt only instance, cannot decrypt");
    }
    int decrypted_size = RSA_size(private_key_);
    auto plaintext = new unsigned char[decrypted_size];
    int result = RSA_private_decrypt(data.size(), reinterpret_cast<const unsigned char*>(data.c_str()), plaintext, private_key_, RSA_PKCS1_PADDING);

    if (result == -1) {
        ERR_print_errors_fp(stderr);
        delete[] plaintext;
        return "";
    }

    std::string decrypted(reinterpret_cast<char*>(plaintext), result);
    delete[] plaintext;
    return decrypted;
}

void Crypto::encrypt(const char *data, int len, char* encrypted) const {
    if (public_key_ == nullptr) {
        throw std::runtime_error("public key is not set");
    }
    int encrypted_size = RSA_size(public_key_);

    int result = RSA_public_encrypt(len, reinterpret_cast<const unsigned char*>(data), reinterpret_cast<unsigned char*>(encrypted), public_key_, RSA_PKCS1_PADDING);

    if (result == -1) {
        ERR_print_errors_fp(stderr);
    }
}

void Crypto::decrypt(const char *data, int len, char* decrypted) const {
    if (private_key_ == nullptr) {
        throw std::runtime_error("this is only encrypt only instance, cannot decrypt");
    }
    int decrypted_size = RSA_size(private_key_);
    int result = RSA_private_decrypt(len, reinterpret_cast<const unsigned char*>(data), reinterpret_cast<unsigned char*>(decrypted), private_key_, RSA_PKCS1_PADDING);

    if (result == -1) {
        ERR_print_errors_fp(stderr);
    }
}

std::string Crypto::public_key() const {
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(bio, public_key_);
    char* buffer;
    size_t length = BIO_get_mem_data(bio, &buffer);
    std::string key(buffer, length);
    BIO_free(bio);

    return key;
}

Crypto& Crypto::operator=(const Crypto &other) {
    // copy public key
    if (other.public_key_ != nullptr) {
        RSA_free(public_key_);
    }

    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(bio, other.public_key_);

    PEM_read_bio_RSAPublicKey(bio, &public_key_, nullptr, nullptr);

    if (public_key_ == NULL) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    BIO_free(bio);

    // copy private key
    if (other.private_key_ != nullptr) {
        RSA_free(private_key_);
    }

    bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(bio, other.private_key_, nullptr, nullptr, 0, nullptr, nullptr);

    PEM_read_bio_RSAPrivateKey(bio, &private_key_, nullptr, nullptr);

    if (private_key_ == NULL) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    BIO_free(bio);
    
    return *this;
}

Crypto &Crypto::operator=(Crypto &&other) noexcept
{
    private_key_ = other.private_key_;
    public_key_ = other.public_key_;
    other.private_key_ = nullptr;
    other.public_key_ = nullptr;
    return *this;
}

bool Crypto::verifyKey(const std::string& key) {
    BIO* bio = BIO_new(BIO_s_mem());
    BIO_write(bio, key.c_str(), strlen(key.c_str()) + 1);
    RSA* rsa = nullptr;
    PEM_read_bio_RSAPublicKey(bio, &rsa, nullptr, nullptr);
    ERR_print_errors_fp(stderr);
    BIO_free(bio);

    if (rsa == nullptr) {
        return false;
    }
    RSA_free(rsa);
    return true;
}

#pragma GCC diagnostic pop
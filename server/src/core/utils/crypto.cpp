#include "crypto.h"
#include <filesystem>
#include <iostream>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <random>

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

Crypto::Crypto(const Crypto& other) 
    : private_key_(nullptr), public_key_(nullptr)
{
    if (other.public_key_ != nullptr) {
        public_key_ = copyPublicKey(other.public_key_);
    }
    if (other.private_key_ != nullptr) {
        private_key_ = copyPrivateKey(other.private_key_);
    }
}

Crypto::Crypto(Crypto&& other) noexcept {
    private_key_ = other.private_key_;
    public_key_ = other.public_key_;
    other.private_key_ = nullptr;
    other.public_key_ = nullptr;
}

Crypto::~Crypto() {
    if (private_key_) RSA_free(private_key_);
    if (public_key_) RSA_free(public_key_);
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
    //int encrypted_size = RSA_size(public_key_);
    int result = RSA_public_encrypt(len, reinterpret_cast<const unsigned char*>(data), reinterpret_cast<unsigned char*>(encrypted), public_key_, RSA_PKCS1_PADDING);

    if (result == -1) {
        ERR_print_errors_fp(stderr);
    }
}

void Crypto::decrypt(const char *data, int len, char* decrypted) const {
    if (private_key_ == nullptr) {
        throw std::runtime_error("this is only encrypt only instance, cannot decrypt");
    }
    //int decrypted_size = RSA_size(private_key_);
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

Crypto& Crypto::operator=(const Crypto& other) {
    if (this == &other) return *this;

    if (other.public_key_ != nullptr) RSA_free(public_key_);
    if (other.private_key_ != nullptr) RSA_free(private_key_);
    
    copyPublicKey(other.public_key_);
    copyPrivateKey(other.private_key_);
    
    return *this;
}

Crypto& Crypto::operator=(Crypto &&other) noexcept {
    private_key_ = other.private_key_;
    public_key_ = other.public_key_;
    other.private_key_ = nullptr;
    other.public_key_ = nullptr;
    return *this;
}

RSA* Crypto::copyPublicKey(const RSA* key) const {
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(bio, key);
    RSA* rsa = nullptr;
    PEM_read_bio_RSAPublicKey(bio, &rsa, nullptr, nullptr);
    ERR_print_errors_fp(stderr);
    BIO_free(bio);

    if (rsa == nullptr) {
        return nullptr;
    }
    return rsa;
}

RSA* Crypto::copyPrivateKey(const RSA* key) const {
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(bio, key, nullptr, nullptr, 0, nullptr, nullptr);
    RSA* rsa = nullptr;
    PEM_read_bio_RSAPrivateKey(bio, &rsa, nullptr, nullptr);
    ERR_print_errors_fp(stderr);
    BIO_free(bio);

    if (rsa == nullptr) {
        return nullptr;
    }
    return rsa;
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

std::string Crypto::sha256(const std::string& data) {
    return sha256(data.c_str(), data.size());
}

std::string Crypto::sha256(const char* data, int len) {
    std::string hash(SHA256_DIGEST_LENGTH, 0);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, len);
    SHA256_Final(reinterpret_cast<unsigned char*>(hash.data()), &sha256);
    
    return hash;
}

std::string Crypto::salt() {
    std::string s(14, '\0');
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 61);
    for (int i = 0; i < 14; ++i) {
        int r = dis(gen);
        if (r < 26) {
            s[i] = 'a' + r;
        } else if (r < 52) {
            s[i] = 'A' + r - 26;
        } else {
            s[i] = '0' + r - 52;
        }
    }
    return s;
}

std::string Crypto::base64Encode(const std::string& data) {
    BIO* bmem = nullptr;
    BIO* b64 = nullptr;
    BUF_MEM* bptr = nullptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, data.c_str(), data.size());
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    std::string result(bptr->data, bptr->length - 1);
    BIO_free_all(b64);

    return result;    
}

std::string Crypto::base64Decode(const std::string& data) {
    BIO* b64 = nullptr;
    BIO* bmem = nullptr;
    char* buffer = new char[data.size()];
    memset(buffer, 0, data.size());

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new_mem_buf(data.c_str(), data.size());
    bmem = BIO_push(b64, bmem);
    BIO_read(bmem, buffer, data.size());

    std::string result(buffer, data.size());
    BIO_free_all(bmem);
    delete[] buffer;

    return result;
}

int Crypto::random(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

#pragma GCC diagnostic pop
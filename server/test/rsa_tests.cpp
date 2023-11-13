#include <gtest/gtest.h>
#include <core/utils/crypto.h>

TEST(CryptoTest, RSA_EncryptDecrypt) {
    Crypto crypto;
    crypto.create();

    const std::string text = "test";
    char encrypted[Crypto::rsa_size];
    crypto.encrypt(text.c_str(), text.size(), encrypted);

    char decrypted[Crypto::rsa_size];
    crypto.decrypt(encrypted, Crypto::rsa_size, decrypted);

    EXPECT_EQ(std::string(decrypted), text);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
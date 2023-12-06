#include <gtest/gtest.h>
#include <controllers/account_controller.h>
#include <core/cache.h>
#include <core/client.h>
#include <json.hpp>
#include <core/database/user_db_handle.h>

struct AccountControllerTest : public ::testing::Test {
    AccountControllerTest() {
        cache = new Cache();
        client = new Client(-1, sockaddr_in{});
        account_controller = new AccountController(*client, *cache);
    }

    ~AccountControllerTest() {
        delete account_controller;
        delete cache;
        delete client;
    }

    AccountController* account_controller;
    Cache* cache;
    Client* client;
};

const json register_json = json::parse(R"(
{
    "action" : "register",
    "creds" : {
        "username" : "test",
        "password" : "test"
    }
}
)");

TEST_F(AccountControllerTest, Register) {
    auto result = account_controller->invoke("register", register_json);
    EXPECT_EQ(result["result"].get<std::string>(), "ok");

    UserDbHandle db;
    db.drop();
}

TEST_F(AccountControllerTest, RegisterUsernameAlreadyTaken) {
    account_controller->invoke("register", register_json);

    auto result = account_controller->invoke("register", register_json);
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    UserDbHandle db;
    db.drop();
}

const json login_json = json::parse(R"(
{
    "action" : "login",
    "creds" : {
        "username" : "test",
        "password" : "test"
    }
}
)");

TEST_F(AccountControllerTest, Login) {
    account_controller->invoke("register", login_json);

    EXPECT_EQ(cache->isUserOnline("test"), false);

    auto result = account_controller->invoke("login", login_json);
    EXPECT_EQ(result["result"].get<std::string>(), "ok");

    EXPECT_EQ(cache->isUserOnline("test"), true);

    UserDbHandle db;
    db.drop();
}

const json login_invalid_json = json::parse(R"(
{
    "action" : "login",
    "creds" : {
        "username" : "test",
        "password" : "test2"
    }
}
)");

TEST_F(AccountControllerTest, LoginInvalidPassword) {
    account_controller->invoke("register", login_json);

    EXPECT_EQ(cache->isUserOnline("test"), false);

    auto result = account_controller->invoke("login", login_invalid_json);
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    EXPECT_EQ(cache->isUserOnline("test"), false);

    UserDbHandle db;
    db.drop();
}

const json login_invalid_username_json = json::parse(R"(
{
    "action" : "login",
    "creds" : {
        "username" : "test2",
        "password" : "test"
    }
}
)");

TEST_F(AccountControllerTest, LoginInvalidUsername) {
    account_controller->invoke("register", login_json);

    EXPECT_EQ(cache->isUserOnline("test"), false);

    auto result = account_controller->invoke("login", login_invalid_username_json);
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    EXPECT_EQ(cache->isUserOnline("test"), false);

    UserDbHandle db;
    db.drop();
}

TEST_F(AccountControllerTest, LoginAlreadyLoggedIn) {
    account_controller->invoke("register", login_json);
    account_controller->invoke("login", login_json);

    EXPECT_EQ(cache->isUserOnline("test"), true);

    auto result = account_controller->invoke("login", login_json);
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    EXPECT_EQ(cache->isUserOnline("test"), true);

    UserDbHandle db;
    db.drop();
}


const json logout_json = json::parse(R"(
{
    "action" : "logout"
}
)");

TEST_F(AccountControllerTest, Logout) {
    account_controller->invoke("register", login_json);
    account_controller->invoke("login", login_json);

    EXPECT_EQ(cache->isUserOnline("test"), true);

    auto result = account_controller->invoke("logout", logout_json);
    EXPECT_EQ(result["result"].get<std::string>(), "ok");

    EXPECT_EQ(cache->isUserOnline("test"), false);

    UserDbHandle db;
    db.drop();
}

TEST_F(AccountControllerTest, LogoutNotLoggedIn) {
    account_controller->invoke("register", login_json);

    EXPECT_EQ(cache->isUserOnline("test"), false);

    auto result = account_controller->invoke("logout", logout_json);
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    EXPECT_EQ(cache->isUserOnline("test"), false);

    UserDbHandle db;
    db.drop();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
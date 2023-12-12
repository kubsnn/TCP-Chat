#include <gtest/gtest.h>
#include <controllers/controller.h>
#include <controllers/account_controller.h>
#include <core/cache.h>
#include <core/client.h>
#include <json.hpp>
#include <core/database/user_db_handle.h>


struct ControllerTest : public ::testing::Test {
    ControllerTest() {
        UserDbHandle db;
        db.drop();
        cache = new Cache();
        client1 = new Client(-1, sockaddr_in{});
        client2 = new Client(-1, sockaddr_in{});
        controller1 = new Controller(*client1, *cache);
        controller2 = new Controller(*client2, *cache);
        const auto creds1 = json::parse(R"(
        {
            "creds" : {
                "username" : "test1",
                "password" : "test"
            }
        }
        )");
        const auto creds2 = json::parse(R"(
        {
            "action" : "register",
            "creds" : {
                "username" : "test2",
                "password" : "test"
            }
        }
        )");
        AccountController(*client1, *cache).invoke("register", creds1);
        AccountController(*client2, *cache).invoke("register", creds2);
        AccountController(*client1, *cache).invoke("login", creds1);
        AccountController(*client2, *cache).invoke("login", creds2);
    }

    ~ControllerTest() {
        UserDbHandle db;
        db.drop();

        delete controller1;
        delete controller2;
        delete cache;
        delete client1;
        delete client2;
    }

    Controller* controller1;
    Controller* controller2;
    Cache* cache;
    Client* client1;
    Client* client2;
};

const json add_friend_json = json::parse(R"(
{
    "action" : "invite",
    "who" : "test2"
}
)");

TEST_F(ControllerTest, AddFriend) {
    auto result = controller1->invoke("invite", add_friend_json);
    EXPECT_EQ(result["result"].get<std::string>(), "ok");

    auto invitations = controller2->invoke("invitations", json::dictionary());
    EXPECT_EQ(invitations["result"].get<std::string>(), "ok");
    EXPECT_EQ(invitations["values"][0]["username"].get<std::string>(), "test1");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, AddFriendUserNotExists) {
    auto result = controller1->invoke("invite", json::parse(R"(
    {
        "action" : "invite",
        "who" : "test3"
    }
    )"));
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, AddFriendAlreadyInvited) {
    controller1->invoke("invite", add_friend_json);
    auto result = controller1->invoke("invite", add_friend_json);
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    UserDbHandle db;
    db.drop();
}

const json accept_friend_json = json::parse(R"(
{
    "action" : "accept",
    "who" : "test1"
}
)");

TEST_F(ControllerTest, AddFriendAccept) {
    controller1->invoke("invite", add_friend_json);
    controller2->invoke("accept", accept_friend_json);
    auto friends = controller2->invoke("friends", json::dictionary());
    EXPECT_EQ(friends["result"].get<std::string>(), "ok");
    EXPECT_EQ(friends["values"][0]["username"].get<std::string>(), "test1");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, AddFriendAlreadyFriends) {
    controller1->invoke("invite", add_friend_json);
    controller2->invoke("accept", accept_friend_json);
    auto result = controller1->invoke("invite", add_friend_json);
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, AddFriendAlreadyInvitedByOther) {
    controller1->invoke("invite", add_friend_json);
    auto result = controller2->invoke("invite", json::parse(R"(
    {
        "action" : "invite",
        "who" : "test1"
    }
    )"));
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, AddFriendAcceptNotInvited) {
    auto result = controller2->invoke("accept", accept_friend_json);
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, RemoveFriend) {
    controller1->invoke("invite", add_friend_json);
    controller2->invoke("accept", accept_friend_json);
    auto result = controller1->invoke("removeFriend", {
        { "action", "removeFriend" },
        { "who", "test2" }
    });
    EXPECT_EQ(result["result"].get<std::string>(), "ok");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, RemoveFriendNotFriends) {
    auto result = controller1->invoke("removeFriend", {
        { "action", "removeFriend" },
        { "who", "test2" }
    });
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, RemoveFriendNotInvited) {
    controller1->invoke("invite", add_friend_json);
    auto result = controller1->invoke("removeFriend", {
        { "action", "removeFriend" },
        { "who", "test2" }
    });
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, RemoveFriendNotInvitedByOther) {
    controller1->invoke("invite", add_friend_json);
    auto result = controller2->invoke("removeFriend", {
        { "action", "removeFriend" },
        { "who", "test1" }
    });
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, RemoveFriendNotExists) {
    auto result = controller1->invoke("removeFriend", {
        { "action", "removeFriend" },
        { "who", "test3" }
    });
    EXPECT_EQ(result["result"].get<std::string>(), "fail");

    UserDbHandle db;
    db.drop();
}

TEST_F(ControllerTest, GetFriends) {
    controller1->invoke("invite", add_friend_json);
    controller2->invoke("accept", accept_friend_json);
    auto friends = controller1->invoke("friends", json::dictionary());
    EXPECT_EQ(friends["result"].get<std::string>(), "ok");
    EXPECT_EQ(friends["values"][0]["username"].get<std::string>(), "test2");

    UserDbHandle db;
    db.drop();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
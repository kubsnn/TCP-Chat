#include <gtest/gtest.h>

int Add(int a, int b) {
    return a + b;
}

TEST(AddTest, HandlesPositiveInput) {
    EXPECT_EQ(2, Add(1, 1));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
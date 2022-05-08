#include <module2/hoo.hpp>
#include <gtest/gtest.h>

TEST(Foo, footest)
{
    Foo v(3);
    EXPECT_EQ(3, v.data_member);
    EXPECT_EQ(3, v.member_function());
    EXPECT_EQ(3, v.member_function2());
    EXPECT_EQ(3, v.member_function3());
}
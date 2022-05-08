#include <module1/incl.hpp>
#include <gtest/gtest.h>

TEST(Incl, Incl) {
    EXPECT_EQ(2, incl(1));
    EXPECT_EQ(3, incl(2));
}
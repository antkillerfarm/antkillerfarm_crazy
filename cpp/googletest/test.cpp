#include "gtest/gtest.h"

int max(int a, int b)
{
return a>b?a:b;
}

TEST(foo, max)
{
EXPECT_EQ(2, max(2,-1));
EXPECT_EQ(3, max(2,3));
}
 
int main(int argc, char** argv)
{
::testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}

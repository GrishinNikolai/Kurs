#include <UnitTest++/UnitTest++.h>

SUITE(MinimalTest)
{
    TEST(BasicTest) {
        CHECK(true);
    }
    
    TEST(AnotherTest) {
        CHECK_EQUAL(2, 1+1);
    }
}

int main() {
    return UnitTest::RunAllTests();
}

#include <gmock/gmock.h>

class ui_test : public testing::Test {
 public:
};

TEST_F(ui_test, CheckSomeResults) { EXPECT_EQ((unsigned int)21, int(21)); }

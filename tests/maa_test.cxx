#include "maa.h"
#include "gtest/gtest.h"

using namespace maa;

TEST (basic, GetVersion) {
    ASSERT_EQ(get_version(), 1);
}

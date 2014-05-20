#include <maa.h>
#include "gtest/gtest.h"
#include "version.h"

/* Careful, this test will only attempt to check the returned version is valid,
 * it doesn't try to check the version is a release one.
 */
TEST (basic, maa_get_version) {
    char bar[64];
    strcpy(bar, maa_get_version());
    ASSERT_STREQ(maa_get_version(), gVERSION);
}

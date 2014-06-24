#include <mraa.h>
#include "gtest/gtest.h"
#include "version.h"

/* Careful, this test will only attempt to check the returned version is valid,
 * it doesn't try to check the version is a release one.
 */
TEST (basic, mraa_get_version) {
    char bar[64];
    strcpy(bar, mraa_get_version());
    ASSERT_STREQ(mraa_get_version(), gVERSION);
}

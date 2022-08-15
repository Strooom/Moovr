#include <unity.h>
#include "gcodeword.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test


void initialization() {
    TEST_IGNORE();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    UNITY_END();
}
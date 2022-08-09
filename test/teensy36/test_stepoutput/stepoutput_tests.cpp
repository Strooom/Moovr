// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <stdint.h>
#include <unity.h>

void initialization() {
    TEST_ABORT();
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    UNITY_END();
}

void loop() {
}

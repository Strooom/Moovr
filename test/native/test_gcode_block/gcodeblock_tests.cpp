#include <unity.h>
#include "gcodeblock.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void initialization() {
    gCodeBlock aBlock;
    TEST_ASSERT_EQUAL(0, aBlock.getNmbrWords());        // empty after construction

    aBlock.nmbrWords = 1;
    aBlock.initialize();
    TEST_ASSERT_EQUAL(0, aBlock.getNmbrWords());        // empty after re-initialization
}

void wordCount() {
    gCodeBlock aBlock;
    aBlock.getBlockFromString("X100 G1");
    TEST_ASSERT_EQUAL(2, aBlock.getNmbrWords());
    aBlock.initialize();        // re-initialize
    aBlock.getBlockFromString("L9 X100 Y200 Z300 G1 I4 J5 K6 F700");
    TEST_ASSERT_EQUAL(9, aBlock.getNmbrWords());
}

void wordSearchAndRemove() {
    gCodeBlock aBlock;
    TEST_ASSERT_EQUAL(-1, aBlock.searchWord('G', 10));
    aBlock.getBlockFromString("L9 X100 Y200 Z300 G1 I4 J5 K6 F700");
    TEST_ASSERT_EQUAL(4, aBlock.searchWord('G', 10));
    TEST_ASSERT_EQUAL(-1, aBlock.searchWord('M', 10));
    aBlock.removeWord(aBlock.searchWord('L'));
    aBlock.removeWord(aBlock.searchWord('Y'));
    aBlock.removeWord(aBlock.searchWord('F'));
    aBlock.removeWord(aBlock.searchWord('M'));
    TEST_ASSERT_EQUAL(6, aBlock.getNmbrWords());
}

void modalGroupViolation() {
    gCodeBlock aBlock;
    aBlock.initialize();
    TEST_ASSERT_FALSE(aBlock.hasModalGroupViolation());
    aBlock.getBlockFromString("L9 X100 Y200 Z300 G1 I4 J5 K6 F700");
    TEST_ASSERT_FALSE(aBlock.hasModalGroupViolation());
    aBlock.getBlockFromString("G0 G1");
    TEST_ASSERT_TRUE(aBlock.hasModalGroupViolation());
}

void letterWordViolation() {
    gCodeBlock aBlock;
    aBlock.initialize();
    TEST_ASSERT_FALSE(aBlock.hasLetterWordViolation());
    aBlock.getBlockFromString("L9 X100 Y200 Z300 G1 I4 J5 K6 F700");
    TEST_ASSERT_FALSE(aBlock.hasLetterWordViolation());
    aBlock.getBlockFromString("X0 X1");
    TEST_ASSERT_TRUE(aBlock.hasLetterWordViolation());
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    RUN_TEST(wordCount);
    RUN_TEST(wordSearchAndRemove);
    RUN_TEST(modalGroupViolation);
    RUN_TEST(letterWordViolation);
    UNITY_END();
}

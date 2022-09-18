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

// TODO : get floating point values from words, in all kinds of formatting..
// TODO : works for uppercase or lowercase or mixed case words
// TODO : test all boundary conditions for properly detecting words from string
// TODO : do comments work ?
    // TODO : test boundaries.. > maxNmbrwords


void hasAxis() {
    gCodeBlock aBlock;
    aBlock.getBlockFromString("S100");
    TEST_ASSERT_FALSE(aBlock.hasAxis());
    aBlock.getBlockFromString("X100");
    TEST_ASSERT_TRUE(aBlock.hasAxis());
    aBlock.getBlockFromString("Y100");
    TEST_ASSERT_TRUE(aBlock.hasAxis());
    aBlock.getBlockFromString("Z100");
    TEST_ASSERT_TRUE(aBlock.hasAxis());
    aBlock.getBlockFromString("X100 Y200 Z300");
    TEST_ASSERT_TRUE(aBlock.hasAxis());
}

void hasOffsets() {
    gCodeBlock aBlock;
    aBlock.getBlockFromString("S100");
    TEST_ASSERT_FALSE(aBlock.hasOffset());
    aBlock.getBlockFromString("I100");
    TEST_ASSERT_TRUE(aBlock.hasOffset());
    aBlock.getBlockFromString("J100");
    TEST_ASSERT_TRUE(aBlock.hasOffset());
    aBlock.getBlockFromString("K100");
    TEST_ASSERT_TRUE(aBlock.hasOffset());
    aBlock.getBlockFromString("I100 J200 K300");
    TEST_ASSERT_TRUE(aBlock.hasOffset());
}

void hasRadius() {
    gCodeBlock aBlock;
    aBlock.getBlockFromString("S100");
    TEST_ASSERT_FALSE(aBlock.hasRadius());
    aBlock.getBlockFromString("R10");
    TEST_ASSERT_TRUE(aBlock.hasRadius());
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
    aBlock.initialize();
    aBlock.getBlockFromString("G61 G64");
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


void numberDetection() {
    gCodeBlock aBlock;
    aBlock.initialize();
    aBlock.getBlockFromString("X5");
    TEST_ASSERT_EQUAL(5, aBlock.gCodeWords[0].number);
    aBlock.initialize();
    aBlock.getBlockFromString("X00005");
    TEST_ASSERT_EQUAL(5, aBlock.gCodeWords[0].number);
    aBlock.initialize();
    aBlock.getBlockFromString("X.005");
    TEST_ASSERT_EQUAL(0.005, aBlock.gCodeWords[0].number);
    aBlock.initialize();
    aBlock.getBlockFromString("X005.");
    TEST_ASSERT_EQUAL(5, aBlock.gCodeWords[0].number);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    RUN_TEST(wordCount);
    RUN_TEST(hasAxis);
    RUN_TEST(hasOffsets);
    RUN_TEST(hasRadius);
    RUN_TEST(wordSearchAndRemove);
    RUN_TEST(modalGroupViolation);
    RUN_TEST(letterWordViolation);
    RUN_TEST(numberDetection);
    UNITY_END();
}

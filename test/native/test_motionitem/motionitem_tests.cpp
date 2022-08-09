#include <unity.h>
#include "motionitem.h"
#include "machineproperties.h"
#include "overrides.h"
#include <math.h>

#include "step.h"
#include "event.h"

// TEST_ASSERT_FLOAT_WITHIN() : Use this for testing if a float is almost the same, within a certain error band

// ----------------------------------
// --- globals                    ---
// ----------------------------------

machineProperties theMachineProperties;        //

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void initializeMotionPart() {
    speedProfileAccDec theMotionPart = speedProfileAccDec(1.1f, 2.2f, 3.3f, 4.4f, 5.5f);        // constructing a motionpart with certain inputs
    TEST_ASSERT_EQUAL_FLOAT(1.1f, theMotionPart.vStart);                                        // inputs should be copied
    TEST_ASSERT_EQUAL_FLOAT(2.2f, theMotionPart.vEnd);
    TEST_ASSERT_EQUAL_FLOAT(3.3f, theMotionPart.aMax);
    TEST_ASSERT_EQUAL_FLOAT(4.4f, theMotionPart.dMax);
    TEST_ASSERT_EQUAL_FLOAT(5.5f, theMotionPart.jMax);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, theMotionPart.dV1);        // output should still be zero
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theMotionPart.s1);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theMotionPart.s2);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theMotionPart.s3);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theMotionPart.length);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theMotionPart.d1d3);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theMotionPart.d2);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theMotionPart.duration);
}

void calculateMotionPartFirstOrder() {
    {
        speedProfileAccDec firstOrderMotion = speedProfileAccDec(0.0f, 1.0f, 1.0f, -1.0f, 10.0f);        // simple acceleration, starting from vStart = 0
        firstOrderMotion.calculate(speedProfileOrder::firstOrder);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.vStart);        // inputs should be unchanged
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.vEnd);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.aMax);
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, firstOrderMotion.dMax);
        TEST_ASSERT_EQUAL_FLOAT(10.0f, firstOrderMotion.jMax);

        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.dV1);        // outputs should be calculated correctly
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s1);
        TEST_ASSERT_EQUAL_FLOAT(0.5f, firstOrderMotion.s2);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s3);
        TEST_ASSERT_EQUAL_FLOAT(0.5f, firstOrderMotion.length);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.d1d3);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.d2);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.duration);

        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.a(0.0f));        // checking a(t), v(t) and s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.a(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.a(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.v(0.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.5f, firstOrderMotion.v(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.v(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s(0.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.125f, firstOrderMotion.s(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(0.5f, firstOrderMotion.s(1.0f));
    }
    {
        speedProfileAccDec firstOrderMotion = speedProfileAccDec(1.0f, 0.0f, 1.0f, -1.0f, 10.0f);        // simple deceleration, ending in vEnd = 0
        firstOrderMotion.calculate(speedProfileOrder::firstOrder);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.vStart);        // inputs should be unchanged
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.vEnd);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.aMax);
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, firstOrderMotion.dMax);
        TEST_ASSERT_EQUAL_FLOAT(10.0f, firstOrderMotion.jMax);

        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.dV1);        // outputs should be calculated correctly
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s1);
        TEST_ASSERT_EQUAL_FLOAT(0.5f, firstOrderMotion.s2);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s3);
        TEST_ASSERT_EQUAL_FLOAT(0.5f, firstOrderMotion.length);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.d1d3);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.d2);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.duration);

        TEST_ASSERT_EQUAL_FLOAT(-1.0f, firstOrderMotion.a(0.0f));        // checking a(t), v(t) and s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, firstOrderMotion.a(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, firstOrderMotion.a(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.v(0.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.5f, firstOrderMotion.v(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.v(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s(0.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.375f, firstOrderMotion.s(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(0.5f, firstOrderMotion.s(1.0f));
    }
    {
        speedProfileAccDec firstOrderMotion = speedProfileAccDec(1.0f, 2.0f, 1.0f, -1.0f, 10.0f);        // more complex case, starting from vStart = 1
        firstOrderMotion.calculate(speedProfileOrder::firstOrder);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.vStart);        // inputs should be unchanged
        TEST_ASSERT_EQUAL_FLOAT(2.0f, firstOrderMotion.vEnd);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.aMax);
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, firstOrderMotion.dMax);
        TEST_ASSERT_EQUAL_FLOAT(10.0f, firstOrderMotion.jMax);

        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.dV1);        // outputs should be calculated correctly
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s1);
        TEST_ASSERT_EQUAL_FLOAT(1.5f, firstOrderMotion.s2);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s3);
        TEST_ASSERT_EQUAL_FLOAT(1.5f, firstOrderMotion.length);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.d1d3);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.d2);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.duration);

        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.a(0.0f));        // checking a(t), v(t) and s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.a(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.a(1.0f));

        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.v(0.0f));
        TEST_ASSERT_EQUAL_FLOAT(1.5f, firstOrderMotion.v(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(2.0f, firstOrderMotion.v(1.0f));

        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s(0.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.625f, firstOrderMotion.s(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.5f, firstOrderMotion.s(1.0f));
    }
    {
        speedProfileAccDec firstOrderMotion = speedProfileAccDec(2.0f, 1.0f, 1.0f, -1.0f, 10.0f);        // more complex case,  ending in vEnd = 1
        firstOrderMotion.calculate(speedProfileOrder::firstOrder);
        TEST_ASSERT_EQUAL_FLOAT(2.0f, firstOrderMotion.vStart);        // inputs should be unchanged
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.vEnd);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.aMax);
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, firstOrderMotion.dMax);
        TEST_ASSERT_EQUAL_FLOAT(10.0f, firstOrderMotion.jMax);

        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.dV1);        // outputs should be calculated correctly
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s1);
        TEST_ASSERT_EQUAL_FLOAT(1.5f, firstOrderMotion.s2);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s3);
        TEST_ASSERT_EQUAL_FLOAT(1.5f, firstOrderMotion.length);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.d1d3);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.d2);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.duration);

        TEST_ASSERT_EQUAL_FLOAT(-1.0f, firstOrderMotion.a(0.0f));        // checking a(t), v(t) and s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, firstOrderMotion.a(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, firstOrderMotion.a(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(2.0f, firstOrderMotion.v(0.0f));
        TEST_ASSERT_EQUAL_FLOAT(1.5f, firstOrderMotion.v(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, firstOrderMotion.v(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.0f, firstOrderMotion.s(0.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.875f, firstOrderMotion.s(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.5f, firstOrderMotion.s(1.0f));
    }
}

void calculateMotionPartSecondOrder() {
    {
        speedProfileAccDec secondOrderMotion = speedProfileAccDec(0.0f, 2.0f, 1.0f, -1.0f, 1.0f);        // simple acceleration
        secondOrderMotion.calculate(speedProfileOrder::secondOrder);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.vStart);        // inputs should be unchanged
        TEST_ASSERT_EQUAL_FLOAT(2.0f, secondOrderMotion.vEnd);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.aMax);
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, secondOrderMotion.dMax);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.jMax);

        TEST_ASSERT_EQUAL_FLOAT(0.5f, secondOrderMotion.dV1);        // outputs should be calculated correctly
        TEST_ASSERT_EQUAL_FLOAT((1.0f / 6.0f), secondOrderMotion.s1);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.s2);
        TEST_ASSERT_EQUAL_FLOAT((2.0f - (1.0f / 6.0f)), secondOrderMotion.s3);
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.length);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.d1d3);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.d2);
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.duration);

        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.a(0.0f));        // checking a(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(0.5f, secondOrderMotion.a(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.a(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.a(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.a(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.5f, secondOrderMotion.a(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.a(3.0f));

        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.v(0.0f));        // checking v(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(0.125f, secondOrderMotion.v(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(0.5f, secondOrderMotion.v(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.v(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.5f, secondOrderMotion.v(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(1.875f, secondOrderMotion.v(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(2.0f, secondOrderMotion.v(3.0f));

        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.s(0.0f));        // checking s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT((0.125f / 6.0f), secondOrderMotion.s(0.5f));
        TEST_ASSERT_EQUAL_FLOAT((1.0f / 6.0f), secondOrderMotion.s(1.0f));
        TEST_ASSERT_EQUAL_FLOAT((1.0f / 6.0f) + 0.375f, secondOrderMotion.s(1.5f));
        TEST_ASSERT_EQUAL_FLOAT((1.0f / 6.0f) + 1.0f, secondOrderMotion.s(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(2.0f + (0.125f / 6.0f), secondOrderMotion.s(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.s(3.0f));
    }
    {
        speedProfileAccDec secondOrderMotion = speedProfileAccDec(2.0f, 0.0f, 1.0f, -1.0f, 1.0f);        // simple deceleration
        secondOrderMotion.calculate(speedProfileOrder::secondOrder);
        TEST_ASSERT_EQUAL_FLOAT(2.0f, secondOrderMotion.vStart);        // inputs should be unchanged
        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.vEnd);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.aMax);
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, secondOrderMotion.dMax);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.jMax);

        TEST_ASSERT_EQUAL_FLOAT(-0.5f, secondOrderMotion.dV1);        // outputs should be calculated correctly
        TEST_ASSERT_EQUAL_FLOAT((2.0f - (1.0f / 6.0f)), secondOrderMotion.s1);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.s2);
        TEST_ASSERT_EQUAL_FLOAT((1.0f / 6.0f), secondOrderMotion.s3);
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.length);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.d1d3);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.d2);
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.duration);

        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.a(0.0f));        // checking a(t), v(t) and s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(-0.5f, secondOrderMotion.a(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, secondOrderMotion.a(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, secondOrderMotion.a(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, secondOrderMotion.a(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(-0.5f, secondOrderMotion.a(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.a(3.0f));

        TEST_ASSERT_EQUAL_FLOAT(2.0f, secondOrderMotion.v(0.0f));        // checking a(t), v(t) and s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(1.875f, secondOrderMotion.v(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.5f, secondOrderMotion.v(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.v(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(0.5f, secondOrderMotion.v(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.125f, secondOrderMotion.v(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.v(3.0f));

        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.s(0.0f));        // checking s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(1.0f - (0.125f / 6.0f), secondOrderMotion.s(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(2.0f - (1.0f / 6.0f), secondOrderMotion.s(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(2.625f - (1.0f / 6.0f), secondOrderMotion.s(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(3.0f - (1.0f / 6.0f), secondOrderMotion.s(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(3.0f - (0.125f / 6.0f), secondOrderMotion.s(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.s(3.0f));
    }
    {
        speedProfileAccDec secondOrderMotion = speedProfileAccDec(1.0f, 3.0f, 1.0f, -1.0f, 1.0f);        // more complex case with vStart = 1
        secondOrderMotion.calculate(speedProfileOrder::secondOrder);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.vStart);        // inputs should be unchanged
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.vEnd);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.aMax);
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, secondOrderMotion.dMax);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.jMax);

        TEST_ASSERT_EQUAL_FLOAT(0.5f, secondOrderMotion.dV1);        // outputs should be calculated correctly
        TEST_ASSERT_EQUAL_FLOAT(1.0f + (1.0f / 6.0f), secondOrderMotion.s1);
        TEST_ASSERT_EQUAL_FLOAT(2.0f, secondOrderMotion.s2);
        TEST_ASSERT_EQUAL_FLOAT((3.0f - (1.0f / 6.0f)), secondOrderMotion.s3);
        TEST_ASSERT_EQUAL_FLOAT(6.0f, secondOrderMotion.length);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.d1d3);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.d2);
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.duration);

        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.a(0.0f));        // checking a(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(0.5f, secondOrderMotion.a(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.a(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.a(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.a(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(0.5f, secondOrderMotion.a(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.a(3.0f));

        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.v(0.0f));        // checking v(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(1.125f, secondOrderMotion.v(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.5f, secondOrderMotion.v(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(2.0f, secondOrderMotion.v(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(2.5f, secondOrderMotion.v(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(2.875f, secondOrderMotion.v(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.v(3.0f));

        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.s(0.0f));        // checking s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(0.5f + (0.125f / 6.0f), secondOrderMotion.s(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f + (1.0f / 6.0f), secondOrderMotion.s(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(1.5f + (1.0f / 6.0f) + 0.375f, secondOrderMotion.s(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(2.0f + (1.0f / 6.0f) + 1.0f, secondOrderMotion.s(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(4.5f + (0.125f / 6.0f), secondOrderMotion.s(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(6.0f, secondOrderMotion.s(3.0f));
    }
    {
        speedProfileAccDec secondOrderMotion = speedProfileAccDec(3.0f, 1.0f, 1.0f, -1.0f, 1.0f);        // more complex case with vEnd = 1
        secondOrderMotion.calculate(speedProfileOrder::secondOrder);
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.vStart);        // inputs should be unchanged
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.vEnd);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.aMax);
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, secondOrderMotion.dMax);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.jMax);

        TEST_ASSERT_EQUAL_FLOAT(-0.5f, secondOrderMotion.dV1);        // outputs should be calculated correctly
        TEST_ASSERT_EQUAL_FLOAT((3.0f - (1.0f / 6.0f)), secondOrderMotion.s1);
        TEST_ASSERT_EQUAL_FLOAT(2.0f, secondOrderMotion.s2);
        TEST_ASSERT_EQUAL_FLOAT(1.0f + (1.0f / 6.0f), secondOrderMotion.s3);
        TEST_ASSERT_EQUAL_FLOAT(6.0f, secondOrderMotion.length);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.d1d3);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.d2);
        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.duration);

        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.a(0.0f));        // checking a(t), v(t) and s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(-0.5f, secondOrderMotion.a(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, secondOrderMotion.a(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, secondOrderMotion.a(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, secondOrderMotion.a(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(-0.5f, secondOrderMotion.a(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.a(3.0f));

        TEST_ASSERT_EQUAL_FLOAT(3.0f, secondOrderMotion.v(0.0f));        // checking a(t), v(t) and s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(2.875f, secondOrderMotion.v(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(2.5f, secondOrderMotion.v(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(2.0f, secondOrderMotion.v(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.5f, secondOrderMotion.v(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(1.125f, secondOrderMotion.v(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(1.0f, secondOrderMotion.v(3.0f));

        TEST_ASSERT_EQUAL_FLOAT(0.0f, secondOrderMotion.s(0.0f));        // checking s(t) on all boundaries
        TEST_ASSERT_EQUAL_FLOAT(1.5f - (0.125f / 6.0f), secondOrderMotion.s(0.5f));
        TEST_ASSERT_EQUAL_FLOAT(3.0f - (1.0f / 6.0f), secondOrderMotion.s(1.0f));
        TEST_ASSERT_EQUAL_FLOAT(4.125f - (1.0f / 6.0f), secondOrderMotion.s(1.5f));
        TEST_ASSERT_EQUAL_FLOAT(5.0f - (1.0f / 6.0f), secondOrderMotion.s(2.0f));
        TEST_ASSERT_EQUAL_FLOAT(5.5f - (0.125f / 6.0f), secondOrderMotion.s(2.5f));
        TEST_ASSERT_EQUAL_FLOAT(6.0f, secondOrderMotion.s(3.0f));
    }
}

void vTri() {
    TEST_ASSERT_EQUAL_FLOAT(2.0f, motion::vTri(1.0f, 1.0f, 1.0f, -1.0f, 3.0f, motionStrategy::maximizeSpeed));
    TEST_ASSERT_EQUAL_FLOAT(1.0f, motion::vTri(2.0f, 2.0f, 1.0f, -1.0f, 3.0f, motionStrategy::minimizeSpeed));
    TEST_ASSERT_EQUAL_FLOAT(1.0f, motion::vTri(0.0f, 0.0f, 1.0f, -1.0f, 1.0f, motionStrategy::maximizeSpeed));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, motion::vTri(1.0f, 1.0f, 1.0f, -1.0f, 1.0f, motionStrategy::minimizeSpeed));
    // TODO : test the case where Vtri would go below zero in a minimizeSpeed scenario
}

void vOut() {
    TEST_ASSERT_EQUAL_FLOAT(2.0f, motion::vOut(1.0f, 1.0f, -1.0f, 1.5f, motionStrategy::maximizeSpeed, motionCalculateDirection::forward));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, motion::vOut(1.0f, 1.0f, -1.0f, 1.5f, motionStrategy::minimizeSpeed, motionCalculateDirection::forward));
    TEST_ASSERT_EQUAL_FLOAT(2.0f, motion::vOut(1.0f, 1.0f, -1.0f, 1.5f, motionStrategy::maximizeSpeed, motionCalculateDirection::reverse));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, motion::vOut(1.0f, 1.0f, -1.0f, 1.5f, motionStrategy::minimizeSpeed, motionCalculateDirection::reverse));
}

void setTrajectory() {
    simplifiedMotion aMotion;              // this has all zeroes as default
    motionTrajectory theTrajectory;        // this has no defaults
    theTrajectory.set(aMotion);            // copy properties
    for (int axisIndex = 0; axisIndex < nmbrAxis; axisIndex++) {
        TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.startPosition[axisIndex]);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.delta[axisIndex]);
    }
    TEST_ASSERT_EQUAL(axis::nmbrAxis, theTrajectory.arcAxis0);
    TEST_ASSERT_EQUAL(axis::nmbrAxis, theTrajectory.arcAxis1);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.arcCenter0);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.arcCenter1);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.startAngle);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.deltaAngle);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.radius);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.length);

    aMotion.setForTest(0U);
    theTrajectory.set(aMotion);

    TEST_ASSERT_EQUAL_FLOAT(1.0f, theTrajectory.deltaRealTime[0]);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, theTrajectory.directionUnitVector[0]);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.deltaRealTime[1]);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.directionUnitVector[1]);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.deltaRealTime[2]);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, theTrajectory.directionUnitVector[2]);

    // TODO : test some other cases eg. helical move
}

void setSpeedProfile() {
    {
        simplifiedMotion aMotion;
        aMotion.setForTest(0U);
        motionSpeedProfile speedProfile;
        speedProfile.setSpeed(aMotion);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, speedProfile.left.aMax);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, speedProfile.left.dMax);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, speedProfile.left.jMax);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, speedProfile.right.aMax);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, speedProfile.right.aMax);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, speedProfile.right.aMax);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, speedProfile.left.vStart);
        TEST_ASSERT_EQUAL_FLOAT(0.0f, speedProfile.right.vEnd);
    }
    {
        simplifiedMotion aMotion;
        aMotion.setForTest(40U);
        motionSpeedProfile speedProfile;
        speedProfile.setDuration(aMotion);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, speedProfile.duration);
    }
}

void limit() {
    theMachineProperties.motors.aMax[0] = 1.0f;        // WARNING : side effect to other tests as this is a global variable
    theMachineProperties.motors.dMax[0] = -1.0f;
    theMachineProperties.motors.vMax[0] = 1.0f;
    theMachineProperties.motors.aMax[1] = 1.0f;
    theMachineProperties.motors.dMax[1] = -1.0f;
    theMachineProperties.motors.vMax[1] = 1.0f;
    theMachineProperties.motors.aMax[2] = 1.0f;
    theMachineProperties.motors.dMax[2] = -1.0f;
    theMachineProperties.motors.vMax[2] = 1.0f;

    // test with 1 axis
    {
        simplifiedMotion aSimpleMotion;
        motion aMotion;
        aSimpleMotion.setForTest(0U);
        aMotion.set(aSimpleMotion);
        aMotion.limit();
        TEST_ASSERT_EQUAL_FLOAT(1.0f, aMotion.speedProfile.aMax);
        TEST_ASSERT_EQUAL_FLOAT(-1.0f, aMotion.speedProfile.dMax);
        TEST_ASSERT_EQUAL_FLOAT(1.0f, aMotion.speedProfile.vMax);
    }
    // test with 3 axis'
    {
        simplifiedMotion aSimpleMotion;
        motion aMotion;
        aSimpleMotion.setForTest(1U);
        aMotion.set(aSimpleMotion);
        aMotion.limit();
        TEST_ASSERT_EQUAL_FLOAT(sqrt(3.0f), aMotion.speedProfile.vMax);
        TEST_ASSERT_EQUAL_FLOAT(sqrt(3.0f), aMotion.speedProfile.aMax);
        TEST_ASSERT_EQUAL_FLOAT(-sqrt(3.0f), aMotion.speedProfile.dMax);
    }
}

void optimize() {
    overrides theOverrrides;
    //theStrategy = motionStrategy::maximizeSpeed;
    simplifiedMotion aSimpleMotion;
    motion aMotion;
    aSimpleMotion.setForTest(0U);
    aMotion.set(aSimpleMotion);
    aMotion.limit();
    aMotion.optimize(theOverrrides);
    TEST_ASSERT_EQUAL_FLOAT(2.0f, aMotion.speedProfile.duration);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, aMotion.speedProfile.mid.vMid);

    theMachineProperties.motors.jMax    = 1.0f;
    theMachineProperties.motors.vMax[0] = 2.0f;
    aSimpleMotion.setForTest(2U);
    aMotion.set(aSimpleMotion);
    aMotion.limit();
    aMotion.optimize(theOverrrides);
    TEST_ASSERT_EQUAL_FLOAT(8.0f, aMotion.speedProfile.duration);
    TEST_ASSERT_EQUAL_FLOAT(2.0f, aMotion.speedProfile.mid.vMid);
}

void calculateMotion() {
    overrides theOverrrides;
    simplifiedMotion aSimpleMotion;
    motion aMotion;
    theMachineProperties.motors.jMax    = 1.0f;
    theMachineProperties.motors.vMax[0] = 2.0f;
    aSimpleMotion.setForTest(2U);
    aMotion.set(aSimpleMotion);
    aMotion.limit();
    aMotion.optimize(theOverrrides);

    // check a(t),
    // check v(t),

    TEST_ASSERT_EQUAL_FLOAT(0.0f, aMotion.s(0.0f));        // checking s(t) on all boundaries
    TEST_ASSERT_EQUAL_FLOAT((0.125f / 6.0f), aMotion.s(0.5f));
    TEST_ASSERT_EQUAL_FLOAT((1.0f / 6.0f), aMotion.s(1.0f));
    TEST_ASSERT_EQUAL_FLOAT((1.0f / 6.0f) + 0.375f, aMotion.s(1.5f));
    TEST_ASSERT_EQUAL_FLOAT((1.0f / 6.0f) + 1.0f, aMotion.s(2.0f));
    TEST_ASSERT_EQUAL_FLOAT(2.0f + (0.125f / 6.0f), aMotion.s(2.5f));
    TEST_ASSERT_EQUAL_FLOAT(3.0f, aMotion.s(3.0f));

    TEST_ASSERT_EQUAL_FLOAT(7.0f, aMotion.s(5.0f));
    TEST_ASSERT_EQUAL_FLOAT(8.0f - (0.125f / 6.0f), aMotion.s(5.5f));
    TEST_ASSERT_EQUAL_FLOAT(9.0f - (1.0f / 6.0f), aMotion.s(6.0f));
    TEST_ASSERT_EQUAL_FLOAT(9.625f - (1.0f / 6.0f), aMotion.s(6.5f));
    TEST_ASSERT_EQUAL_FLOAT(10.0f - (1.0f / 6.0f), aMotion.s(7.0f));
    TEST_ASSERT_EQUAL_FLOAT(10.0f - (0.125f / 6.0f), aMotion.s(7.5f));
    TEST_ASSERT_EQUAL_FLOAT(10.0f, aMotion.s(8.0f));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initializeMotionPart);
    RUN_TEST(calculateMotionPartFirstOrder);
    RUN_TEST(calculateMotionPartSecondOrder);
    RUN_TEST(vTri);
    RUN_TEST(vOut);
    RUN_TEST(setSpeedProfile);
    RUN_TEST(limit);
    RUN_TEST(optimize);
    RUN_TEST(calculateMotion);
    UNITY_END();
}
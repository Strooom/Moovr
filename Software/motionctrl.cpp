// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "motionctrl.h"
#include <math.h>
#include "logging.h"

extern uLog theLog;

motionCtrl::motionCtrl(eventBuffer &theEventBuffer, machineProperties &theMachineProperties, overrides &theOverrides, stepBuffer &theStepBuffer) : theEventBuffer(theEventBuffer), theMachineProperties(theMachineProperties), theOverrides(theOverrides), theStepBuffer(theStepBuffer) {
}

void motionCtrl::append(gCodeParserResult &theParseResult) {
    if (!theMotionBuffer.isFull()) {
        uint32_t newItemIndex = theMotionBuffer.push();                                                                         // add an item to the buffer
        theMotionBuffer.motionBuffer[newItemIndex].set(theParseResult, theMachineProperties, theStrategy, theOverrides);        // populate the motion from the results of the gCode parsing
    } else {
        theEventBuffer.pushEvent(event::motionBufferOverflow);        // send a critical error event to the mainController, as this should never happen..
    }
}

void motionCtrl::optimize() {
    //switch (motionBufferLevel)
    //    {
    //    case 0:									// empty motionBuffer - nothing to do as there is nothing to optimize
    //        break;

    //    case 1:									// only 1 item, which is currently being executed -> derive a new motion for the remaining part, and optimize it's speed
    //        motionBuffer[motionBufferReadIndex].adjustRemaining();			// TODO : create new item from remaining part of motion...
    //        motionBuffer[motionBufferReadIndex].optimize(theStrategy, theOverrides, theMachineProperties);
    //        break;

    //    default:								// 2 or more items -> optimize them in pairs, each call of this function optimizes 1 pair.
    //        // minimizing is done by taking the pairs from left to right = oldest to newest.
    //        // maximizing is done by taking the pairs from right to left = newest to oldest
    //        // As only the junction speed of a pair is modified (not the entry/exit speed of the pair) it takes several iterations over the complete buffer to arrive a full optimization
    //        uint8_t itemLeft;					// index of the left motion (oldest)
    //        uint8_t itemRight;					// index of the right motion (newest)
    //        float vJunction;					// exit-entry speed between the motion-pair after optimizing
    //        float v;							// local variable to calculate and test for speed conditions

    //        switch (theStrategy)
    //            {
    //            case MotionStrategy::minimizeSpeed:
    //                // minimizing speed -> iterate from left to right over motions
    //                if (junctionIndex > motionBufferLevel - 2)
    //                    {
    //                    junctionIndex = 0;
    //                    }
    //                else
    //                    {
    //                    junctionIndex++;
    //                    }
    //                break;

    //            case MotionStrategy::maximizeSpeed:
    //                // maximize speed -> iterate from right to left over motions
    //                if (0 == junctionIndex)
    //                    {
    //                    junctionIndex = motionBufferLevel - 2;
    //                    }
    //                else
    //                    {
    //                    junctionIndex--;
    //                    }
    //                break;
    //            }

    //        itemLeft = (motionBufferReadIndex + junctionIndex) % motionBufferLength;
    //        itemRight = (motionBufferReadIndex + junctionIndex + 1) % motionBufferLength;

    //        if ((itemLeft == motionBufferReadIndex) || (itemRight == motionBufferReadIndex))
    //            {
    //            motionBuffer[motionBufferReadIndex].adjustRemaining();			// create new item from remaining part of motion...
    //            }

    //        switch (theStrategy)
    //            {
    //            case MotionStrategy::minimizeSpeed:
    //                vJunction = 0; // assume we can slow down to zero, adjust upwards if needed by other constraints

    //                v = motionBuffer[itemLeft].calcOtherV(MotionStrategy::minimizeSpeed, true);
    //                if (v > vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                v = motionBuffer[itemRight].calcOtherV(MotionStrategy::minimizeSpeed, false);
    //                if (v > vJunction)
    //                    {
    //                    vJunction = v;
    //                    }
    //                break;

    //            case MotionStrategy::maximizeSpeed:
    //                // calculate the maximal vEnd/vStart = vJunction, starting from a very large value, then restricting it down so it meets all kind of constraints
    //                vJunction = 999999999; // some large value to be restricted down... TODO : makes this cleaner eg using HUGE_VALF

    //                // restrict to maximal vEnd for given vStart of the lefthand motion
    //                v = motionBuffer[itemLeft].calcOtherV(MotionStrategy::maximizeSpeed, true);
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                // restrict to maximal vStart for given vEnd of the righthand motion
    //                v = motionBuffer[itemRight].calcOtherV(MotionStrategy::maximizeSpeed, false);
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                // restrict to respect feedrates and machine limits of both left and right
    //                v = motionBuffer[itemLeft].vFeed * theOverrides.feedOverride;
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                v = motionBuffer[itemLeft].vMax;
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                v = motionBuffer[itemRight].vFeed * theOverrides.feedOverride;
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                v = motionBuffer[itemRight].vMax;
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                // TODO : vJunction needs to be further reduced according to the kind of corners the trajectory is taking...
    //                break;
    //            }

    //        // now with this optimal junction speed, recalculate both motionItems
    //        motionBuffer[itemLeft].vEnd = vJunction;
    //        motionBuffer[itemRight].vStart = vJunction;
    //        motionBuffer[itemLeft].optimize(theStrategy, theOverrides, theMachineProperties);
    //        motionBuffer[itemRight].optimize(theStrategy, theOverrides, theMachineProperties);
    //    }
}

step motionCtrl::getNextStep() {
    while (true) {
        timeInMotionTicks++;
        ticksSinceLastOutput++;
        theStepSignals.shift();

        while (true) {
            if (theMotionBuffer.isEmpty()) {
                timeInMotionTicks = 0;
                return output(maxTicksSinceLastOutput);
            } else {
                motion *currentMotion   = theMotionBuffer.getCurrent();
                float timeInMotionFloat = (float)timeInMotionTicks * minStepPulseWidth;
                if (timeInMotionFloat <= currentMotion->theSpeedProfile.duration) {
                    float sNow = currentMotion->s(timeInMotionFloat);
                    for (uint8_t anAxis = 0; anAxis < (uint8_t)axis::nmbrAxis; ++anAxis) {
                        if (currentMotion->isMoving(anAxis)) {
                            calcNextPositionInMm(anAxis, sNow, currentMotion);
                            if (needStepForward(anAxis)) {
                                theStepSignals.stepForward(anAxis);
                                currentPositionInSteps[anAxis]++;
                            } else if (needStepBackward(anAxis)) {
                                theStepSignals.stepBackward(anAxis);
                                currentPositionInSteps[anAxis]--;
                            }
                        }
                    }
                    if (theStepSignals.isModified()) {
                        return output(ticksSinceLastOutput);
                    }
                    if (isTimedOut()) {
                        return output(maxTicksSinceLastOutput);
                    }
                    break;
                } else {
                    timeInMotionTicks = timeInMotionTicks - (uint32_t)((currentMotion->theSpeedProfile.duration) * outputTimerFrequency);
                    theMotionBuffer.pop();
                    theEventBuffer.pushEvent(event::motionCompleted);
                    theLog.output(loggingLevel::Debug, "motion Completed");
                    if (theMotionBuffer.isEmpty()) {
                        theEventBuffer.pushEvent(event::allMotionsCompleted);
                        theLog.output(loggingLevel::Debug, "all Motions Completed");
                    }
                }
            }
        }
    }
}

step motionCtrl::output(uint32_t timeBefore) {
    theStepSignals.lastOutput = theStepSignals.dirSetup;
    ticksSinceLastOutput      = 0;
    return step{timeBefore, theStepSignals.dirSetup};
}

void motionCtrl::run() {
        while (theStepBuffer.needsFilling()) {
        step aStep = getNextStep();        // get next step from Motion...
        theStepBuffer.write(aStep);        // ... and pump it to buffer
    }
 }

bool motionCtrl::needStepForward(uint8_t axis) {
    return ((int32_t)((nextPositionInMm[axis] * theMachineProperties.motors.stepsPerMm[axis]) - hysteresis) > currentPositionInSteps[axis]);
}

bool motionCtrl::needStepBackward(uint8_t axis) {
    return ((int32_t)((nextPositionInMm[axis] * theMachineProperties.motors.stepsPerMm[axis]) + hysteresis) < currentPositionInSteps[axis]);
}

void motionCtrl::calcNextPositionInMm(uint8_t axis, float sNow, motion *currentMotion) {
    // TODO : simplify by passing a pointer to theTrajectory io currentMotion

    // 4b. Determine if this new position requires a step, forward or backwards
    // NOTE - TODO there is some problem here as the conversion from float to int rounds towards zero (discards the fractional part) which means the interval [-1,1] takes twice the time..
    // Should test this on the K64/K66 to see how it behaves there
    if (axis == (uint8_t)currentMotion->theTrajectory.arcAxis0) {
        nextPositionInMm[axis] = (currentMotion->theTrajectory.arcCenter0 + (currentMotion->theTrajectory.radius * cosf(currentMotion->theTrajectory.startAngle + (currentMotion->theTrajectory.deltaRealTime[axis] * sNow))));
    } else if (axis == (uint8_t)currentMotion->theTrajectory.arcAxis1) {
        nextPositionInMm[axis] = (currentMotion->theTrajectory.arcCenter1 + (currentMotion->theTrajectory.radius * sinf(currentMotion->theTrajectory.startAngle + (currentMotion->theTrajectory.deltaRealTime[axis] * sNow))));
    } else {
        nextPositionInMm[axis] = (currentMotion->theTrajectory.startPosition[axis] + currentMotion->theTrajectory.deltaRealTime[axis] * sNow);
    }
}

bool motionCtrl::isTimedOut() {
    return (ticksSinceLastOutput >= maxTicksSinceLastOutput);
}
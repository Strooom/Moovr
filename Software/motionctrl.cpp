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
        uint32_t newItemIndex = theMotionBuffer.push();                                                                        // add an item to the buffer
        theMotionBuffer.motionBuffer[newItemIndex].set(theParseResult, theMachineProperties, strategy(), theOverrides);        // populate the motion from the results of the gCode parsing
    } else {
        theEventBuffer.pushEvent(event::motionBufferOverflow);        // send a critical error event to the mainController, as this should never happen..
    }
}

void motionCtrl::optimize() {
    switch (theMotionBuffer.level()) {
        case 0:
            isOptimal = true;
            break;
        case 1:
            theMotionBuffer.current()->optimizeCurrent(strategy(), theOverrides, (float)sampleIndex * minStepPulseWidth);        // ToDo : add a real time here, so we properly optimize the current motion
            break;
        default:
            switch (strategy()) {
                case motionStrategy::minimizeSpeed:
                    for (int32_t junctionIndex = 0; junctionIndex <= ((int32_t)theMotionBuffer.level() - 2); junctionIndex++) {
                        optimizePair(junctionIndex);
                    }
                    break;
                case motionStrategy::maximizeSpeed:
                    for (int32_t junctionIndex = (theMotionBuffer.level() - 2); junctionIndex >= 0; junctionIndex--) {
                        optimizePair(junctionIndex);
                    }
                    break;
            }
            break;
    }
}

void motionCtrl::optimizePair(int32_t junctionIndex) {
    uint32_t left  = (theMotionBuffer.readIndex + junctionIndex) % theMotionBuffer.bufferLength;            // index of the left motion (oldest)
    uint32_t right = (theMotionBuffer.readIndex + junctionIndex + 1) % theMotionBuffer.bufferLength;        // index of the right motion (newest)
    float v        = vJunction(left, right);                                                                // exit-entry speed between the motion-pair after optimizing

    // Todo : if the vStart/vEnd is already vJunction, then further optimizing is not possible. If all of the junctions are optimal, then the complete motion is optimal

    theMotionBuffer.motionBuffer[left].speedProfile.right.vEnd = v;
    if (0 == junctionIndex) {
        //            theMotionBuffer.motionBuffer[left].optimizeCurrent(strategy(), theOverrides, theMachineProperties, 0.0F);
        theMotionBuffer.motionBuffer[left].optimize(strategy(), theOverrides);
    } else {
        theMotionBuffer.motionBuffer[left].optimize(strategy(), theOverrides);
    }
    theMotionBuffer.motionBuffer[right].speedProfile.left.vStart = v;
    theMotionBuffer.motionBuffer[right].optimize(strategy(), theOverrides);
}

step motionCtrl::nextStep() {
    while (true) {
        if ((motionState::ready == theState) || (motionState::stopped == theState)) {
            return output(maxTicksSinceLastOutput);
        } else {
            sampleIndex++;
            ticksSinceLastOutput++;
            theStepSignals.shift();

            while (true) {
                if (theMotionBuffer.isEmpty()) {
                    sampleIndex = 0;
                    return output(maxTicksSinceLastOutput);
                } else {
                    motion *currentMotion   = theMotionBuffer.current();
                    float timeInMotionFloat = (float)sampleIndex * minStepPulseWidth;
                    if (timeInMotionFloat <= currentMotion->speedProfile.duration) {
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
                        sampleIndex = sampleIndex - (uint32_t)((currentMotion->speedProfile.duration) * outputTimerFrequency);
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
}

step motionCtrl::output(uint32_t timeBefore) {
    theStepSignals.lastOutput = theStepSignals.dirSetup;
    ticksSinceLastOutput      = 0;
    return step{timeBefore, theStepSignals.dirSetup};
}

float motionCtrl::sampleTime() const {
    return sampleZeroOffset + ((float)sampleIndex * minStepPulseWidth);
}

motionStrategy motionCtrl::strategy() const {
    switch (theState) {
        case motionState::ready:
        case motionState::running:
        default:
            return motionStrategy::maximizeSpeed;
            break;
        case motionState::stopping:
        case motionState::stopped:
            return motionStrategy::minimizeSpeed;
            break;
    }
}

float motionCtrl::vJunction(uint32_t left, uint32_t right) const {
    // for testing : TODO add smarter code to determine aligned segments from turns..
    return 0.0F;

    float vResult{0};        // local variable to calculate and test for speed conditions
    float vTest{0};          // local variable to calculate and test for speed conditions

    switch (strategy()) {
        case motionStrategy::minimizeSpeed:
            vResult = 0;        // assume we can slow down to zero, adjust upwards if needed by other constraints
            vTest   = theMotionBuffer.motionBuffer[left].vOut(motionStrategy::minimizeSpeed, motionCalculateDirection::forward);
            if (vTest > vResult) {
                vResult = vTest;
            }
            vTest = theMotionBuffer.motionBuffer[right].vOut(motionStrategy::minimizeSpeed, motionCalculateDirection::reverse);
            if (vTest > vResult) {
                vResult = vTest;
            }
            break;

        case motionStrategy::maximizeSpeed:
            // calculate the maximal vEnd/vStart = vJunction, starting from a very large value, then restricting it down so it meets all kind of constraints
            vResult = std::numeric_limits<float>::infinity();        // some large value to be restricted down...
            // restrict to maximal vEnd for given vStart of the lefthand motion
            vTest = theMotionBuffer.motionBuffer[left].vOut(motionStrategy::maximizeSpeed, motionCalculateDirection::forward);
            if (vTest < vResult) {
                vResult = vTest;
            }
            // restrict to maximal vStart for given vEnd of the righthand motion
            vTest = theMotionBuffer.motionBuffer[right].vOut(motionStrategy::maximizeSpeed, motionCalculateDirection::reverse);
            if (vTest < vResult) {
                vResult = vTest;
            }
            // restrict to respect feedrates and machine limits of both left and right
            vTest = theMotionBuffer.motionBuffer[left].speedProfile.vFeed * theOverrides.feedOverride;
            if (vTest < vResult) {
                vResult = vTest;
            }
            vTest = theMotionBuffer.motionBuffer[left].speedProfile.vMax;
            if (vTest < vResult) {
                vResult = vTest;
            }
            vTest = theMotionBuffer.motionBuffer[right].speedProfile.vFeed * theOverrides.feedOverride;
            if (vTest < vResult) {
                vResult = vTest;
            }
            vTest = theMotionBuffer.motionBuffer[right].speedProfile.vMax;
            if (vTest < vResult) {
                vResult = vTest;
            }
            // TODO !
            // TODO : vJunction needs to be further reduced according to the kind of corners the trajectory is taking...
            // TODO !
            break;
    }
    return vResult;
}

void motionCtrl::run() {
    while (theStepBuffer.needsFilling()) {
        step aStep = nextStep();           // get next step from Motion...
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
    // TODO : simplify by passing a pointer to trajectory io currentMotion

    // 4b. Determine if this new position requires a step, forward or backwards
    // NOTE - TODO there is some problem here as the conversion from float to int rounds towards zero (discards the fractional part) which means the interval [-1,1] takes twice the time..
    // Should test this on the K64/K66 to see how it behaves there
    if (axis == (uint8_t)currentMotion->trajectory.arcAxis0) {
        nextPositionInMm[axis] = (currentMotion->trajectory.arcCenter0 + (currentMotion->trajectory.radius * cosf(currentMotion->trajectory.startAngle + (currentMotion->trajectory.deltaRealTime[axis] * sNow))));
    } else if (axis == (uint8_t)currentMotion->trajectory.arcAxis1) {
        nextPositionInMm[axis] = (currentMotion->trajectory.arcCenter1 + (currentMotion->trajectory.radius * sinf(currentMotion->trajectory.startAngle + (currentMotion->trajectory.deltaRealTime[axis] * sNow))));
    } else {
        nextPositionInMm[axis] = (currentMotion->trajectory.startPosition[axis] + currentMotion->trajectory.deltaRealTime[axis] * sNow);
    }
}

bool motionCtrl::isTimedOut() {
    return (ticksSinceLastOutput >= maxTicksSinceLastOutput);
}
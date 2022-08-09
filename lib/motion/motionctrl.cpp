// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "motionctrl.h"
#include "machineproperties.h"
#include "overrides.h"
#include "eventbuffer.h"
#include "stepbuffer.h"

#include <math.h>
#include <limits>

extern machineProperties theMachineProperties;
extern eventBuffer theEventBuffer;
extern stepBuffer theStepBuffer;

void motionCtrl::append(simplifiedMotion &theMotion) {
    if (!theMotionBuffer.isFull()) {
        uint32_t newItemIndex = theMotionBuffer.push();
        theMotionBuffer.motionBuffer[newItemIndex].set(theMotion);
    } else {
        theEventBuffer.pushEvent(event::motionBufferOverflow);
    }
}

void motionCtrl::start() {
    if (!theMotionBuffer.isEmpty()) {
        theMotionCtrlState = motionState::running;
        optimize();
    } else {
    }
}

void motionCtrl::resume() {
    // TODO : is this different from start() ?
}

void motionCtrl::stop() {
    theMotionCtrlState = motionState::stopping;
    optimize();
}

void motionCtrl::hold() {
    // TOFO : is this different from stop() ?
}

void motionCtrl::optimize() {
    switch (theMotionBuffer.getLevel()) {
        case 0:
            isOptimal = true;
            break;
        case 1:
            theMotionBuffer.getHeadPtr()->optimize(theOverrides, theSampleTime.timeInMotion);
            break;
        default:
            switch (theStrategy) {
                case motionStrategy::minimizeSpeed:
                    for (int32_t junctionIndex = 0; junctionIndex <= ((int32_t)theMotionBuffer.getLevel() - 2); junctionIndex++) {
                        optimizePair(junctionIndex);
                    }
                    break;
                case motionStrategy::maximizeSpeed:
                    for (int32_t junctionIndex = (theMotionBuffer.getLevel() - 2); junctionIndex >= 0; junctionIndex--) {
                        optimizePair(junctionIndex);
                    }
                    break;
            }
            break;
    }
}

void motionCtrl::optimizePair(int32_t junctionIndex) {
    uint32_t left  = (theMotionBuffer.head + junctionIndex) % theMotionBuffer.length;            // index of the left motion (oldest)
    uint32_t right = (theMotionBuffer.head + junctionIndex + 1) % theMotionBuffer.length;        // index of the right motion (newest)
    float v        = vJunction(left, right);                                                     // exit-entry speed between the motion-pair after optimizing

    // TODO : if the vStart/vEnd is already vJunction, then further optimizing is not possible. If all of the junctions are optimal, then the complete motion is optimal

    theMotionBuffer.motionBuffer[left].speedProfile.right.vEnd = v;
    if (0 == junctionIndex) {
        theMotionBuffer.motionBuffer[left].optimize(theOverrides, theSampleTime.timeInMotion);
        theMotionBuffer.motionBuffer[left].optimize(theOverrides);
    } else {
        theMotionBuffer.motionBuffer[left].optimize(theOverrides);
    }
    theMotionBuffer.motionBuffer[right].speedProfile.left.vStart = v;
    theMotionBuffer.motionBuffer[right].optimize(theOverrides);
}

// #if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
// FASTRUN
// #endif

step motionCtrl::calculateStepperSignals() {
    while (true) {
        theStepSignals.next();
        if (isRunning()) {
            theSampleTime.next();
            while (true) {
                if (theSampleTime.isBeyondStop()) {
                    theMotionCtrlState = motionState::stopped;
                    theEventBuffer.pushEvent(event::motionStopped);
                }
                if (theSampleTime.isBeyondEndOfMotion()) {
                    theSampleTime.initializeNextMotion();
                    theMotionBuffer.pop();
                    theEventBuffer.pushEvent(event::motionCompleted);
                    if (theMotionBuffer.isEmpty()) {
                        theMotionCtrlState = motionState::stopped;
                        theEventBuffer.pushEvent(event::allMotionsCompleted);
                        theEventBuffer.pushEvent(event::motionStopped);
                        theSampleTime.initialize();
                        break;
                    }
                } else {
                    calcStepSignals();
                    break;
                }
            }
        }
        if (theStepSignals.isModified()) {
            return theStepSignals.output();
        }
        if (theStepSignals.isTimedOut()) {
            return theStepSignals.outputDefault();
        }
    }
}

void motionCtrl::calcStepSignals() {
    // TODO : could maybe optimize by storing local copy of theMotionBuffer.current()
    float sNow = theMotionBuffer.getHeadPtr()->s(theSampleTime.timeInMotion);
    for (uint8_t anAxis = 0; anAxis < (uint8_t)axis::nmbrAxis; ++anAxis) {
        if (theMotionBuffer.getHeadPtr()->isMoving(anAxis)) {
            calcNextPositionInMm(anAxis, sNow, theMotionBuffer.getHeadPtr());
            if (needStepForward(anAxis)) {
                theStepSignals.stepForward(anAxis);
                currentPositionInSteps[anAxis]++;
            } else if (needStepBackward(anAxis)) {
                theStepSignals.stepBackward(anAxis);
                currentPositionInSteps[anAxis]--;
            }
        }
    }
}

motionState motionCtrl::getState() const {
    return theMotionCtrlState;
}
motionStrategy motionCtrl::getMotionStrategy() const {
    return theStrategy;
}

bool motionCtrl::isRunning() const {
    return ((motionState::running == theMotionCtrlState) || (motionState::stopping == theMotionCtrlState));
    // return (!(motionState::stopped == state)); // alternate way to calculate it...
}

float motionCtrl::vJunction(uint32_t left, uint32_t right) const {
    // TODO add smarter code to determine aligned segments from turns..
    return 0.0F;

    float vResult{0};        // local variable to calculate and test for speed conditions
    float vTest{0};          // local variable to calculate and test for speed conditions

    switch (theStrategy) {
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

// This is a cooperation between motionCtrl and stepBuffer which could be at the main application level. So maybe not put it in either of those classes but just in main()
// void motionCtrl::run() {
//     while (theStepBuffer.needsFilling()) {
//         step aStep = calculateStepperSignals();           // get next step from Motion...
//         theStepBuffer.write(aStep);        // ... and pump it to buffer
//     }
// }

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

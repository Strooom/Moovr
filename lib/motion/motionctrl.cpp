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

motionState motionCtrl::getState() const {
    return theMotionCtrlState;
}

bool motionCtrl::isRunning() const {
    return (!(motionState::stopped == theMotionCtrlState));
}

motionStrategy motionCtrl::theStrategy() const {
    return ((motionState::running == theMotionCtrlState) ? motionStrategy::maximizeSpeed : motionStrategy::minimizeSpeed);
}

void motionCtrl::append(simplifiedMotion& theMotion) {
    if (!theMotionBuffer.isFull()) {
        uint32_t newItemIndex = theMotionBuffer.push();
        theMotionBuffer.motionBuffer[newItemIndex].set(theMotion, theOverrides, theStrategy());
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
            theMotionBuffer.getHead().optimize(theOverrides, (theStrategy()), theSampleTime.timeInMotion);
            break;
        default:
            switch (theStrategy()) {
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
        theMotionBuffer.motionBuffer[left].optimize(theOverrides, theStrategy(), theSampleTime.timeInMotion);
        theMotionBuffer.motionBuffer[left].optimize(theOverrides, theStrategy());
    } else {
        theMotionBuffer.motionBuffer[left].optimize(theOverrides, theStrategy());
    }
    theMotionBuffer.motionBuffer[right].speedProfile.left.vStart = v;
    theMotionBuffer.motionBuffer[right].optimize(theOverrides, theStrategy());
}

float motionCtrl::vJunction(uint32_t left, uint32_t right) const {
    // TODO add smarter code to determine aligned segments from turns..
    return 0.0F;

    float vResult{0};        // local variable to calculate and test for speed conditions
    float vTest{0};          // local variable to calculate and test for speed conditions

    switch (theStrategy()) {
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

step motionCtrl::calcNextStepperMotorSignals() {
    while (true) {
        theStepSignals.next();
        if (isRunning()) {
            theSampleTime.next();
            if (theSampleTime.isBeyond(theMotionBuffer.getHead().speedProfile.tStop)) {
                theMotionCtrlState = motionState::stopped;
                theEventBuffer.pushEvent(event::motionStopped);
            }
            while (true) {
                if (theSampleTime.isBeyond(theMotionBuffer.getHead().speedProfile.duration)) {
                    if (theMotionBuffer.getLevel() > 1) {
                        theSampleTime.sampleZeroOffset = theSampleTime.timeInMotion - theMotionBuffer.getHead().speedProfile.duration;
                        theMotionBuffer.pop();
                        theSampleTime.initializeNextMotion();
                        theEventBuffer.pushEvent(event::motionCompleted);
                    } else {
                        theMotionBuffer.pop();
                        theSampleTime.initialize();
                        theMotionCtrlState = motionState::stopped;
                        theEventBuffer.pushEvent(event::motionStopped);
                        theEventBuffer.pushEvent(event::motionCompleted);
                        theEventBuffer.pushEvent(event::allMotionsCompleted);
                        break;
                    }
                } else {
                    move();
                    break;
                }
            }
        }
        if (theStepSignals.isModified() || theStepSignals.isTimedOut()) {
            return theStepSignals.output();
        }
    }
}

void motionCtrl::move() {
    motion& currentMotion   = theMotionBuffer.getHead();                          // get a reference to the current motionItem, to speed up reading its members
    float distanceTravelled = currentMotion.s(theSampleTime.timeInMotion);        // at current time, what distance have we travelled over the trajectory

    for (uint32_t anAxis = 0; anAxis < nmbrAxis; ++anAxis) {
        if (currentMotion.isMoving(anAxis)) {
            float positionInMm = currentMotion.positionInMmFromDistanceTravelled(anAxis, distanceTravelled);
            if (needStepForward(anAxis, positionInMm)) {
                stepForward(anAxis);
            } else if (needStepBackward(anAxis, positionInMm)) {
                stepBackward(anAxis);
            }
        }
    }
}

bool motionCtrl::needStepForward(uint32_t anAxis, float positionInMm) {
    float positionInStepsFloat = positionInMm * theMachineProperties.motors.stepsPerMm[anAxis];
    int32_t newPosition        = roundf(positionInStepsFloat - hysteresis);
    return (newPosition > currentPositionInSteps[anAxis]);
}

bool motionCtrl::needStepBackward(uint32_t anAxis, float positionInMm) {
    float positionInStepsFloat = positionInMm * theMachineProperties.motors.stepsPerMm[anAxis];
    return ((roundf(positionInStepsFloat + hysteresis)) < currentPositionInSteps[anAxis]);
}

void motionCtrl::stepForward(uint32_t anAxis) {
    theStepSignals.stepForward(anAxis);
    ++currentPositionInSteps[anAxis];
}

void motionCtrl::stepBackward(uint32_t anAxis) {
    theStepSignals.stepBackward(anAxis);
    --currentPositionInSteps[anAxis];
}

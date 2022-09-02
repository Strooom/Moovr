#include "machineproperties.h"
#include "event.h"
#include "eventbuffer.h"
#include "homing.h"
#include "motionctrl.h"
#include "motionstate.h"
#include "debouncedinput.h"

extern motionCtrl theMotionController;
extern machineProperties theMachineProperties;
extern debouncedInput myInputs[nmbrInputs];
extern eventBuffer theEventBuffer;

void homingController::start() {
    goTo(homingState::lost);
    axisIndex = 0;        // TODO : maybe I could move this to enterstate lost
    if (selectAxis()) {
        theEventBuffer.pushEvent(event::homingStarted);
        if (theMotionController.isRunning()) {
            goTo(homingState::stopping);
        } else {
            if (myInputs[theLimitSwitch].getState()) {
                goTo(homingState::opening);
            } else {
                goTo(homingState::closing);
            }
        }
    } else {
        theEventBuffer.pushEvent(event::homingError);
    }
}

bool homingController::nextAxis() {
    axisIndex++;
    return selectAxis();
}

bool homingController::selectAxis() {
    while (axisIndex < nmbrAxis) {
        currentAxis = theMachineProperties.homingSequence[axisIndex];
        if (static_cast<uint32_t>(currentAxis) < nmbrAxis) {
            if (theMachineProperties.homingDirection[static_cast<uint32_t>(currentAxis)]) {
                if (theMachineProperties.limits.hasLimitsMax[static_cast<uint32_t>(currentAxis)]) {
                    theLimitSwitch = theMachineProperties.limits.limitMaxIndex[static_cast<uint32_t>(currentAxis)];
                    switch (currentAxis) {
                        default:
                        case axis::X:
                            limitSwitchClose = event::limitSwitchXMaxClosed;
                            limitSwitchOpen  = event::limitSwitchXMaxOpened;
                            break;

                        case axis::Y:
                            limitSwitchClose = event::limitSwitchYMaxClosed;
                            limitSwitchOpen  = event::limitSwitchYMaxOpened;
                            break;

                        case axis::Z:
                            limitSwitchClose = event::limitSwitchZMaxClosed;
                            limitSwitchOpen  = event::limitSwitchZMaxOpened;
                            break;
                    }
                    return true;
                }
            } else {
                if (theMachineProperties.limits.hasLimitsMin[static_cast<uint32_t>(currentAxis)]) {
                    theLimitSwitch = theMachineProperties.limits.limitMinIndex[static_cast<uint32_t>(currentAxis)];
                    switch (currentAxis) {
                        default:
                        case axis::X:
                            limitSwitchClose = event::limitSwitchXMinClosed;
                            limitSwitchOpen  = event::limitSwitchXMinOpened;
                            break;

                        case axis::Y:
                            limitSwitchClose = event::limitSwitchYMinClosed;
                            limitSwitchOpen  = event::limitSwitchYMinOpened;
                            break;

                        case axis::Z:
                            limitSwitchClose = event::limitSwitchZMinClosed;
                            limitSwitchOpen  = event::limitSwitchZMinOpened;
                            break;
                    }
                    return true;
                }
            }
        }
        axisIndex++;
    }
    return false;
}

void homingController::handleEvents(event theEvent) {
    switch (theHomingState) {
        default:
        case homingState::lost:
            break;

        case homingState::stopping:
            switch (theEvent) {
                case event::motionStopped:
                    if (myInputs[theLimitSwitch].getState()) {
                        goTo(homingState::opening);
                    } else {
                        goTo(homingState::closing);
                    }
                    break;

                default:
                    break;
            }
            break;
        case homingState::closing:
            if (limitSwitchClose == theEvent) {
                goTo(homingState::closedWaitForStop);
            }
            break;

        case homingState::closedWaitForStop:
            switch (theEvent) {
                case event::motionStopped:
                    goTo(homingState::opening);
                    break;

                default:
                    break;
            }
            break;

        case homingState::opening:
            if (limitSwitchOpen == theEvent) {
                goTo(homingState::openedWaitForStop);
            }
            break;

        case homingState::openedWaitForStop:
            switch (theEvent) {
                case event::motionStopped:
                    goTo(homingState::offsettingWaitForStop);
                    break;

                default:
                    break;
            }
            break;

        case homingState::offsettingWaitForStop:
            switch (theEvent) {
                case event::motionStopped:
                    if (nextAxis()) {
                        if (myInputs[theLimitSwitch].getState()) {
                            goTo(homingState::opening);
                        } else {
                            goTo(homingState::closing);
                        }
                    } else {
                        goTo(homingState::found);
                    }
                    break;

                default:
                    break;
            }
            break;

        case homingState::found:
            break;
    }
}

void homingController::handleTimeouts() {
}

void homingController::goTo(homingState theNewState) {
    exitState(theHomingState);
    theHomingState = theNewState;
    enterState(theNewState);
}

void homingController::enterState(homingState theNewState) {
    switch (theHomingState) {
        case homingState::lost:
            break;

        case homingState::stopping:
            theMotionController.stop();
            break;

        case homingState::closing:
            theMotionController.flushMotionBuffer();
            theMotionController.resetMachinePosition();
            {
                simplifiedMotion aMotion;
                aMotion.setForHoming(currentAxis, theMachineProperties.motors.sMax[static_cast<uint32_t>(currentAxis)], theMachineProperties.vHoming);
                theMotionController.append(aMotion);
            }
            theMotionController.start();
            break;

        case homingState::closedWaitForStop:
            theMotionController.stop();
            break;

        case homingState::opening:
            theMotionController.flushMotionBuffer();
            theMotionController.resetMachinePosition();
            {
                simplifiedMotion aMotion;
                aMotion.setForHoming(currentAxis, -10.0, theMachineProperties.vHomingSlow);
                theMotionController.append(aMotion);
            }
            theMotionController.start();
            break;

        case homingState::openedWaitForStop:
            theMotionController.stop();
            break;

        case homingState::offsettingWaitForStop:
            theMotionController.flushMotionBuffer();
            theMotionController.resetMachinePosition();
            {
                simplifiedMotion aMotion;
                aMotion.setForHoming(currentAxis, theMachineProperties.homingOffset[static_cast<uint32_t>(currentAxis)], theMachineProperties.vHoming);
                theMotionController.append(aMotion);
            }
            theMotionController.start();
            break;

        case homingState::found:
            // TODO put all actions here when homing is complete
            break;

        default:
            break;
    }
}

void homingController::exitState(homingState theOldState) {
    timeOut.stop();        // TODO : this could be different for each state, but currently this is good enough
}

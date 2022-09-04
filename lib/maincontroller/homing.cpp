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
    homingSequenceIndex = 0;
    if (selectAxis()) {
        theEventBuffer.pushEvent(event::homingStarted);
        if (theMotionController.isRunning()) {
            goTo(homingState::stopping);
        } else {
            if (myInputs[theLimitSwitch].isClosed()) {
                //Serial.printf("limitSwitch %s is closed\n", toString(currentHomingAxis));
                goTo(homingState::opening);
            } else {
                //Serial.printf("limitSwitch %s is open\n", toString(currentHomingAxis));
                goTo(homingState::closing);
            }
        }
    } else {
        theEventBuffer.pushEvent(event::homingError);
    }
}

bool homingController::nextAxis() {
    homingSequenceIndex++;
    return selectAxis();
}

bool homingController::selectAxis() {
    while (homingSequenceIndex < nmbrAxis) {
        currentHomingAxis      = theMachineProperties.homingSequence[homingSequenceIndex];
        currentHomingAxisIndex = static_cast<uint32_t>(currentHomingAxis);
        if (static_cast<uint32_t>(currentHomingAxis) < nmbrAxis) {
            if (theMachineProperties.homingDirection[currentHomingAxisIndex]) {
                if (theMachineProperties.limits.hasLimitsMax[currentHomingAxisIndex]) {
                    theLimitSwitch = theMachineProperties.limits.limitMaxIndex[currentHomingAxisIndex];
                    switch (currentHomingAxis) {
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
                if (theMachineProperties.limits.hasLimitsMin[currentHomingAxisIndex]) {
                    theLimitSwitch = theMachineProperties.limits.limitMinIndex[currentHomingAxisIndex];
                    switch (currentHomingAxis) {
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
        homingSequenceIndex++;
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
                    if (myInputs[theLimitSwitch].isClosed()) {
                        //Serial.printf("limitSwitch %s is closed\n", toString(currentHomingAxis));
                        goTo(homingState::opening);
                    } else {
                        //Serial.printf("limitSwitch %s is open\n", toString(currentHomingAxis));
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
                        if (myInputs[theLimitSwitch].isClosed()) {
                            //Serial.printf("limitSwitch %s is closed\n", toString(currentHomingAxis));
                            goTo(homingState::opening);
                        } else {
                            //Serial.printf("limitSwitch %s is open\n", toString(currentHomingAxis));
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
    //Serial.printf("homingState from %s to %s\n", toString(theHomingState), toString(theNewState));
    theHomingState = theNewState;
    enterState(theNewState);
}

void homingController::enterState(homingState theNewState) {
    //Serial.printf("[%d, %d, %d]\n", theMotionController.machinePositionInSteps[0], theMotionController.machinePositionInSteps[1], theMotionController.machinePositionInSteps[2]);
    switch (theHomingState) {
        case homingState::lost:
            break;

        case homingState::stopping:
            theMotionController.stop();
            break;

        case homingState::closing:
            theMotionController.flushMotionBuffer();
            {
                point currentPosition;
                simplifiedMotion aMotion;
                theMotionController.getMachinePosition(currentPosition);
                aMotion.set(currentPosition, currentHomingAxis, theMachineProperties.motors.sMax[currentHomingAxisIndex], theMachineProperties.vHoming);
                theMotionController.append(aMotion);
            }
            theMotionController.start();
            //Serial.printf("going to [%f]\n", theMachineProperties.motors.sMax[currentHomingAxisIndex] * theMachineProperties.motors.stepsPerMm[currentHomingAxisIndex]);
            break;

        case homingState::closedWaitForStop:
            theMotionController.stop();
            break;

        case homingState::opening:
            theMotionController.flushMotionBuffer();
            {
                point currentPosition;
                simplifiedMotion aMotion;
                theMotionController.getMachinePosition(currentPosition);
                aMotion.set(currentPosition, currentHomingAxis, -1 * theMachineProperties.motors.sMax[currentHomingAxisIndex], theMachineProperties.vHoming);
                theMotionController.append(aMotion);
            }
            theMotionController.start();
            //Serial.printf("going to [%f]\n", -1 * theMachineProperties.motors.sMax[currentHomingAxisIndex] * theMachineProperties.motors.stepsPerMm[currentHomingAxisIndex]);
            break;

        case homingState::openedWaitForStop:
            theMotionController.stop();
            break;

        case homingState::offsettingWaitForStop:
            theMotionController.flushMotionBuffer();
            {
                point currentPosition;
                simplifiedMotion aMotion;
                theMotionController.getMachinePosition(currentPosition);
                aMotion.set(currentPosition, currentHomingAxis, theMachineProperties.homingOffset[currentHomingAxisIndex], theMachineProperties.vHoming);
                theMotionController.append(aMotion);
            }
            theMotionController.start();
            //Serial.printf("going to [%f]\n", theMachineProperties.homingOffset[currentHomingAxisIndex] * theMachineProperties.motors.stepsPerMm[currentHomingAxisIndex]);
            break;

        case homingState::found:
            theMotionController.resetMachinePosition();
            // TODO put all actions here when homing is complete
            break;

        default:
            break;
    }
}

void homingController::exitState(homingState theOldState) {
    switch (theOldState) {
        case homingState::offsettingWaitForStop:
            break;

        default:
            break;
    }

    //    timeOut.stop();        // TODO : this could be different for each state, but currently this is good enough
}

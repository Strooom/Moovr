#include "machineproperties.h"
#include "event.h"
#include "homing.h"
#include "motionctrl.h"
#include "motionstate.h"
#include "debouncedinput.h"

extern motionCtrl theMotionController;
extern machineProperties theMachineProperties;
extern debouncedInput myInputs[nmbrInputs];

void homingController::start() {
    axisIndex = 0;
    selectAxis();
    if (theMotionController.isRunning()) {
        goTo(homingState::stopping);
    } else {
        if (myInputs[theLimitSwitch].getState()) {
            goTo(homingState::opening);
        } else {
            goTo(homingState::closing);
        }
    }
}

void homingController::selectAxis() {
    axis theAxis = theMachineProperties.homingSequence[axisIndex];
    if (theMachineProperties.homingDirection[static_cast<uint32_t>(theAxis)]) {
        if (theMachineProperties.limits.hasLimitsMax[static_cast<uint32_t>(theAxis)]) {
            theLimitSwitch = theMachineProperties.limits.limitMaxIndex[static_cast<uint32_t>(theAxis)];
        } else {
            // error : cannot home if machine does not have this switch -> skip
        }
    } else {
        if (theMachineProperties.limits.hasLimitsMin[static_cast<uint32_t>(theAxis)]) {
            theLimitSwitch = theMachineProperties.limits.limitMinIndex[static_cast<uint32_t>(theAxis)];
        } else {
            // error : cannot home if machine does not have this switch -> skip
        }
    }

// TODO : also select
// which limitswitch events to listen to
// which gcode to send for
// * closing : move in + or - direction, for maximum distance of sMax @ homingSpeed
// * opening : move in - or + direction, for maximum distance of 2 * limitswitchtravel @ (homingSpeed / 4)
// * offsetting : 3 mm (could set an offset in machineProperties)

}

void homingController::handleEvents(event theEvent) {
    switch (theHomingState) {
        default:
        case homingState::lost:
            break;

        case homingState::stopping:
            switch (theEvent) {
                default:
                    break;
                case event::motionStopped:
                    break;
            }
            break;
        case homingState::closing:
            // if closing event...
            break;
        case homingState::closedWaitForStop:
            switch (theEvent) {
                default:
                    break;
                case event::motionStopped:
                    break;
            }
            break;
        case homingState::opening:
            // if opening event
            break;
        case homingState::openedWaitForStop:
            switch (theEvent) {
                default:
                    break;
                case event::motionStopped:
                    break;
            }
            break;
        case homingState::offsettingWaitForStop:
            switch (theEvent) {
                default:
                    break;
                case event::motionStopped:
                    break;
            }
            break;
        case homingState::found:
            // move to next axis or complete
            break;
    }
}

void homingController::handleTimeouts() {
}

// homingState homingController::getState() {
//     return theHomingState;
// }

void homingController::goTo(homingState theNewState) {
    exitState(theHomingState);
    theHomingState = theNewState;
    enterState(theNewState);
}

void homingController::enterState(homingState theNewState) {
    switch (theHomingState) {
        case homingState::stopping:
            theMotionController.stop();
            break;

        case homingState::closing:
            theMotionController.flush();
            //theMotionController.append(gCodeCommand);
            //theMotionController.start();
            break;

        case homingState::closedWaitForStop:
            theMotionController.stop();
            break;

        case homingState::opening:
            theMotionController.flush();
            //theMotionController.append(gCodeCommand);
            //theMotionController.start();
            break;

        case homingState::openedWaitForStop:
            theMotionController.stop();
            break;

        case homingState::offsettingWaitForStop:
            theMotionController.flush();
            //.append(gCodeCommand);
            //theMotionController.start();
            break;

        case homingState::found:
            break;

        default:
            break;
    }
}

void homingController::exitState(homingState theOldState) {
}

// void homingController::stop() {
//  stop motion
//  when stopped, clear motionbuffer
//}

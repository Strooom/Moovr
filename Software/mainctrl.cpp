// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "mainctrl.h"
#include "eventbuffer.h"
#include "hostinterface.h"
#include "logging.h"

extern uLog theLog;

mainController::mainController(machineProperties &someMachineProperties, eventBuffer &anEventBuffer, HostInterfaceUart &aHostInterface, stepBuffer &aStepBuffer) : theMachineProperties(someMachineProperties), theEventBuffer(anEventBuffer), theHostInterface(aHostInterface), theStepBuffer(aStepBuffer) {
}

void mainController::run() {
    theMotionCtrl.run();
    //theMotionCtrl.optimize();
    handleEvents();        // handle all events such as buttons pressed, limit switches triggering, motions being completed..
    //handleMessages();        // handle all input from the external hostInterface
}

void mainController::handleMessages() {
    if (theHostInterface.hasMessage()) {
        theHostInterface.getMessage(commandLine);

        command aCommand;

        if (ESC == commandLine[0]) {                   // all commands are preceded by a special char, which can never appear in gCode
            aCommand = (command)commandLine[1];        // non gCode commands
        } else {                                       //
            aCommand = command::gCode;                 // gCode
        }



        //switch (mainState) {
        //    case mainStates::ready:
        //        switch (theEvent) {
        //            case event::feedHoldResumeButtonPressed:
        //                if (!theMotionCtrl.theMotionBuffer.isEmpty()) {
        //                    theMotionCtrl.theState = motionState::running;
        //                } else {
        //                    theLog.output(loggingLevel::Error, "go without data");
        //                }
        //                break;
        //            default:
        //                break;
        //        }
        //    case mainStates::homing:
        //        homing(theEvent);
        //        break;
        //    case mainStates::probing:
        //        probing(theEvent);
        //        break;
        //    case mainStates::running:
        //        switch (theEvent) {
        //            case event::allMotionsCompleted:
        //                gotoState(mainStates::ready);
        //                break;
        //            case event::feedHoldResumeButtonPressed:
        //                theMotionCtrl.theState = motionState::stopping;
        //                theMotionCtrl.optimize();
        //                theMotionCtrl.optimize();
        //                theMotionCtrl.optimize();
        //                break;
        //            default:
        //                break;
        //        }
        //        break;
        //    case mainStates::error:
        //        break;
        //    default:
        //        break;
        //}

        switch (mainState) {
            case mainStates::ready:
                switch (aCommand) {
                    case command::gCode:
                        theParser.getBlock(commandLine);        // Parse the line of gCode text into a gCodeBlock
                        while (theParser.getNmbrWords() > 0) {
                            theParser.parseBlock(theResult);        // Parse the gCode Block into a gCode state update and/or a gCodeMotion
                            switch (theResult.theParseResultType) {
                                case gCodeParserResult::ParseResultType::OkContextUpdateOnly:
                                    break;
                                case gCodeParserResult::ParseResultType::OkContextUpdateAndMotion:
                                    theMotionCtrl.append(theResult);
                                    //mainStates = mainStates::RUNNING;
                                    break;
                                case gCodeParserResult::ParseResultType::Error:
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;

                    case command::cancel:
                        break;
                    case command::doHome:
                        gotoState(mainStates::homing);
                        // TODO : start the homing sequence
                        break;
                    case command::doProbe:
                        gotoState(mainStates::probing);
                        // TODO : start the probing sequence
                        break;
                    case command::setOverride:
                        // TODO : set the overrides
                        break;
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}

void mainController::homing(event theEvent) {
    switch (theEvent) {
        case event::emergencyStopButtonPressed:
            break;
        case event::emergencyStopButtonReleased:
            break;
        case event::feedHoldResumeButtonPressed:
            break;
        case event::feedHoldResumeButtonReleased:
            break;
        case event::probeSwitchClosed:
            break;
        case event::probeSwitchOpened:
            break;
        case event::limitSwitchXMinClosed:
            break;
        case event::limitSwitchYMinClosed:
            break;
        case event::limitSwitchZMinClosed:
            break;
        case event::limitSwitchXMaxClosed:
            break;
        case event::limitSwitchYMaxClosed:
            break;
        case event::limitSwitchZMaxClosed:
            break;
        case event::limitSwitchXMinOpened:
            break;
        case event::limitSwitchYMinOpened:
            break;
        case event::limitSwitchZMinOpened:
            break;
        case event::limitSwitchXMaxOpened:
            break;
        case event::limitSwitchYMaxOpened:
            break;
        case event::limitSwitchZMaxOpened:
            break;
        case event::motionAdded:
            break;
        case event::motionCompleted:
            break;
        case event::allMotionsCompleted:
            break;
        default:
            break;
    }
}

void mainController::probing(event theEvent) {
}

void mainController::discovery(event theEvent) {
}

void mainController::initialize() {
    theParser.initialize();
    theParser.getBlock("G1 X800 F2400");
    theParser.parseBlock(theResult);
    theMotionCtrl.append(theResult);
}

void mainController::gotoState(mainStates newState) {
    theLog.snprintf(loggingLevel::Debug, "mainState from %s -> %s", toString(mainState), toString(newState));
    mainState = newState;
}

void mainController::gotoHomingState(homingStates newState) {
    //theLog.snprintf(loggingLevel::Debug, "homingState from %s -> %s", toString(homingState), toString(newState));
    homingState = newState;
}

void mainController::gotoProbingState(probingStates newState) {
    //theLog.snprintf(loggingLevel::Debug, "probingState from %s -> %s", toString(probingState), toString(newState));
    probingState = newState;
}

void mainController::handleEvents() {
    while (theEventBuffer.hasEvents()) {
        event theEvent = theEventBuffer.popEvent();

        switch (mainState) {
            case mainStates::ready:
                switch (theEvent) {
                    case event::feedHoldResumeButtonPressed:
                        if (!theMotionCtrl.theMotionBuffer.isEmpty()) {
                            theMotionCtrl.theState = motionState::running;
                        } else {
                            theLog.output(loggingLevel::Error, "go without data");
                        }
                        break;
                    default:
                        break;
                }
            case mainStates::homing:
                homing(theEvent);
                break;
            case mainStates::probing:
                probing(theEvent);
                break;
            case mainStates::running:
                switch (theEvent) {
                    case event::allMotionsCompleted:
                        gotoState(mainStates::ready);
                        break;
                    case event::feedHoldResumeButtonPressed:
                        theMotionCtrl.theState = motionState::stopping;
                        theMotionCtrl.optimize();
                        theMotionCtrl.optimize();
                        theMotionCtrl.optimize();
                        break;
                    default:
                        break;
                }
                break;
            case mainStates::error:
                break;
            default:
                break;
        }
    }
}

const char *mainController::toString(mainStates theState) const {
    switch (theState) {
        case mainStates::ready:
            return "ready";
            break;
        case mainStates::homing:
            return "homing";
            break;
        case mainStates::probing:
            return "probing";
            break;
        case mainStates::running:
            return "running";
            break;
        case mainStates::error:
            return "error";
            break;
        default:
            return "unknown";
            break;
    }
}

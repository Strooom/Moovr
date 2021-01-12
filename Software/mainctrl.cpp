// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "logging.h"
#include "mainctrl.h"
#include "eventbuffer.h"
#include "hostinterface.h"

extern uLog theLog;

mainController::mainController(machineProperties &someMachineProperties, eventBuffer &anEventBuffer, HostInterfaceUart &aHostInterface, stepBuffer &aStepBuffer) : theMachineProperties(someMachineProperties), theEventBuffer(anEventBuffer), theHostInterface(aHostInterface), theStepBuffer(aStepBuffer) {
}

void mainController::run() {
    theMotionCtrl.run();
    //theMotionCtrl.optimize();
    handleEvents();          // handle all events such as buttons pressed, limit switches triggering, motions being completed..
    //handleMessages();        // handle all input from the external hostInterface
}

void mainController::handleMessages() {
    if (theHostInterface.hasMessage()) {
        theHostInterface.getMessage(commandLine);

        command aCommand;

        if (ESC == commandLine[0]) {
            aCommand = (command)commandLine[1];        // non gCode commands
        } else {
            aCommand = command::gCode;        // gCode
        }

        switch (theMainState) {
            case mainState::Ready:
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
                                    //mainState = mainState::RUNNING;
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
                    case command::uploadFile:
                        break;
                    case command::doHome:
                    case command::doProbe:
                    case command::jog:
                    case command::executeFile:
                    case command::override:
                        break;
                    default:
                        break;
                }
                break;

            case mainState::Loading:
                switch (aCommand)        // then , depending on the command...
                {
                    case command::gCode:
                    case command::doHome:
                    case command::doProbe:
                    case command::jog:
                    case command::uploadFile:
                    case command::executeFile:
                    case command::override:
                        //theHostInterface.sendMessage("invalid command\n");
                        // unsupported commands in this state...
                        break;
                    default:
                        // undefined command codes...
                        break;
                }
            default:
                break;
        }

        //responseMsg[0] = 0x0;	// clear the responseString
        //theHostInterface.getStatus(responseMsg, 0xFF);
        //theHostInterface.sendMessage(responseMsg);

        //responseMsg[0] = 0x0;	// clear the responseString
        //theParser.getState(responseMsg, 0xFF);
        //theHostInterface.sendMessage(responseMsg);

        //responseMsg[0] = 0x0;	// clear the responseString
        //theMotion.getState(responseMsg, 0xFF);	// This will return the motionBufferLevel, which is needed for the sndr to do its flowControl
        //theHostInterface.sendMessage(responseMsg);
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

void mainController::handleEvents() {
    while (theEventBuffer.hasEvents()) {
        event theEvent = theEventBuffer.popEvent();

        switch (theMainState) {
            case mainState::Ready:
                switch (theEvent) {
                    case event::feedHoldResumeButtonPressed:
                        if (!theMotionCtrl.theMotionBuffer.isEmpty()) {
                            theMotionCtrl.theState = motionState::running;
                            theMainState           = mainState::Running;
                            theLog.output(loggingLevel::Debug, toString());
                        } else {
                            theLog.output(loggingLevel::Error, "go without data");
                        }
                        break;
                    default:
                        break;
                }
            case mainState::Homing:
                homing(theEvent);
                break;
            case mainState::Jogging:
                break;
            case mainState::Probing:
                probing(theEvent);
                break;
            case mainState::Running:
                switch (theEvent) {
                    case event::allMotionsCompleted:
                        theMainState           = mainState::Ready;
                        theLog.output(loggingLevel::Debug, toString());
                        break;
                    case event::feedHoldResumeButtonPressed:
                        theMotionCtrl.theState = motionState::stopping;
                        theMotionCtrl.optimize();
                        theMotionCtrl.optimize();
                        theMotionCtrl.optimize();
                        break;
                    case event::motionStopped:
                        theMainState = mainState::Pausing;
                        theLog.output(loggingLevel::Debug, toString());
                        break;
                    default:
                        break;
                }
                break;
            case mainState::Pausing:
                switch (theEvent) {
                    case event::feedHoldResumeButtonPressed:
                        theMotionCtrl.theState = motionState::running;
                        theMotionCtrl.optimize();
                        theMotionCtrl.optimize();
                        theMotionCtrl.optimize();
                        theMainState = mainState::Running;
                        theLog.output(loggingLevel::Debug, toString());
                        break;
                    default:
                        break;
                }
                break;
            case mainState::Loading:
                break;
            case mainState::Error:
                break;
            default:
                break;
        }
    }
}


    const char *mainController::toString() const {
    switch (theMainState) {
            case mainState::Ready:
            return "ready";
                break;
            case mainState::Homing:
                return "";
                break;
            case mainState::Jogging:
                return "";
                break;
            case mainState::Probing:
                return "";
                break;
            case mainState::Running:
                return "running";
                break;
            case mainState::Pausing:
                return "pausing";
                break;
            case mainState::Loading:
                return "";
                break;
            case mainState::Error:
                return "error";
                break;
            default:
                return "unknown";
                break;
    }
}

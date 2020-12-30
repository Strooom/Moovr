// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "maincontroller.h"
#include "eventbuffer.h"
#include "hostinterface.h"

mainController::mainController(eventBuffer &anEventBuffer, HostInterfaceUart &aHostInterface, stepBuffer &aStepBuffer) : theEventBuffer(anEventBuffer), theHostInterface(aHostInterface), theStepBuffer(aStepBuffer) {
}

void mainController::run() {
    theMotionCtrl.run();
    theMotionCtrl.optimize();
    handleEvents();              // handle all events such as buttons pressed, limit switches triggering, motions being completed..
    handleMessages();            // handle all input from the external hostInterface
}

void mainController::handleMessages() {
    if (theHostInterface.hasMessage()) {
        theHostInterface.getMessage(commandLine);

        command command;

        if (ESC == commandLine[0]) {
            command = (command)commandLine[1];        // non gCode commands
        } else {
            command = command::gCode;        // gCode
        }

        switch (mainState)        // first, depending on the state...
        {
            case mainState::Ready:
                switch (command)        // then , depending on the command...
                {
                    case command::gCode:
                        theParser.getBlock(commandLine);        // Parse the line of gCode text into a gCodeBlock
                        while (theParser.getNmbrWords() > 0) {
                            theParser.parseBlock(theParseResult);        // Parse the gCode Block into a gCode state update and/or a gCodeMotion
                            switch (theParseResult.theParseResultType) {
                                case gCodeParserResult::ParseResultType::OkContextUpdateOnly:
                                    break;
                                case gCodeParserResult::ParseResultType::OkContextUpdateAndMotion:
                                    theMotion.append(theParseResult);
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

                        // Cancel in Ready state -> reboot or restart ?
                        break;

                    case command::uploadFile:
                        break;

                    case command::doHome:
                    case command::doProbe:
                    case command::jog:
                    case command::executeFile:
                    case command::override:
                        //theHostInterface.sendMessage("invalid command\n");
                        // unsupported commands in this state...
                        break;

                    default:
                        // undefined command codes...
                        break;
                }
                break;

            case mainState::Loading:
                switch (command)        // then , depending on the command...
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

void mainController::handleEvents() {
    while (theEventBuffer.hasEvents()) {
        event theEvent = theEventBuffer.popEvent();

        switch (mainState) {
            case mainState::Ready:
                switch (theEvent) {
                    case event::emergencyStopButtonPressed:
                        break;
                    case event::emergencyStopButtonReleased:
                        break;
                    case event::feedHoldResumeButtonPressed:
                        break;
                    case event::feedHoldResumeButtonReleased:
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
                    case event::motionCompleted:
                        break;
                }
            case mainState::Homing:
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
                }
                break;
            case mainState::Jogging:
                break;
            case mainState::Probing:
                break;
            case mainState::Running:
                switch (theEvent) {
                    case event::emergencyStopButtonPressed:
                        break;
                    case event::emergencyStopButtonReleased:
                        break;
                    case event::feedHoldResumeButtonPressed:
                        break;
                    case event::feedHoldResumeButtonReleased:
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
                    case event::motionCompleted:
                        break;
                }
                break;
            case mainState::Pausing:
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

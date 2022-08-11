// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "maincontroller.h"
#include "eventbuffer.h"
#include "motionctrl.h"

extern eventBuffer theEventBuffer;
extern motionCtrl theMotionController;


mainController::mainController()
    {
    // INPUTS
    // Inputs [0..7] : PD[0..7]
    // Inputs [8..9] : PE[24..25]
    // Inputs [10..11] : PA[16..17]

    // PORTD_PCR0 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // PORTD_PCR1 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // PORTD_PCR2 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // PORTD_PCR3 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // PORTD_PCR4 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // PORTD_PCR5 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // //PORTD_PCR6 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // //PORTD_PCR7 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // GPIOD_PDDR = 0x000C0000;

    // PORTE_PCR24 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // PORTE_PCR25 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // GPIOE_PDDR = 0x03000000;

    // PORTA_PCR16 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // //PORTA_PCR17 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    // GPIOA_PDDR = 0x00030000;


    // for (uint8_t i = 0; i <= ButtonSwitch::debounceMaxCount; i++)		// initialize the input switches by running them a number of cycles
    //     {
    //     switchesAndButtons[(uint8_t) limitSwitchButton::xMin].run(GPIOD_PDIR & 0x00000001);
    //     switchesAndButtons[(uint8_t) limitSwitchButton::xMax].run(GPIOD_PDIR & 0x00000002);
    //     switchesAndButtons[(uint8_t) limitSwitchButton::yMin].run(GPIOD_PDIR & 0x00000004);
    //     switchesAndButtons[(uint8_t) limitSwitchButton::yMax].run(GPIOD_PDIR & 0x00000008);
    //     switchesAndButtons[(uint8_t) limitSwitchButton::zMin].run(GPIOD_PDIR & 0x00000010);
    //     switchesAndButtons[(uint8_t) limitSwitchButton::zMax].run(GPIOD_PDIR & 0x00000020);

    //     switchesAndButtons[(uint8_t) limitSwitchButton::stop].run(GPIOE_PDIR & 0x01000000);
    //     switchesAndButtons[(uint8_t) limitSwitchButton::hold].run(GPIOE_PDIR & 0x02000000);
    //     switchesAndButtons[(uint8_t) limitSwitchButton::probe].run(GPIOA_PDIR & 0x00010000);
    //     }

    // theHostInterface.sendMessage("Moovr V0.2 - https://github.com/Strooom/Moovr\n");
    }

void mainController::run()
    {
    // theMotionController.run();				// calculate steps from the motion = filling the stepBuffer from the motionBuffer
    // theMotionController.optimize();			// optimize the Motion, to maximize speed, or decelerate to stop/pause, or adjust feedrates from overrides..
    handleEvents();					// handle all events such as buttons pressed, limit switches triggering, motions being completed..
    handleMessages();				// handle all input from the external hostInterface
    }

// void mainController::serialInterrupt()
//     {
//     uint8_t status;
//     status = UART0_S1;						// read status register. Interrupt-flag is cleared after reading this register AND reading/writing the UART0_D register until they are empty/full
//     if (status & UART_S1_RDRF)				// Transmit ready or Receive event ?
//         {
//         theHostInterface.rxReady();			// Handle ' byte received' event
//         }
//     if (status & UART_S1_TDRE)				// Transmit ready event ?
//         {
//         theHostInterface.txReady();			// Handle 'byte sent' event
//         }
//     }

// void mainController::pit1Interrupt()
//     {
//     theMotion.output();  // output stepper signals to the motors
//     }

// void mainController::pit2Interrupt()
//     {
//     // read the inputs, debounce them and if any Buttonevents, translate then into a mainController event..
//     Buttonevent theEvent;

//     theEvent =  switchesAndButtons[(uint8_t) limitSwitchButton::xMin].run(GPIOD_PDIR & 0x00000001);
//     switch (theEvent)
//         {
//         case Buttonevent::closed:
//             theEventBuffer.pushevent(event::limitSwitchXMinClosed);
//             GPIOB_PDOR = 0;					// Enable Motors 123 and 456 ?

// 			logger.logNow("motors enabled\n");
//             break;
//         case Buttonevent::opened:
// 			theEventBuffer.pushevent(event::limitSwitchXMinOpened);
//             GPIOB_PDOR = 0x40000;		// Disable Motors 123 Apparently driving the signal HIGH, Disables the motors, keeping it LOW enables them
//             logger.logNow("motors disabled\n");
//             break;
//         }

//     theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::xMax].run(GPIOD_PDIR & 0x00000002);
//     switch (theEvent)
//         {
//         case Buttonevent::closed:
// 			theEventBuffer.pushevent(event::limitSwitchXMaxClosed);
//             break;
//         case Buttonevent::opened:
// 			theEventBuffer.pushevent(event::limitSwitchXMaxOpened);
//             break;
//         }

//     theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::yMin].run(GPIOD_PDIR & 0x00000004);
//     switch (theEvent)
//         {
//         case Buttonevent::closed:
// 			theEventBuffer.pushevent(event::limitSwitchYMinClosed);
//             break;
//         case Buttonevent::opened:
// 			theEventBuffer.pushevent(event::limitSwitchYMinOpened);
//             break;
//         }

//     theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::yMax].run(GPIOD_PDIR & 0x00000008);
//     switch (theEvent)
//         {
//         case Buttonevent::closed:
// 			theEventBuffer.pushevent(event::limitSwitchYMaxClosed);
//             break;
//         case Buttonevent::opened:
// 			theEventBuffer.pushevent(event::limitSwitchYMaxOpened);
//             break;
//         }

//     theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::zMin].run(GPIOD_PDIR & 0x00000010);
//     switch (theEvent)
//         {
//         case Buttonevent::closed:
// 			theEventBuffer.pushevent(event::limitSwitchZMinClosed);
//             break;
//         case Buttonevent::opened:
// 			theEventBuffer.pushevent(event::limitSwitchZMinOpened);
//             break;
//         }

//     theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::zMax].run(GPIOD_PDIR & 0x00000020);
//     switch (theEvent)
//         {
//         case Buttonevent::closed:
// 			theEventBuffer.pushevent(event::limitSwitchZMaxClosed);
//             break;
//         case Buttonevent::opened:
// 			theEventBuffer.pushevent(event::limitSwitchZMaxOpened);
//             break;
//         }

//     theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::stop].run(GPIOE_PDIR & 0x01000000);
//     switch (theEvent)
//         {
//         case Buttonevent::closed:
// 			theEventBuffer.pushevent(event::emergencyStopButtonPressed);
//             break;
//         case Buttonevent::opened:
// 			theEventBuffer.pushevent(event::emergencyStopButtonReleased);
//             break;
//         }

//     theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::hold].run(GPIOE_PDIR & 0x02000000);
//     switch (theEvent)
//         {
//         case Buttonevent::closed:
// 			theEventBuffer.pushevent(event::feedHoldResumeButtonPressed);
//             break;
//         case Buttonevent::opened:
// 			theEventBuffer.pushevent(event::feedHoldResumeButtonReleased);
//             break;
//         }

//     theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::probe].run(GPIOA_PDIR & 0x00010000);
//     switch (theEvent)
//         {
//         case Buttonevent::closed:
// 			theEventBuffer.pushevent(event::probeSwitchClosed);
//             break;
//         case Buttonevent::opened:
// 			theEventBuffer.pushevent(event::probeSwitchOpenend);
//             break;
//         }
//     }

void mainController::handleMessages()
    {
    // if (theHostInterface.hasMessage())
    //     {
    //     theHostInterface.getMessage(commandLine);

    //     command theCommand;

    //     if (ESC == commandLine[0])
    //         {
    //         theCommand = (command)commandLine[1];		// non gCode commands
    //         }
    //     else
    //         {
    //         theCommand = command::gCode;				// gCode
    //         }

    //     switch (theMainState)								// first, depending on the state...
    //         {
    //         case mainState::ready:
    //             switch (theCommand)						// then , depending on the command...
    //                 {
    //                 case command::gCode:
    //                     theParser.getBlock(commandLine);	// Parse the line of gCode text into a gCodeBlock
    //                     while (theParser.getNmbrWords() > 0)
    //                         {
    //                         theParser.parseBlock(theParseResult);							// Parse the gCode Block into a gCode state update and/or a gCodeMotion
    //                         switch (theParseResult.theParseResultType)
    //                             {
    //                             case gCodeParserResult::ParseResultType::OkContextUpdateOnly:
    //                                 break;
    //                             case gCodeParserResult::ParseResultType::OkContextUpdateAndMotion:
    //                                 theMotion.append(theParseResult);
    //                                 //mainState = mainState::RUNNING;
    //                                 break;
    //                             case gCodeParserResult::ParseResultType::Error:
    //                                 break;
    //                             default:
    //                                 break;
    //                             }
    //                         }
    //                     break;

    //                 case command::cancel:
    //                     break;

    //                     // Cancel in Ready state -> reboot or restart ?
    //                     break;

    //                 case command::uploadFile:
    //                     break;

    //                 case command::doHome:
    //                 case command::doProbe:
    //                 case command::jog:
    //                 case command::executeFile:
    //                 case command::override:
    //                     //theHostInterface.sendMessage("invalid Command\n");
    //                     // unsupported commands in this state...
    //                     break;

    //                 default:
    //                     // undefined command codes...
    //                     break;
    //                 }
    //             break;

    //         case mainState::loading:
    //             switch (command)						// then , depending on the command...
    //                 {
    //                 case command::gCode:
    //                 case command::doHome:
    //                 case command::doProbe:
    //                 case command::jog:
    //                 case command::uploadFile:
    //                 case command::executeFile:
    //                 case command::override:
    //                     //theHostInterface.sendMessage("invalid Command\n");
    //                     // unsupported commands in this state...
    //                     break;
    //                 default:
    //                     // undefined command codes...
    //                     break;
    //                 }
    //         default:
    //             break;
    //         }

    //     //responseMsg[0] = 0x0;	// clear the responseString
    //     //theHostInterface.getStatus(responseMsg, 0xFF);
    //     //theHostInterface.sendMessage(responseMsg);

    //     //responseMsg[0] = 0x0;	// clear the responseString
    //     //theParser.getState(responseMsg, 0xFF);
    //     //theHostInterface.sendMessage(responseMsg);

    //     //responseMsg[0] = 0x0;	// clear the responseString
    //     //theMotion.getState(responseMsg, 0xFF);	// This will return the motionBufferLevel, which is needed for the sndr to do its flowControl
    //     //theHostInterface.sendMessage(responseMsg);
    //     }
    }

void mainController::handleEvents()
    {
    // while (theEventBuffer.hasEvents())
    //     {
    //     event theEvent = theEventBuffer.popEvent();

    //     switch (mainState)
    //         {
    //         case mainState::ready:
    //             switch (theEvent)
    //                 {
    //                 case event::emergencyStopButtonPressed:
    //                     break;
    //                 case event::emergencyStopButtonReleased:
    //                     break;
    //                 case event::feedHoldResumeButtonPressed:
    //                     break;
    //                 case event::feedHoldResumeButtonReleased:
    //                     break;
    //                 case event::limitSwitchXMinClosed:
    //                     break;
    //                 case event::limitSwitchYMinClosed:
    //                     break;
    //                 case event::limitSwitchZMinClosed:
    //                     break;
    //                 case event::limitSwitchXMaxClosed:
    //                     break;
    //                 case event::limitSwitchYMaxClosed:
    //                     break;
    //                 case event::limitSwitchZMaxClosed:
    //                     break;
    //                 case event::limitSwitchXMinOpened:
    //                     break;
    //                 case event::limitSwitchYMinOpened:
    //                     break;
    //                 case event::limitSwitchZMinOpened:
    //                     break;
    //                 case event::limitSwitchXMaxOpened:
    //                     break;
    //                 case event::limitSwitchYMaxOpened:
    //                     break;
    //                 case event::limitSwitchZMaxOpened:
    //                     break;
    //                 case event::motionCompleted:
    //                     break;
    //                 }
    //         case mainState::homing:
    //             switch (theEvent)
    //                 {
    //                 case event::emergencyStopButtonPressed:
    //                     break;
    //                 case event::emergencyStopButtonReleased:
    //                     break;
    //                 case event::feedHoldResumeButtonPressed:
    //                     break;
    //                 case event::feedHoldResumeButtonReleased:
    //                     break;
    //                 case event::probeSwitchClosed:
    //                     break;
    //                 case event::probeSwitchOpenend:
    //                     break;
    //                 case event::limitSwitchXMinClosed:
    //                     break;
    //                 case event::limitSwitchYMinClosed:
    //                     break;
    //                 case event::limitSwitchZMinClosed:
    //                     break;
    //                 case event::limitSwitchXMaxClosed:
    //                     break;
    //                 case event::limitSwitchYMaxClosed:
    //                     break;
    //                 case event::limitSwitchZMaxClosed:
    //                     break;
    //                 case event::limitSwitchXMinOpened:
    //                     break;
    //                 case event::limitSwitchYMinOpened:
    //                     break;
    //                 case event::limitSwitchZMinOpened:
    //                     break;
    //                 case event::limitSwitchXMaxOpened:
    //                     break;
    //                 case event::limitSwitchYMaxOpened:
    //                     break;
    //                 case event::limitSwitchZMaxOpened:
    //                     break;
    //                 case event::motionAdded:
    //                     break;
    //                 case event::motionCompleted:
    //                     break;
    //                 case event::allMotionsCompleted:
    //                     break;
    //                 }
    //             break;
    //         case mainState::jogging:
    //             break;
    //         case mainState::probing:
    //             break;
    //         case mainState::running:
    //             switch (theEvent)
    //                 {
    //                 case event::emergencyStopButtonPressed:
    //                     break;
    //                 case event::emergencyStopButtonReleased:
    //                     break;
    //                 case event::feedHoldResumeButtonPressed:
    //                     break;
    //                 case event::feedHoldResumeButtonReleased:
    //                     break;
    //                 case event::limitSwitchXMinClosed:
    //                     break;
    //                 case event::limitSwitchYMinClosed:
    //                     break;
    //                 case event::limitSwitchZMinClosed:
    //                     break;
    //                 case event::limitSwitchXMaxClosed:
    //                     break;
    //                 case event::limitSwitchYMaxClosed:
    //                     break;
    //                 case event::limitSwitchZMaxClosed:
    //                     break;
    //                 case event::limitSwitchXMinOpened:
    //                     break;
    //                 case event::limitSwitchYMinOpened:
    //                     break;
    //                 case event::limitSwitchZMinOpened:
    //                     break;
    //                 case event::limitSwitchXMaxOpened:
    //                     break;
    //                 case event::limitSwitchYMaxOpened:
    //                     break;
    //                 case event::limitSwitchZMaxOpened:
    //                     break;
    //                 case event::motionCompleted:
    //                     break;
    //                 }
    //             break;
    //         case mainState::pausing:
    //             break;
    //         case mainState::loading:
    //             break;
    //         case mainState::error:
    //             break;
    //         default:
    //             break;
    //         }
    //     }
    }


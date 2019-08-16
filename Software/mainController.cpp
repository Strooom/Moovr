// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "logging.h"
#include "event.h"
#include "mainController.h"
#include "HostInterface.h"

#ifdef log_all
extern Logger logger;
//#define log_mainController_class				// logging output for the mainController class/object
//#define log_mainController_stateChanges
#define log_mainController_message_command
#define log_mainController_message_gcode
//#define log_mainController_event
#define log_gCode_ParseResult
//#define logStepperFill
#endif

MainController::MainController()
    {
#ifdef log_mainController_class
    logger.logNow("MainController constructor start\n");
#endif

#ifndef WIN32
    // INPUTS
    // Inputs [0..7] : PD[0..7]
    // Inputs [8..9] : PE[24..25]
    // Inputs [10..11] : PA[16..17]

    PORTD_PCR0 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    PORTD_PCR1 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    PORTD_PCR2 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    PORTD_PCR3 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    PORTD_PCR4 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    PORTD_PCR5 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    //PORTD_PCR6 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    //PORTD_PCR7 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    GPIOD_PDDR = 0x000C0000;

    PORTE_PCR24 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    PORTE_PCR25 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    GPIOE_PDDR = 0x03000000;

    PORTA_PCR16 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    //PORTA_PCR17 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    GPIOA_PDDR = 0x00030000;

#ifdef log_mainController_class
    logger.logNow("PD[0..7], PE[24..25], PA[16..17] set as input\n");
#endif

    for (uint8_t i = 0; i <= ButtonSwitch::debounceMaxCount; i++)		// initialize the input switches by running them a number of cycles
        {
        switchesAndButtons[(uint8_t) limitSwitchButton::xMin].run(GPIOD_PDIR & 0x00000001);
        switchesAndButtons[(uint8_t) limitSwitchButton::xMax].run(GPIOD_PDIR & 0x00000002);
        switchesAndButtons[(uint8_t) limitSwitchButton::yMin].run(GPIOD_PDIR & 0x00000004);
        switchesAndButtons[(uint8_t) limitSwitchButton::yMax].run(GPIOD_PDIR & 0x00000008);
        switchesAndButtons[(uint8_t) limitSwitchButton::zMin].run(GPIOD_PDIR & 0x00000010);
        switchesAndButtons[(uint8_t) limitSwitchButton::zMax].run(GPIOD_PDIR & 0x00000020);

        switchesAndButtons[(uint8_t) limitSwitchButton::stop].run(GPIOE_PDIR & 0x01000000);
        switchesAndButtons[(uint8_t) limitSwitchButton::hold].run(GPIOE_PDIR & 0x02000000);
        switchesAndButtons[(uint8_t) limitSwitchButton::probe].run(GPIOA_PDIR & 0x00010000);
        }


#ifdef log_mainController_class
    logger.logNow("input switch/button statemachines initialized\n");
#endif
#endif


#ifdef log_mainController_class
    logger.logNow("MainController constructor end\n");
#endif

    theHostInterface.sendMessage("Moovr V0.2 - https://github.com/Strooom/Moovr\n");
    }

void MainController::run()
    {
    //theMotion.run();				// calculate steps from the motion = filling the stepBuffer from the motionBuffer
    theMotion.optimize();			// optimize the Motion, to maximize speed, or decelerate to stop/pause, or adjust feedrates from overrides..
    //handleEvents();					// handle all events such as buttons pressed, limit switches triggering, motions being completed..
    handleMessages();				// handle all input from the external hostInterface

    theMotion.toString();
    }

void MainController::serialInterrupt()
    {
#ifndef WIN32
    uint8_t status;
    status = UART0_S1;						// read status register. Interrupt-flag is cleared after reading this register AND reading/writing the UART0_D register until they are empty/full
    if (status & UART_S1_RDRF)				// Transmit ready or Receive event ?
        {
        theHostInterface.rxReady();			// Handle ' byte received' event
        }
    if (status & UART_S1_TDRE)				// Transmit ready event ?
        {
        theHostInterface.txReady();			// Handle 'byte sent' event
        }
#endif
    }

void MainController::pit1Interrupt()
    {
    theMotion.output();  // output stepper signals to the motors
    }

void MainController::pit2Interrupt()
    {
#ifndef WIN32
    // read the inputs, debounce them and if any ButtonEvents, translate then into a mainController event..
    ButtonEvent theEvent;

    theEvent =  switchesAndButtons[(uint8_t) limitSwitchButton::xMin].run(GPIOD_PDIR & 0x00000001);
    switch (theEvent)
        {
        case ButtonEvent::closed:
            theEventBuffer.pushEvent(Event::limitSwitchXMinClosed);
            GPIOB_PDOR = 0;					// Enable Motors 123 and 456 ?
            logger.logNow("motors enabled\n");
            break;
        case ButtonEvent::opened:
			theEventBuffer.pushEvent(Event::limitSwitchXMinOpened);
            GPIOB_PDOR = 0x40000;		// Disable Motors 123 Apparently driving the signal HIGH, Disables the motors, keeping it LOW enables them
            logger.logNow("motors disabled\n");
            break;
        }

    theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::xMax].run(GPIOD_PDIR & 0x00000002);
    switch (theEvent)
        {
        case ButtonEvent::closed:
			theEventBuffer.pushEvent(Event::limitSwitchXMaxClosed);
            break;
        case ButtonEvent::opened:
			theEventBuffer.pushEvent(Event::limitSwitchXMaxOpened);
            break;
        }

    theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::yMin].run(GPIOD_PDIR & 0x00000004);
    switch (theEvent)
        {
        case ButtonEvent::closed:
			theEventBuffer.pushEvent(Event::limitSwitchYMinClosed);
            break;
        case ButtonEvent::opened:
			theEventBuffer.pushEvent(Event::limitSwitchYMinOpened);
            break;
        }

    theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::yMax].run(GPIOD_PDIR & 0x00000008);
    switch (theEvent)
        {
        case ButtonEvent::closed:
			theEventBuffer.pushEvent(Event::limitSwitchYMaxClosed);
            break;
        case ButtonEvent::opened:
			theEventBuffer.pushEvent(Event::limitSwitchYMaxOpened);
            break;
        }

    theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::zMin].run(GPIOD_PDIR & 0x00000010);
    switch (theEvent)
        {
        case ButtonEvent::closed:
			theEventBuffer.pushEvent(Event::limitSwitchZMinClosed);
            break;
        case ButtonEvent::opened:
			theEventBuffer.pushEvent(Event::limitSwitchZMinOpened);
            break;
        }

    theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::zMax].run(GPIOD_PDIR & 0x00000020);
    switch (theEvent)
        {
        case ButtonEvent::closed:
			theEventBuffer.pushEvent(Event::limitSwitchZMaxClosed);
            break;
        case ButtonEvent::opened:
			theEventBuffer.pushEvent(Event::limitSwitchZMaxOpened);
            break;
        }

    theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::stop].run(GPIOE_PDIR & 0x01000000);
    switch (theEvent)
        {
        case ButtonEvent::closed:
			theEventBuffer.pushEvent(Event::emergencyStopButtonPressed);
            break;
        case ButtonEvent::opened:
			theEventBuffer.pushEvent(Event::emergencyStopButtonReleased);
            break;
        }

    theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::hold].run(GPIOE_PDIR & 0x02000000);
    switch (theEvent)
        {
        case ButtonEvent::closed:
			theEventBuffer.pushEvent(Event::feedHoldResumeButtonPressed);
            break;
        case ButtonEvent::opened:
			theEventBuffer.pushEvent(Event::feedHoldResumeButtonReleased);
            break;
        }

    theEvent = switchesAndButtons[(uint8_t) limitSwitchButton::probe].run(GPIOA_PDIR & 0x00010000);
    switch (theEvent)
        {
        case ButtonEvent::closed:
			theEventBuffer.pushEvent(Event::probeSwitchClosed);
            break;
        case ButtonEvent::opened:
			theEventBuffer.pushEvent(Event::probeSwitchOpenend);
            break;
        }

#endif
    }

void MainController::handleMessages()
    {
    if (theHostInterface.hasMessage())
        {
        theHostInterface.getMessage(commandLine);

        Command command;

        if (ESC == commandLine[0])
            {
            command = (Command)commandLine[1];		// non gCode commands
#ifdef log_mainController_message_command
            logger.logNow("received message : command ");
#endif // log_mainController_message
            }
        else
            {
            command = Command::gCode;				// gCode
#ifdef log_mainController_message_gcode
            logger.logNow("mainCtrl received gCode : ");
            logger.logNow(commandLine);
            logger.logNow("\n");
#endif // log_mainController_message
            }


#ifdef log_mainController_stateChanges
        logger.logNow("mainCtrl handleEvents : entry state ");
        logger.logNow((uint32_t)mainState);
        logger.logNow("\n");

#endif // log_mainController_stateChanges

        switch (mainState)								// first, depending on the state...
            {
            case MainState::Ready:
                switch (command)						// then , depending on the command...
                    {
                    case Command::gCode:
                        theParser.getBlock(commandLine);	// Parse the line of gCode text into a gCodeBlock
                        while (theParser.getNmbrWords() > 0)
                            {
                            theParser.parseBlock(theParseResult);							// Parse the gCode Block into a gCode state update and/or a gCodeMotion
#ifdef log_gCode_ParseResult
                            theParseResult.toString();
#endif
                            switch (theParseResult.theParseResultType)
                                {
                                case gCodeParserResult::ParseResultType::OkContextUpdateOnly:
                                    break;
                                case gCodeParserResult::ParseResultType::OkContextUpdateAndMotion:
                                    theMotion.append(theParseResult);
                                    //mainState = MainState::RUNNING;
                                    break;
                                case gCodeParserResult::ParseResultType::Error:
                                    break;
                                default:
                                    break;
                                }
                            }
                        break;

                    case Command::cancel:
                        break;

                        // Cancel in Ready state -> reboot or restart ?
                        break;

                    case Command::uploadFile:
                        break;

                    case Command::doHome:
                    case Command::doProbe:
                    case Command::jog:
                    case Command::executeFile:
                    case Command::override:
                        //theHostInterface.sendMessage("invalid Command\n");
                        // unsupported commands in this state...
                        break;

                    default:
                        // undefined command codes...
                        break;
                    }
                break;

            case MainState::Loading:
                switch (command)						// then , depending on the command...
                    {
                    case Command::gCode:
                    case Command::doHome:
                    case Command::doProbe:
                    case Command::jog:
                    case Command::uploadFile:
                    case Command::executeFile:
                    case Command::override:
                        //theHostInterface.sendMessage("invalid Command\n");
                        // unsupported commands in this state...
                        break;
                    default:
                        // undefined command codes...
                        break;
                    }
            default:
                break;
            }
#ifdef log_mainController_stateChanges
        logger.logNow("mainCtrl handleEvents : exit state ");
        logger.logNow((uint32_t)mainState);
        logger.logNow("\n");
#endif // log_mainController_stateChanges

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

void MainController::handleEvents()
    {
    while (theEventBuffer.hasEvents())
        {
        Event theEvent = theEventBuffer.popEvent();
#ifdef log_mainController_event
        logger.logNow("events ");
        logger.logNow((uint32_t)theEvent);
#endif

#ifdef log_mainController_stateChanges
        logger.logNow("handleEvents : entry state ");
        logger.logNow((uint32_t) mainState);
#endif // log_mainController_stateChanges


        switch (mainState)
            {
            case MainState::Ready:
                switch (theEvent)
                    {
                    case Event::emergencyStopButtonPressed:
//                        logger.logNow("Emergency Stop pressed");
                        break;
                    case Event::emergencyStopButtonReleased:
//                        logger.logNow("Emergency Stop released");
                        break;
                    case Event::feedHoldResumeButtonPressed:
                        break;
                    case Event::feedHoldResumeButtonReleased:
                        break;
                    case Event::limitSwitchXMinClosed:
                        logger.logNow("Motors enabled\n");
                        break;
                    case Event::limitSwitchYMinClosed:
                        break;
                    case Event::limitSwitchZMinClosed:
                        break;
                    case Event::limitSwitchXMaxClosed:
                        break;
                    case Event::limitSwitchYMaxClosed:
                        break;
                    case Event::limitSwitchZMaxClosed:
                        break;
                    case Event::limitSwitchXMinOpened:
                        logger.logNow("Motors disabled\n");
                        break;
                    case Event::limitSwitchYMinOpened:
                        break;
                    case Event::limitSwitchZMinOpened:
                        break;
                    case Event::limitSwitchXMaxOpened:
                        break;
                    case Event::limitSwitchYMaxOpened:
                        break;
                    case Event::limitSwitchZMaxOpened:
                        break;
                    case Event::motionCompleted:
                        break;
                    }
            case MainState::Homing:
                switch (theEvent)
                    {
                    case Event::emergencyStopButtonPressed:
                        break;
                    case Event::emergencyStopButtonReleased:
                        break;
                    case Event::feedHoldResumeButtonPressed:
                        break;
                    case Event::feedHoldResumeButtonReleased:
                        break;
                    case Event::probeSwitchClosed:
                        break;
                    case Event::probeSwitchOpenend:
                        break;
                    case Event::limitSwitchXMinClosed:
                        break;
                    case Event::limitSwitchYMinClosed:
                        break;
                    case Event::limitSwitchZMinClosed:
                        break;
                    case Event::limitSwitchXMaxClosed:
                        break;
                    case Event::limitSwitchYMaxClosed:
                        break;
                    case Event::limitSwitchZMaxClosed:
                        break;
                    case Event::limitSwitchXMinOpened:
                        break;
                    case Event::limitSwitchYMinOpened:
                        break;
                    case Event::limitSwitchZMinOpened:
                        break;
                    case Event::limitSwitchXMaxOpened:
                        break;
                    case Event::limitSwitchYMaxOpened:
                        break;
                    case Event::limitSwitchZMaxOpened:
                        break;
                    case Event::motionAdded:
                        break;
                    case Event::motionCompleted:
                        break;
                    case Event::allMotionsCompleted:
                        break;
                    }
                break;
            case MainState::Jogging:
                break;
            case MainState::Probing:
                break;
            case MainState::Running:
                switch (theEvent)
                    {
                    case Event::emergencyStopButtonPressed:
                        break;
                    case Event::emergencyStopButtonReleased:
                        break;
                    case Event::feedHoldResumeButtonPressed:
                        break;
                    case Event::feedHoldResumeButtonReleased:
                        break;
                    case Event::limitSwitchXMinClosed:
                        break;
                    case Event::limitSwitchYMinClosed:
                        break;
                    case Event::limitSwitchZMinClosed:
                        break;
                    case Event::limitSwitchXMaxClosed:
                        break;
                    case Event::limitSwitchYMaxClosed:
                        break;
                    case Event::limitSwitchZMaxClosed:
                        break;
                    case Event::limitSwitchXMinOpened:
                        break;
                    case Event::limitSwitchYMinOpened:
                        break;
                    case Event::limitSwitchZMinOpened:
                        break;
                    case Event::limitSwitchXMaxOpened:
                        break;
                    case Event::limitSwitchYMaxOpened:
                        break;
                    case Event::limitSwitchZMaxOpened:
                        break;
                    case Event::motionCompleted:
                        break;
                    }
                break;
            case MainState::Pausing:
                break;
            case MainState::Loading:
                break;
            case MainState::Error:
                break;
            default:
                break;
            }
#ifdef log_mainController_stateChanges
        logger.logNow("handleEvents : exity state ");
        logger.logNow((uint32_t)mainState);
#endif // log_mainController_stateChanges
        }
    }


// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "logging.h"
#include "HostInterface.h"

#ifdef WIN32
#include <stdio.h>
#endif

extern Logger logger;

HostInterface::HostInterface()
    {
    };

void HostInterface::getMessage(char* destinationBuffer)
    {
    uint8_t byteReceived;
    uint8_t bytesRead = 0;

    while (rxBufferLevel > 0)
        {
        byteReceived = rxBuffer[rxBufferHead];								//	read a byte from the rxBuffer
        rxBufferHead = (rxBufferHead + 1) % rxBufferLength;					//  increment the bufferHead pointer
        rxBufferLevel--;
        destinationBuffer[bytesRead] = byteReceived;						//  copy byte to destination buffer
        bytesRead++;

        if ('\n' == byteReceived)
            {
            rxBufferLines--;												// we read a linefeed, so decrease the number of lines
            destinationBuffer[bytesRead] = 0x00;							// terminate string copied to destination
            return;
            }
        }
    // Problem : no CR terminated message...
    destinationBuffer[bytesRead] = 0x00;									// anyway... terminate string copied to destination
    };

void HostInterface::sendMessage(const char* sourceBuffer)
    {
    // If enough room in txBffer..
    // ..Copy message from the sourceBuffer to the txBuffer
    // Adjust buffer-properties

    uint8_t length;
    for (length = 0; 0 != sourceBuffer[length]; ++length);					// determine length of string to be sent in sourceBuffer

    if (length <= (txBufferLength - txBufferLevel))							// If there's enough free space in rxBuffer for this String
        {
        for (uint8_t index = 0; index < length; index++)					// Copy it - EXcluding terminating 0x00
            {
            if (txBufferLevel < txBufferLength)
                {
                txBuffer[(txBufferHead + txBufferLevel) % txBufferLength] = sourceBuffer[index];
                txBufferLevel++;
                }
            else
                {
                //lastError = commsError::TXOVERFLOW;									// Transmit Overflow : trying to send when txBuffer is full
                }
            }
        }
    else
        {
        //lastError = commsError::TXOVERFLOW;									// Transmit Overflow : trying to send when txBuffer is full
        }
    };

bool HostInterface::hasMessage() // const
    {
    return 	(rxBufferLines > 0);
    };

void HostInterface::getStatus(char *theMsg, uint8_t theSelectedItems = 0xFF)
    {
    if (theSelectedItems)
        {
        strcat(theMsg, "hostInterface");
        }
    if (theSelectedItems & (uint8_t)ReportItems::rxLevel)
        {
        strcat(theMsg, "|rxBufferLevel:");
        char tmpMsg[11] = "";
        itoa(rxBufferLevel, tmpMsg, 10);
        strcat(theMsg, tmpMsg);

        //snprintf(theMsg, 127, "hostInterface|rxBufferLevel:=%d", rxBufferLevel);
        }
    if (theSelectedItems & (uint8_t)ReportItems::msgLevel)
        {
        strcat(theMsg, "|rxBufferLines:");
        char tmpMsg[11] = "";
        itoa(rxBufferLines, tmpMsg, 10);
        strcat(theMsg, tmpMsg);
        }
    if (theSelectedItems & (uint8_t)ReportItems::test)
        {
        theMsg += strlen(theMsg);
        snprintf(theMsg, 127, "rxBufferLevel:%d", rxBufferLevel);
        }
    if (theSelectedItems)
        {
        strcat(theMsg, "\n");
        }
    }

// Derived class, implementing the HostInterface over UART0 of the Teensy 3.5/3.6


HostInterfaceUart::HostInterfaceUart()
    {
#ifdef log_hostInterface
    logger.logNow("HostInterfaceUart constructor start\n");
#endif
#ifndef WIN32
    // Initialize UART0 Hardware
    SIM_SCGC4 |= (0x01 << 10);                          // Enable the UART clock to power on UART0

    // Enable the GPIO port on which rx and tx will be connected
    SIM_SCGC5 |= (0x01 << 9);                           // Enable the PORTA clock, as the UART pins will appear on this port PA14, PA15
    PORTA_PCR14 |= PORT_PCR_MUX(3);                     // Activate UART ALT3 function for this pin : UART0_TX
    PORTA_PCR15 |= PORT_PCR_MUX(3);                     // Activate UART ALT3 function for this pin : UART0_RX

    UART0_C1 = UART_C1_ILT;
    UART0_C2 = 0;                                       // Disable receiver and transmitter and related interrupts - before setting BAUDrate


#ifdef K64
    // Teensy 3.5 @ 120 MHz
    // Baud rate is defined as a fraction of system clock, and so it needs different parameters for Teensy 3.5 vs 3.6
    // 38400 = 120M / (16 * 195,..)
    //UART0_BDH = 0;                                      // Write BDH first, then write BDL !
    //UART0_BDL = 195;
    //UART0_C4 = UART_C4_BRFA(10);
#endif
#ifdef K66
    // Teensy 3.6 @ 180 MHz
    // Baud rate is defined as a fraction of system clock, and so it needs different parameters for Teensy 3.5 vs 3.6
    UART0_BDH = 1;                                      // Write BDH first, then write BDL !
    UART0_BDL = 36;
    UART0_C4 = UART_C4_BRFA(31);
#endif

    UART0_C3 = 0;
    UART0_C5 = 0;
    UART0_PFIFO = 0;
    UART0_C2 = (UART_C2_TE | UART_C2_RE | UART_C2_RIE);						// Enable receiver and transmitter, and receiver interrupts. Transmitter interrupt will be enabled as soon as there is something in the TxBuffer
    NVIC_ENABLE_IRQ(IRQ_UART0_STATUS);
#else
    // Load the rxBuffer with contents from a file..
    FILE *gcodeFile;
    gcodeFile = fopen("C:\\Users\\Pascal\\Documents\\Visual Studio 2017\\Projects\\moovrWin\\moovrWin\\testGCode.nc", "r");
    // C:\Users\Pascal\Documents\Visual Studio 2017\Projects\moovrWin\moovrWin\

    char temp[256];
    uint32_t i;

    while (fgets(temp, 256, gcodeFile) != NULL)
        {
        i = 0;
        // now copy the contents to the rxBuffer;
        while (temp[i])
            {
            rxBuffer[(rxBufferHead + rxBufferLevel) % rxBufferLength] = temp[i];
            rxBufferLevel++;
            if ('\n' == temp[i])
                {
                rxBufferLines++;											// ... also increase number of 'lines' received
                }
			i++;
            }
        }
    fclose(gcodeFile);
#endif

#ifdef log_hostInterface
    logger.logNow("HostInterfaceUart constructor end\n");
#endif

    }

void HostInterfaceUart::rxReady()
    {
#ifndef WIN32
    if (rxBufferLevel < (rxBufferLength))									// if buffer NOT full...
        {
            {
            uint8_t byteReceived = UART0_D;
            rxBuffer[(rxBufferHead + rxBufferLevel) % rxBufferLength] = byteReceived;	// store received byte in rxBuffer
            rxBufferLevel++;												// increment amount of bytes in rxBuffer
            if ('\n' == byteReceived)										// if byte received is a lineFeed...
//            if ('*' == byteReceived)										// if byte received is a *...
                {
                rxBufferLines++;											// ... also increase number of 'lines' received
                }
            }
        }
    else
        {
        // rxBuffer full -> log overflow error
        }
#endif
    }

void HostInterfaceUart::txReady()
    {
#ifndef WIN32
    if (txBufferLevel > 0)													// more bytes still to be transmitted in the txBuffer ?
        {
        UART0_D = txBuffer[txBufferHead];									// write the next byte to the UART Tx Data register
        txBufferHead = (txBufferHead + 1) % txBufferLength;					// increment the bufferHead pointer
        txBufferLevel--;													// decrement the bufferLevel
        }
    else
        {
        UART0_C2 &= ~UART_C2_TIE;											// Disable transmitter interrupts
        }
#endif
    }

void HostInterfaceUart::sendMessage(const char* sourceBuffer)
    {
#ifndef WIN32
    HostInterface::sendMessage(sourceBuffer);
    UART0_C2 |= UART_C2_TIE;											// Enable transmitter interrupts
#endif
    }

void HostInterfaceUart::getMessage(char* destinationBuffer)
    {
    HostInterface::getMessage(destinationBuffer);
    }

bool HostInterfaceUart::hasMessage()
    {
    return 	HostInterface::hasMessage();
    };


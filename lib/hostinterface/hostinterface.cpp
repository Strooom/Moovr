// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "hostinterface.h"

// --------------------------------------------------------------------------------------------------------------------
// Base class for a hostInterface. actual HW implementation could be UART, USBSerial, USBRAW, Ethernet, CAN, whatever..
// --------------------------------------------------------------------------------------------------------------------

hostInterface::hostInterface(){};

void hostInterface::getMessage(uint8_t* destinationBuffer) {
    uint32_t byteReceived;
    uint32_t bytesRead = 0;
    while (rxBufferLevel > 0) {
        byteReceived = rxBuffer[rxBufferHead];                     //	read a byte from the rxBuffer
        rxBufferHead = (rxBufferHead + 1) % rxBufferLength;        //  increment the bufferHead pointer
        rxBufferLevel--;
        destinationBuffer[bytesRead] = byteReceived;        //  copy byte to destination buffer
        bytesRead++;

        if ('\n' == byteReceived) {
            rxBufferLines--;                            // we read a linefeed, so decrease the number of lines
            destinationBuffer[bytesRead] = 0x00;        // terminate string copied to destination
            return;
        }
    }

    destinationBuffer[bytesRead] = 0x00;        // anyway... terminate string copied to destination
};

void hostInterface::sendMessage(const uint8_t* sourceBuffer) {
    // If enough room in txBffer..
    // ..Copy message from the sourceBuffer to the txBuffer
    // Adjust buffer-properties

    uint32_t length;
    for (length = 0; 0 != sourceBuffer[length]; ++length)
        ;                                                   // determine length of string to be sent in sourceBuffer
    if (length <= (txBufferLength - txBufferLevel))        // If there's enough free space in txBuffer for this String
    {
        for (uint32_t index = 0; index < length; index++)        // Copy it - EXcluding terminating 0x00
        {
            txBuffer[(txBufferHead + txBufferLevel) % txBufferLength] = sourceBuffer[index];
            txBufferLevel++;
        }
    } else {

    }
};

void hostInterface::initialize() {
    rxBufferHead  = 0;
    rxBufferLevel = 0;
    rxBufferLines = 0;
    txBufferHead  = 0;
    txBufferLevel = 0;
}

bool hostInterface::hasMessage() const {
    return (rxBufferLines > 0);
};


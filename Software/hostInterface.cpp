// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "hostInterface.h"
#include "logging.h"

extern uLog theLog;

// --------------------------------------------------------------------------------------------------------------------
// Base class for a HostInterface. actual HW implementation could be UART, USBSerial, USBRAW, Ethernet, CAN, whatever..
// --------------------------------------------------------------------------------------------------------------------

HostInterface::HostInterface(){};

void HostInterface::getMessage(uint8_t* destinationBuffer) {
    uint32_t byteReceived;
    uint32_t bytesRead = 0;
    cli();        // begin critical section
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
    sei();        // end critical section
    theLog.output(loggingLevel::Error, "hostInterface::readWhenNoMessage");
    destinationBuffer[bytesRead] = 0x00;        // anyway... terminate string copied to destination
};

void HostInterface::sendMessage(const uint8_t* sourceBuffer) {
    // If enough room in txBffer..
    // ..Copy message from the sourceBuffer to the txBuffer
    // Adjust buffer-properties

    uint32_t length;
    for (length = 0; 0 != sourceBuffer[length]; ++length)
        ;                                                  // determine length of string to be sent in sourceBuffer
    cli();                                                 // begin critical section
    if (length <= (txBufferLength - txBufferLevel))        // If there's enough free space in txBuffer for this String
    {
        for (uint32_t index = 0; index < length; index++)        // Copy it - EXcluding terminating 0x00
        {
            txBuffer[(txBufferHead + txBufferLevel) % txBufferLength] = sourceBuffer[index];
            txBufferLevel++;
        }
    } else {
        theLog.output(loggingLevel::Critical, "hostInterface::txBufferOverflow");
    }
    sei();        // end critical section
};

void HostInterface::initialize() {
    rxBufferHead  = 0;
    rxBufferLevel = 0;
    rxBufferLines = 0;
    txBufferHead  = 0;
    txBufferLevel = 0;
}

bool HostInterface::hasMessage() const {
    return (rxBufferLines > 0);
};

// ------------------------------------------------------------------------------
// Derived class, implementing the HostInterface over UART0 of the Teensy 3.5/3.6
// ------------------------------------------------------------------------------

HostInterfaceUart::HostInterfaceUart() {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    // Initialize UART0 Hardware
    SIM_SCGC4 |= (0x01 << 10);        // Enable the UART clock to power on UART0

    // Enable the GPIO port on which rx and tx will be connected
    SIM_SCGC5 |= (0x01 << 9);              // Enable the PORTA clock, as the UART pins will appear on this port PA14, PA15
    PORTA_PCR14 |= PORT_PCR_MUX(3);        // Activate UART ALT3 function for this pin : UART0_TX
    PORTA_PCR15 |= PORT_PCR_MUX(3);        // Activate UART ALT3 function for this pin : UART0_RX

    UART0_C1 = UART_C1_ILT;
    UART0_C2 = 0;        // Disable receiver and transmitter and related interrupts - before setting BAUDrate
#endif

#if defined(__MK64FX512__)        // Teensy 3.5
    // Teensy 3.5 @ 120 MHz
    // Baud rate is defined as a fraction of system clock, and so it needs different parameters for Teensy 3.5 vs 3.6
    // 38400 = 120M / (16 * 195,..)
    UART0_BDH = 0;        // Write BDH first, then write BDL !
    UART0_BDL = 195;
    UART0_C4  = UART_C4_BRFA(10);
#endif

#if defined(__MK66FX1M0__)        // Teensy 3.6
    // Teensy 3.6 @ 180 MHz
    // Baud rate is defined as a fraction of system clock, and so it needs different parameters for Teensy 3.5 vs 3.6
    UART0_BDH = 1;        // Write BDH first, then write BDL !
    UART0_BDL = 36;
    UART0_C4  = UART_C4_BRFA(31);
#endif

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    UART0_C3    = 0;
    UART0_C5    = 0;
    UART0_PFIFO = 0;
    UART0_C2    = (UART_C2_TE | UART_C2_RE | UART_C2_RIE);        // Enable receiver and transmitter, and receiver interrupts. Transmitter interrupt will be enabled as soon as there is something in the TxBuffer
    NVIC_ENABLE_IRQ(IRQ_UART0_STATUS);
#endif
    theLog.output(loggingLevel::Debug, "hostInterface initialized");
}

void HostInterfaceUart::rxReady() {
    uint8_t byteReceived;

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    byteReceived = UART0_D;
#endif

    if (rxBufferLevel < (rxBufferLength))        // if buffer NOT full...
    {
        {
            rxBuffer[(rxBufferHead + rxBufferLevel) % rxBufferLength] = byteReceived;        // store received byte in rxBuffer
            rxBufferLevel++;                                                                 // increment amount of bytes in rxBuffer
            if ('\n' == byteReceived)                                                        // if byte received is a lineFeed...
            {
                rxBufferLines++;        // ... also increase number of 'lines' received
            }
        }
    } else {
        theLog.output(loggingLevel::Critical, "hostInterface::rxBufferOverflow");
    }
}

void HostInterfaceUart::txReady() {
    if (txBufferLevel > 0)        // more bytes still to be transmitted in the txBuffer ?
    {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
        UART0_D = txBuffer[txBufferHead];                   // write the next byte to the UART Tx Data register
#endif
        txBufferHead = (txBufferHead + 1) % txBufferLength;        // increment the bufferHead pointer
        txBufferLevel--;                                           // decrement the bufferLevel
    } else {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
        UART0_C2 &= ~UART_C2_TIE;                           // Disable transmitter interrupts
#endif
    }
}

void HostInterfaceUart::sendMessage(const uint8_t* sourceBuffer) {
    HostInterface::sendMessage(sourceBuffer);
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    UART0_C2 |= UART_C2_TIE;                                // Enable transmitter interrupts
#endif
}

void HostInterfaceUart::getMessage(uint8_t* destinationBuffer) {
    HostInterface::getMessage(destinationBuffer);
}

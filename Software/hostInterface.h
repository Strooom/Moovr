#pragma once

// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif

// --------------------------------------------------------------------------------------------------------------------
// Base class for a HostInterface. actual HW implementation could be UART, USBSerial, USBRAW, Ethernet, CAN, whatever..
// --------------------------------------------------------------------------------------------------------------------

class HostInterface
    {
    public:
        HostInterface();											// constructor

		void initialize();
		bool hasMessage() const;									// checks for a valid message received
        void getMessage(uint8_t *);									// read the oldest msg received from the host
        void sendMessage(const uint8_t *);							// write a message to the host

        virtual void txReady() = 0;									// transmit-data-register empty -> 'pop' the next byte from head of txBuffer to transmit. This EMPTIES the txBuffer
        virtual void rxReady() = 0;									// member function to be called from interrupt handler when receiving a byte. 'Push'ing the byte received at the end of the rxBuffer. FILLS the rxBuffer

		static constexpr uint32_t rxBufferLength = 256;				// According to GCode standards, maximum length of a GCode block is 256 chars
		static constexpr uint32_t txBufferLength = 256;				// what is the length of the largest msg you want to report back..

    protected:
        uint8_t rxBuffer[rxBufferLength]{};                         // Receive buffer
        uint32_t rxBufferHead = 0;									// Position of oldest byte received
        volatile uint32_t rxBufferLevel = 0;						// Number of bytes in the receive buffer
        volatile uint32_t rxBufferLines = 0;						// Number of 'lines' in the receive buffer - a line is terminted with a linefeed = \n = 0x0A

        uint8_t txBuffer[txBufferLength]{};                         // Transmit buffer
        uint32_t txBufferHead = 0;									// Position of next byte to be transmitted
        volatile uint32_t txBufferLevel = 0;						// Number of bytes in the transmit buffer
    };


// ------------------------------------------------------------------------------
// Derived class, implementing the HostInterface over UART0 of the Teensy 3.5/3.6
// ------------------------------------------------------------------------------

class HostInterfaceUart : public HostInterface
    {
    public :
        HostInterfaceUart();									// Constructor, initializes the UART

        void sendMessage(const uint8_t *);
        void getMessage(uint8_t *);

        void rxReady() override;
        void txReady() override;
    };

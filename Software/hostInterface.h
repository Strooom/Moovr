// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#ifndef HostInterface_h
#define HostInterface_h

#ifndef WIN32
#include <Arduino.h>
#else
#include <inttypes.h>
#endif

enum class commsError : uint8_t
    {
    NONE = 0,
    RXOVERFLOW,
    TXOVERFLOW,
    RXUNDERFLOW,
    RXEOLNOTFOUND,
    RXINVALIDCHECKSUM,
    RXINVALIDSEQNMBR,
    nmbrErrors
    };

// This is the base class for a HostInterface. actual HW implementation could be UART, USBSerial, USBRAW, Ethernet, CAN, whatever..

class HostInterface
    {
    public:
        HostInterface();										// constructor

        enum class ReportItems : uint8_t
            {
            rxLevel = 1 << 0,
            msgLevel = 1 << 1,
            test = 1 << 2
            };

		bool hasMessage(); //const;								// checks for a valid message received
        void getMessage(char *);								// read the oldest msg received from the host
        void sendMessage(const char *);							// write a message to the host
        void getStatus(char *, uint8_t statusTypesFlags);		// get the status of the hostInterface as a string

        virtual void txReady() = 0;								// transmit-data-register empty -> 'pop' the next byte from head of txBuffer to transmit. This EMPTIES the txBuffer
        virtual void rxReady() = 0;								// member function to be called from interrupt handler when receiving a byte. 'Push'ing the byte received at the end of the rxBuffer. FILLS the rxBuffer

    protected:
        static constexpr uint32_t rxBufferLength = 512;			// According to GCode standards, maximum length of a GCode block is 256 chars
        uint8_t rxBuffer[rxBufferLength];						// Receive buffer
        uint32_t rxBufferHead = 0;								// Position of oldest byte received
        volatile uint32_t rxBufferLevel = 0;					// Number of bytes in the receive buffer
        volatile uint32_t rxBufferLines = 0;					// Number of 'lines' in the receive buffer - a line is terminted with a linefeed = \n = 0x0A

        static constexpr uint32_t txBufferLength = 512;			// what is the length of the largest msg you want to report back..
        uint8_t txBuffer[txBufferLength];						// Transmit buffer
        uint32_t txBufferHead = 0;								// Position of next byte to be transmitted
        volatile uint32_t txBufferLevel = 0;					// Number of bytes in the transmit buffer
    };


// Derived class, implementing the HostInterface over UART0 of the Teensy 3.5/3.6

class HostInterfaceUart : public HostInterface
    {
    public :
        HostInterfaceUart();		// Constructor, initializes the UART

        bool hasMessage();
        void sendMessage(const char *);
        void getMessage(char *);

        void rxReady();
        void txReady();

    private:
    };


#endif


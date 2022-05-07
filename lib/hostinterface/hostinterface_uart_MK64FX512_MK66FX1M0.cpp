#include "hostinterface_uart.h"

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6

hostInterfaceUart::hostInterfaceUart() {
    // Initialize UART0 Hardware
    SIM_SCGC4 |= (0x01 << 10);        // Enable the UART clock to power on UART0

    // Enable the GPIO port on which rx and tx will be connected
    SIM_SCGC5 |= (0x01 << 9);              // Enable the PORTA clock, as the UART pins will appear on this port PA14, PA15
    PORTA_PCR14 |= PORT_PCR_MUX(3);        // Activate UART ALT3 function for this pin : UART0_TX
    PORTA_PCR15 |= PORT_PCR_MUX(3);        // Activate UART ALT3 function for this pin : UART0_RX

    UART0_C1 = UART_C1_ILT;
    UART0_C2 = 0;        // Disable receiver and transmitter and related interrupts - before setting BAUDrate

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

    UART0_C3    = 0;
    UART0_C5    = 0;
    UART0_PFIFO = 0;
    UART0_C2    = (UART_C2_TE | UART_C2_RE | UART_C2_RIE);        // Enable receiver and transmitter, and receiver interrupts. Transmitter interrupt will be enabled as soon as there is something in the TxBuffer
    NVIC_ENABLE_IRQ(IRQ_UART0_STATUS);
}

void hostInterfaceUart::rxReady() {
    uint8_t byteReceived;
    byteReceived = UART0_D;

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

    }
}

void hostInterfaceUart::txReady() {
    if (txBufferLevel > 0)        // more bytes still to be transmitted in the txBuffer ?
    {
        UART0_D = txBuffer[txBufferHead];                   // write the next byte to the UART Tx Data register
        txBufferHead = (txBufferHead + 1) % txBufferLength;        // increment the bufferHead pointer
        txBufferLevel--;                                           // decrement the bufferLevel
    } else {
        UART0_C2 &= ~UART_C2_TIE;                           // Disable transmitter interrupts
    }
}

void hostInterfaceUart::sendMessage(const uint8_t* sourceBuffer) {
    hostInterface::sendMessage(sourceBuffer);
    UART0_C2 |= UART_C2_TIE;                                // Enable transmitter interrupts
}

void hostInterfaceUart::getMessage(uint8_t* destinationBuffer) {
    hostInterface::getMessage(destinationBuffer);
}

// -------------------------
// --- interrupt handler ---
// -------------------------

extern hostInterfaceUart theHostInterface;

void uart0_status_isr(void)
    {
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
    }


#endif

// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "switches.h"
#include "Arduino.h"

// -------------------------------
// raw GPIO - hardware abstraction
// -------------------------------

void inputs::initialize() {
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
    PORTD_PCR6 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    PORTD_PCR7 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    GPIOD_PDDR &= 0xFFFFFF00;
    PORTE_PCR24 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    PORTE_PCR25 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    GPIOE_PDDR &= 0x0C000000;
    PORTA_PCR16 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    PORTA_PCR17 = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
    GPIOA_PDDR &= 0x000C0000;
}

bool inputs::get(uint32_t index) {
    if (index >= nmbrInputs) {
        return false;
    } else {
        return (static_cast<bool>(theInputs & (0x1 << index)));
    }
}

void inputs::sample() {
    theInputs = (GPIOD_PDIR & 0x00'00'00'FF);                  // Bits 0..7 from portD
    theInputs |= ((GPIOE_PDIR & 0x01'00'00'00) >> 15U);        // Bit 24 from portE Note : I accidentaly swapped PE24 and PE25 on the PCB due to an error in a non official pinout excel.. :-(
    theInputs |= ((GPIOE_PDIR & 0x02'00'00'00) >> 17U);        // Bit 25 from portE
    theInputs |= ((GPIOA_PDIR & 0x00'03'00'00) >> 6U);         // Bits 16..17 from portA
    theInputs = theInputs ^ inputMask;                         // takes care of inversions of certain inputs if needed
}


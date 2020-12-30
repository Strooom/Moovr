// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "digitalInputs.h"
#include "eventbuffer.h"
#include "logging.h"

extern uLog theLog;

// -------------------------------
// raw GPIO - hardware abstraction
// -------------------------------

inputs::inputs() {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6

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

#endif
    theLog.output(loggingLevel::Debug, "digitalInputs initialized");
}

bool inputs::get(uint32_t index) {
    ready = false;
    if (index > 31) {
        return false;
    } else {
        return (static_cast<bool>(theInputs & (0x1 << index)));
    }
}

void inputs::sample() {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)          // Teensy 3.5 || Teensy 3.6
    theInputs = (GPIOD_PDIR & 0x00'00'00'FF);                 // Bits 0..7 from portD
    theInputs |= ((GPIOE_PDIR & 0x01'00'00'00) >> 15);        // Bit 24 from portE Note : I accidentaly swapped PE24 and PE25 on the PCB due to an error in a non official pinout excel.. :-(
    theInputs |= ((GPIOE_PDIR & 0x02'00'00'00) >> 17);        // Bit 25 from portE
    theInputs |= ((GPIOA_PDIR & 0x00'03'00'00) >> 6);         // Bits 16..17 from portA
    theInputs = theInputs ^ inputMask;                        // takes care of inversions of certain inputs if needed
    ready     = true;                                         // indicates we have new samples, ready to be debounced etc..
#endif
}

bool inputs::isReady() const {
    return ready;
}

// ---------------------------------------------------
// debounced input - mapped to IO and generates events
// ---------------------------------------------------

debouncedInput::debouncedInput(inputs &someInputs, const uint32_t anIndex, const event anEventOnOpen, const event anEventOnClose) : theInputs(someInputs), index(anIndex), onOpen(anEventOnOpen), onClose(anEventOnClose) {
    // TODO : check if pinNmbr is in valid range
    // TODO : some better naming of the ctor parameters..
    // TODO : further initialize members to avoid transient events at startup...
    theLog.snprintf(loggingLevel::Debug, "debouncedInput %d initialized", index);
}

bool debouncedInput::getState() const {
    return currentState;
}

event debouncedInput::getEvent() {
    if (theInputs.get(index)) {
        if (debounceCounter < debounceMaxCount) {
            ++debounceCounter;
        } else {
            currentState = true;
            if (!previousState) {
                previousState = true;
                return onClose;
            }
        }
    } else {
        if (debounceCounter > 0) {
            --debounceCounter;
        } else {
            currentState = false;
            if (previousState) {
                previousState = false;
                return onOpen;
            }
        }
    }
    return event::none;
}

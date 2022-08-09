// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "Arduino.h"
#include "steppermotor.h"

void stepperMotorOutputs::initialize() {
    // Hardware Pins for Moovr Motherboard
    // OUTPUTS
    // Step and Dir : PC[0..11]
    // Enable for Motors [1..3] : PB18
    // Enable for Motors [4..6] : PB19
    // Peripherals [1..2] : PB[0..1]
    // Peripherals [3..4] : PA[12..13]
    // Peripherals [5] : PA[5]
    // Peripherals [6] : PE[26]

    // Setup the PortC which will output Step and Dir signals
    PORTC_PCR0  = PORT_PCR_MUX(1) | PORT_PCR_DSE;        // Set PortC0 into Alt 1, High Drive Strength
    PORTC_PCR1  = PORT_PCR_MUX(1) | PORT_PCR_DSE;        // Set PortC1 into Alt 1, High Drive Strength
    PORTC_PCR2  = PORT_PCR_MUX(1) | PORT_PCR_DSE;        // Set PortC2 into Alt 1, High Drive Strength
    PORTC_PCR3  = PORT_PCR_MUX(1) | PORT_PCR_DSE;        // Set PortC3 into Alt 1, High Drive Strength
    PORTC_PCR4  = PORT_PCR_MUX(1) | PORT_PCR_DSE;        // Set PortC4 into Alt 1, High Drive Strength
    PORTC_PCR5  = PORT_PCR_MUX(1) | PORT_PCR_DSE;        // Set PortC5 into Alt 1, High Drive Strength
    PORTC_PCR6  = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    PORTC_PCR7  = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    PORTC_PCR8  = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    PORTC_PCR9  = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    PORTC_PCR10 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    PORTC_PCR11 = PORT_PCR_MUX(1) | PORT_PCR_DSE;

    GPIOC_PDDR = 0x00000FFF;        // Set PortC[0..11] as Output

    // StepBuffer Driver Enable signals
    GPIOB_PDDR  = 0x000C0000;                            // Set PortB[18..19] as Output
    PORTB_PCR18 = PORT_PCR_MUX(1) | PORT_PCR_DSE;        // Set PortB[18..19] into Alt 1, High Drive Strength		// Enable for Motors [1..3] : PB18
    PORTB_PCR19 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    enableMotors123(false);
    enableMotors456(false);
}

void stepperMotorOutputs::write(uint32_t value) {
    GPIOC_PDOR = value;
}

void stepperMotorOutputs::enableMotors123(bool onOff) {
    if (onOff) {
        GPIOB_PCOR = (0x00000001 << 18);
    } else {
        GPIOB_PSOR = (0x00000001 << 18);
    }
}

void stepperMotorOutputs::enableMotors456(bool onOff) {
    if (onOff) {
        GPIOB_PCOR = (0x00000001 << 19);
    } else {
        GPIOB_PSOR = (0x00000001 << 19);
    }
}


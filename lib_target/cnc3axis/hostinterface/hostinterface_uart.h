// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <stdint.h>
#include "hostinterface.h"

// ------------------------------------------------------------------------------
// Derived class, implementing the HostInterface over UART0 of the Teensy 3.5/3.6
// ------------------------------------------------------------------------------

class hostInterfaceUart : public hostInterface
    {
    public :
        hostInterfaceUart();

        void sendMessage(const uint8_t *);
        void getMessage(uint8_t *);

        void rxReady() override;
        void txReady() override;
    };

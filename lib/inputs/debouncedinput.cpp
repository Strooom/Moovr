// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "debouncedinput.h"

debouncedInput::debouncedInput(const event eventOnOpen, const event eventOnClose) : onOpen(eventOnOpen), onClose(eventOnClose) {
}

bool debouncedInput::getState() const {
    return currentState;
}

event debouncedInput::getEvent(bool undebouncedInput) {
    if (undebouncedInput) {
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
        if (debounceCounter > 0U) {
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

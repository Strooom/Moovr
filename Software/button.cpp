#include "button.h"

ButtonEvent ButtonSwitch::run(bool pinSignal)
    {
    if (!pinSignal)								// HW is pullup-resistor, pulled to GND when switch closes. So active button reads 0, inactive button reads 1
        {
        if (debounceCounter < debounceMaxCount)
            {
            ++debounceCounter;					// increment as long as pin is high and debounceCounter not yet maxed out
            }
        else
            {
            currentState = true;				// debouncecounter is maxed out -> the button is pressed
            if (!previousState)
                {
                previousState = true;
                return ButtonEvent::closed;
                }
            }
        }
    else
        {
        if (debounceCounter > 0)
            {
            --debounceCounter;					// decrement as long as pin is low and debounceCounter not yet down to zero
            }
        else
            {
            currentState = false;				// debouncecounter completely count down -> the button is released
            if (previousState)
                {
                previousState = false;
                return ButtonEvent::opened;
                }
            }
        }
    return ButtonEvent::none;
    }

bool ButtonSwitch::getState()
    {
    return currentState;
    }


#ifndef button_h
#define button_h

#ifndef WIN32
#include <Arduino.h>
#else
#include <inttypes.h>		// included to get access to uint8_t, uint32_t etc.
#endif


enum class ButtonEvent : uint8_t
    {
    none = 0,
    closed,
	opened,
	nmbrButtonEvents
    };

class ButtonSwitch
    {
    private:
        uint8_t pinNmbr;
        uint8_t debounceCounter = 0;				// counts up (input high) or down (input low) until it hits boundaries 0 or maxCount

        bool currentState = false;					// after debouncing : true : button is pressed, false : button is not pressed
        bool previousState = false;					// remembers previous state, to detect flanks

    public:
        static constexpr uint8_t debounceMaxCount = 8;	// sets the upper boundary for debounceCounter : 8 * 5ms = 40ms
        bool getState();
		ButtonEvent run(bool pinSignal);
    };

#endif



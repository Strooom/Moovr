#pragma once

#include <Arduino.h>

// This class provides hardware abstraction for the hardware timers needed in Moovr :
// 1. one timer is needed to sample all input buttons and switches every xx ms
// 2. one (reload) timer is needed to write the motor signals to the outputs with correct timing
//
// For the Teensy 3.5, 3.6 they can be implemented using the PIT0, PIT1 and PIT2. One extra PIT being used as a prescaler
// For the Teensy 4.1 ...
// For the ESP32 ...
// For the Arduino Portenta ...

// Note : one instance of this class will create BOTH timers


class hardwareTimers {
  public:
    hardwareTimers();

    void enableInputTimer(bool onOff);
    void enableOutputTimer(bool onOff);
    void setOutputTimerReload(uint32_t value);

  private:
};

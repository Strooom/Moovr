#pragma once

#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif

enum class peripheralOutputType : uint8_t {
    digital,
    pwm
    // other typs like uart controlled spindles could go here..
};

class peripheralOutput {
  public:
    explicit peripheralOutput(peripheralOutputType type);
    void set(bool onOff);
    void set(uint8_t value);

  private:
    peripheralOutputType type;
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    static constexpr uint8_t nmbrOutputs{6};                // how many outputs does this particular HW have
#else
    static constexpr uint8_t nmbrOutputs{6};
#endif
};

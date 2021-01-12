#pragma once

#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "logging.h"

class intervalTimer {
  public:
    explicit intervalTimer(unsigned long);        // constructor
    bool expired();                               // check if intervalTimer has expired

  private:
    unsigned long timerValue{0};
    unsigned long timerInterval;
};

class singleTimer {
  public:
    bool expired();                   // check if timer has expired
    void start(unsigned long);        // re-start a new timeout

  private:
    bool running{false};
    unsigned long timerStart;
    unsigned long timerDuration;
};

class counter {
  public:
    explicit counter(uint32_t maxCount);
    uint32_t value() const;
    bool expired() const;        // check if counter has expired
    void increment();
    void reset();

  private:
    uint32_t count;
    uint32_t maxCount;
};
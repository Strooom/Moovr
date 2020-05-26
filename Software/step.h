#pragma once

#ifndef  WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif

struct step
    {
    uint32_t out;						// bit signals to drive the Step / Dir signals. only 12 LSB are really use, driving 6 stepper motors
    uint32_t reload;					// 32-bit value to write in Timer Reload register.
    };
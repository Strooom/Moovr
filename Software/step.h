#pragma once

#include <stdint.h>        // access to uintxx_t data types

struct step {
    uint32_t timeBefore;        // timeBefore from previous step to this step
    uint32_t signals;           // signals to drive the Step & Dir signals
};
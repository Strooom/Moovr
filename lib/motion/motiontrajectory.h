// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "simplifiedmotion.h"

class motionTrajectory {
  public:
    void set(const simplifiedMotion& theMotion);

#ifndef unitTesting
  private:
#endif
    friend class motion;
    friend class motionBuffer;
    friend class motionCtrl;

    float startPosition[nmbrAxis]{};        // [mm]

    // Linear moves
    float delta[nmbrAxis]{};        // [mm]

    // Helical moves
    axis arcAxis0{axis::nmbrAxis};        //
    axis arcAxis1{axis::nmbrAxis};        //
    float arcCenter0{};                   // [mm]
    float arcCenter1{};                   // [mm]
    float startAngle{};                   // [rad]
    float deltaAngle{};                   // [rad]
    float radius{};                       // [mm]

    // Common to all moves
    float length{};                         // [mm]
    float deltaRealTime[nmbrAxis]{};        // [mm]
    float entryVector[nmbrAxis]{};          // [dimensionless]
    float exitVector[nmbrAxis]{};           // [dimensionless]

    uint32_t toString(char* output, const motionType type) const;
};

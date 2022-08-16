// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "gcodeblock.h"
#include "gcodestate.h"
#include "simplifiedmotion.h"
#include "gcodeparseresulttype.h"
#include "gcodeparseerror.h"

class gCode {
  public:
    gCode();        // Constructor

    void reset();        // completely reset the parser, forgetting all state from already parsed blocks

    void saveState();           // Save the state of the Parser, so we can do some interim stuff and afterwards restore the state back to what is was..
    void restoreState();        // Restore the state from previous saveState

    void parseBlock(simplifiedMotion &theMotion);        // Process all words in block, update gCode context and spawn a motion if the block contains one..

#ifndef unitTesting
  private:
#endif
    static constexpr double pi{3.141592653589793238463};        // constant for calculations in radians
    static constexpr double inches2mm{25.4};                    // 25.4 mm/inch

    gCodeBlock theBlock;                   //
    gCodeState theState;                   // gCode parser context : current position, value of all modal groups, etc.
    gCodeParseResultType theResult;        //
    gCodeParseError theError;              //

    // TODO : some of the helpers below may not need the simplifiedMotion as they are only updating the state..
    void getAxisOffsetRadiusWords();
    void getLineNumberWord();
    void setFeedrateMode();
    void setFeedrateValue();
    void setSpindleSpeed();
    void setSpindleMode();
    void setCoolantMode();
    void setOverrideMode();
    void setArcPlane();
    void setUnitsMode();
    void setWCSMode();
    void setdistanceMode();

    void calcMotion(simplifiedMotion &theMotion);        // calculate all the details for a motion resulting from a G0, G1, G2, G3, G4 block..

    void calcNextPosition(simplifiedMotion &theMotion);
    void setArcAxisFromPlane(simplifiedMotion &theMotion);
    void calcArcCenterAndRadius(simplifiedMotion &theMotion);
    void calcArcAngles(simplifiedMotion &theMotion);
    void calcTrajectoryLength(simplifiedMotion &theMotion);
    void calcSpeedAndOrDuration(simplifiedMotion &theMotion);
    void calcPeripherals(simplifiedMotion &theMotion);
    void updatePosition();
};

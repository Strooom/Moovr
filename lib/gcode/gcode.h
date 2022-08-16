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

    void initialize();        // initialize the parser, before parsing a new block
    void reset();             // completely reset the parser, forgetting all state from already parsed blocks

    void saveState();           // Save the state of the Parser, so we can do some interim stuff and afterwards restore the state back to what is was..
    void restoreState();        // Restore the state from previous saveState

    void parseBlock(simplifiedMotion &theParseResult);        // Process all words in block, update gCode context and spawn a motion if the block contains one..
    void calcMotion(simplifiedMotion &theParseResult);        // calculate all the details for a motion resulting from a G0, G1, G2, G3, G4 block..

#ifndef unitTesting
  private:
#endif
    static constexpr double pi{3.141592653589793238463};        // constant for calculations in radians
    static constexpr double inches2mm{25.4};                    // 25.4 mm/inch

    gCodeBlock theBlock;        //
    gCodeState theState;        // gCode parser context : current position, value of all modal groups, etc.
    simplifiedMotion theMotion;
    gCodeParseResultType theResult;
    gCodeParseError theError;
};

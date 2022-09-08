// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <cstdlib>        // required for access to strtod under windows
#include <math.h>
#include "gcode.h"
#include <limits>

gCode::gCode() {
    reset();
};

void gCode::reset() {
    theState.initialize();
    theBlock.initialize();
    theResult = gCodeParseResultType::EmptyBlock;
    theError  = gCodeParseError::None;

    // Test Only Defaults
    theState.WCSorigin[static_cast<uint32_t>(modalGroupCoordinateSet::G54)][static_cast<uint32_t>(axis::X)] = 0;
    theState.WCSorigin[static_cast<uint32_t>(modalGroupCoordinateSet::G54)][static_cast<uint32_t>(axis::Y)] = 0;
    theState.WCSorigin[static_cast<uint32_t>(modalGroupCoordinateSet::G54)][static_cast<uint32_t>(axis::Z)] = 0;
}

void gCode::interpreteBlock(simplifiedMotion &theMotion) {
    theResult      = gCodeParseResultType::EmptyBlock;
    theMotion.type = motionType::none;
    theError       = gCodeParseError::None;

    if (theBlock.hasModalGroupViolation()) {
        theResult = gCodeParseResultType::Error;
        theError  = gCodeParseError::ModalGroupCollision;
        return;
    }

    if (theBlock.hasLetterWordViolation()) {
        theResult = gCodeParseResultType::Error;
        theError  = gCodeParseError::ValueWordCollision;
        return;
    }

    getAxisOffsetRadiusWords();
    getLineNumberWord();
    setFeedrateMode();
    setFeedrateValue();
    setSpindleSpeed();
    setSpindleMode();
    setCoolantMode();
    setOverrideMode();

    if (theBlock.hasWords()) {
        int32_t i;
        //	dwell (G4)
        i = theBlock.searchWord('G', 40U);
        if (i >= 0) {
            int32_t j;
            j = theBlock.searchWord('P');
            if (j >= 0) {
                if (theBlock.gCodeWords[j].number >= 0) {
                    theState.letterValueState[static_cast<uint32_t>(gCodeLetter::P)] = theBlock.gCodeWords[j].number;
                    theResult                                                        = gCodeParseResultType::OkContextUpdateAndMotion;
                    theMotion.type                                                   = motionType::pauseAndResume;
                    calcMotion(theMotion);
                    theBlock.removeWord(i);
                    theBlock.removeWord(j);
                    return;
                } else {
                    theResult = gCodeParseResultType::Error;
                    theError  = gCodeParseError::InvalidDwellTime;
                }
            } else {
                theResult = gCodeParseResultType::Error;
                theError  = gCodeParseError::MissingPForG4;
            }
        }
    }

    setArcPlane();
    setUnitsMode();
    setWCSMode();
    setdistanceMode();

    if (theBlock.hasWords()) {
        int32_t i;
        //	Change coordinate system data (G10)
        if ((i = theBlock.searchWord('G', 100)) >= 0) {
            int32_t j;
            if ((j = theBlock.searchWord('L', 20)) >= 0) {
                int32_t k;
                if ((k = theBlock.searchWord('P')) >= 0) {
                    uint32_t WCSindex = theBlock.gCodeWords[k].intNumber / 10;
                    if ((WCSindex >= 0) && (WCSindex <= 9)) {

                    } else {
                    theResult = gCodeParseResultType::Error;
                    theError  = gCodeParseError::InvalidPForG10;
                    }
                } else {
                    theResult = gCodeParseResultType::Error;
                    theError  = gCodeParseError::MissingPForG10;
                }
            } else if ((j = theBlock.searchWord('L', 200)) >= 0) {
                int32_t k;
                if ((k = theBlock.searchWord('P')) >= 0) {
                    uint32_t WCSindex = theBlock.gCodeWords[k].intNumber / 10;
                    if ((WCSindex >= 0) && (WCSindex <= 9)) {

                    } else {
                    theResult = gCodeParseResultType::Error;
                    theError  = gCodeParseError::InvalidPForG10;
                    }
                } else {
                    theResult = gCodeParseResultType::Error;
                    theError  = gCodeParseError::MissingPForG10;
                }
            } else {
                theResult = gCodeParseResultType::Error;
                theError  = gCodeParseError::MissingLForG10;
            }
        }
    }
    {
        int32_t i;
        if ((i = theBlock.searchWord('G', 530)) >= 0) {
            // NonModal G53 move
            uint8_t currentCoordinateSet                                                = theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)];        // remember which WCS is active
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = static_cast<uint32_t>(modalGroupCoordinateSet::G53);                                // just for this move, use machine coordinates
            int32_t j;
            if ((j = theBlock.searchWord('G', 0)) >= 0) {
                // non-modal move at traverse rate
                theMotion.type = motionType::traverse;
                calcMotion(theMotion);
                theBlock.removeWord(j);
            } else if ((j = theBlock.searchWord('G', 10)) >= 0) {
                // non-modal move at feed rate
                theMotion.type = motionType::feedLinear;
                // check if feedrate > 0
                calcMotion(theMotion);

                theBlock.removeWord(j);
            } else {
                // non-modal move at current G0/G1 rate - determine if G0 or G1 and add another one
                switch ((modalGroupMotion)modalGroup::Motion) {
                    case modalGroupMotion::G0:
                        theMotion.type = motionType::traverse;
                        calcMotion(theMotion);
                        theBlock.removeWord(j);
                        break;

                    case modalGroupMotion::G1:
                        theMotion.type = motionType::feedLinear;
                        // check if feedrate > 0
                        calcMotion(theMotion);
                        theBlock.removeWord(j);
                        break;

                    default:
                        // Error : G53 without G0 or G1 and modalGroup::Motion = G2 or G3..
                        break;
                }
            }
            theBlock.removeWord(i);
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = currentCoordinateSet;        // go back to whatever WCS was active

        } else {
            // No G53, so normal modal moves
            if ((i = theBlock.searchWord('G', 0)) >= 0) {
                theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Motion)] = static_cast<uint32_t>(modalGroupMotion::G0);

                theResult      = gCodeParseResultType::OkContextUpdateAndMotion;
                theError       = gCodeParseError::None;
                theMotion.type = motionType::traverse;
                calcMotion(theMotion);
                theBlock.removeWord(i);
                return;
            } else if ((i = theBlock.searchWord('G', 10)) >= 0) {
                theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Motion)] = static_cast<uint32_t>(modalGroupMotion::G1);
                theMotion.type                                                       = motionType::feedLinear;
                if (theState.letterValueState[static_cast<uint32_t>(gCodeLetter::F)] > 0) {
                    calcMotion(theMotion);
                    theBlock.removeWord(i);
                    if (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::FeedRate)] == static_cast<uint32_t>(modalGroupFeedRate::G93)) {
                        theState.letterValueState[static_cast<uint32_t>(gCodeLetter::F)] = 0;
                    }
                    theError  = gCodeParseError::None;
                    theResult = gCodeParseResultType::OkContextUpdateAndMotion;
                    return;
                } else {
                    theResult          = gCodeParseResultType::Error;
                    theError           = gCodeParseError::InvalidFeedrate;
                    theBlock.nmbrWords = 0;
                    return;
                }
            } else if ((i = theBlock.searchWord('G', 20)) >= 0) {
                theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Motion)] = static_cast<uint32_t>(modalGroupMotion::G2);
                theMotion.type                                                       = motionType::feedHelicalCW;
                // check if feedrate > 0
                calcMotion(theMotion);
                theBlock.removeWord(i);
                theError  = gCodeParseError::None;
                theResult = gCodeParseResultType::OkContextUpdateAndMotion;
                return;
            } else if ((i = theBlock.searchWord('G', 30)) >= 0) {
                theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Motion)] = static_cast<uint32_t>(modalGroupMotion::G3);
                theMotion.type                                                       = motionType::feedHelicalCCW;
                // check if feedrate > 0
                calcMotion(theMotion);
                theBlock.removeWord(i);
                theError  = gCodeParseError::None;
                theResult = gCodeParseResultType::OkContextUpdateAndMotion;
                return;
            } else if (theBlock.hasAxis()) {        // no G-code, but axis words -> continue the current mode
                                                    // TODO : do I need additional checks ?
                switch (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Motion)]) {
                    case static_cast<uint32_t>(modalGroupMotion::G0):
                        theMotion.type = motionType::traverse;
                        break;
                    case static_cast<uint32_t>(modalGroupMotion::G1):
                        theMotion.type = motionType::feedLinear;
                        break;
                    case static_cast<uint32_t>(modalGroupMotion::G2):
                        theMotion.type = motionType::feedHelicalCW;
                        break;
                    case static_cast<uint32_t>(modalGroupMotion::G3):
                        theMotion.type = motionType::feedHelicalCCW;
                        break;
                    default:
                        // TODO : Error ?
                        break;
                }

                // check if feedrate > 0
                calcMotion(theMotion);
                theError  = gCodeParseError::None;
                theResult = gCodeParseResultType::OkContextUpdateAndMotion;
                return;
            }
        }
    }

    if (theBlock.hasWords()) {
        int32_t i;

        //	stop (M0, M1, M2, M30, M60)
        if ((i = theBlock.searchWord('M', 0)) >= 0) {
            // pause until resume
        } else if ((i = theBlock.searchWord('M', 10)) >= 0) {
            // pause until resume
        } else if ((i = theBlock.searchWord('M', 20)) >= 0) {
            // program ends.. ignore all subsequent lines of gCode - reset parser to default state
        } else if ((i = theBlock.searchWord('M', 300)) >= 0) {
            // program ends.. ignore all subsequent lines of gCode - reset parser to default state
        } else if ((i = theBlock.searchWord('M', 600)) >= 0) {
            // program ends.. ignore all subsequent lines of gCode - reset parser to default state
        }
    }
    // Now check for any unused words - normally they should be unsupported
    if (theBlock.hasWords()) {
        theResult          = gCodeParseResultType::Error;
        theError           = gCodeParseError::UnsupportedgCodeWord;
        theBlock.nmbrWords = 0;
    }

    theError  = gCodeParseError::None;
    theResult = gCodeParseResultType::OkContextUpdateOnly;
    return;
};

void gCode::setdistanceMode() {
    if (theBlock.hasWords()) {
        //	set distance mode (G90, G91)
        int32_t i;
        if ((i = theBlock.searchWord('G', 900U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Distance)] = static_cast<uint32_t>(modalGroupDistance::G90);
            theBlock.removeWord(i);
        } else if ((i = theBlock.searchWord('G', 910U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Distance)] = static_cast<uint32_t>(modalGroupDistance::G91);
            theBlock.removeWord(i);
        }
    }
}

void gCode::setWCSMode() {
    if (theBlock.hasWords()) {
        int32_t i;
        //	Coordinate system selection (G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3)
        if ((i = theBlock.searchWord('G', 540U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = static_cast<uint32_t>(modalGroupCoordinateSet::G54);
            theBlock.removeWord(i);
        } else if ((i = theBlock.searchWord('G', 550U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = static_cast<uint32_t>(modalGroupCoordinateSet::G55);
            theBlock.removeWord(i);

        } else if ((i = theBlock.searchWord('G', 560U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = static_cast<uint32_t>(modalGroupCoordinateSet::G56);
            theBlock.removeWord(i);

        } else if ((i = theBlock.searchWord('G', 570U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = static_cast<uint32_t>(modalGroupCoordinateSet::G57);
            theBlock.removeWord(i);

        } else if ((i = theBlock.searchWord('G', 580U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = static_cast<uint32_t>(modalGroupCoordinateSet::G58);
            theBlock.removeWord(i);

        } else if ((i = theBlock.searchWord('G', 590U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = static_cast<uint32_t>(modalGroupCoordinateSet::G59);
            theBlock.removeWord(i);

        } else if ((i = theBlock.searchWord('G', 591U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = static_cast<uint32_t>(modalGroupCoordinateSet::G59_1);
            theBlock.removeWord(i);

        } else if ((i = theBlock.searchWord('G', 592U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = static_cast<uint32_t>(modalGroupCoordinateSet::G59_2);
            theBlock.removeWord(i);
        } else if ((i = theBlock.searchWord('G', 593U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)] = static_cast<uint32_t>(modalGroupCoordinateSet::G59_3);
            theBlock.removeWord(i);
        }
    }
}

void gCode::setUnitsMode() {
    if (theBlock.hasWords()) {
        int32_t i;
        //	Set length units (G20, G21)
        if ((i = theBlock.searchWord('G', 200U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Unit)] = static_cast<uint32_t>(modalGroupUnit::G20);
            theBlock.removeWord(i);
        } else if ((i = theBlock.searchWord('G', 210U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Unit)] = static_cast<uint32_t>(modalGroupUnit::G21);
            theBlock.removeWord(i);
        }
    }
}

void gCode::setArcPlane() {
    if (theBlock.hasWords()) {
        int32_t i;
        // Set active plane (G17, G18, G19)
        if ((i = theBlock.searchWord('G', 170U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Plane)] = static_cast<uint32_t>(modalGroupPlane::G17);
            theBlock.removeWord(i);
        } else if (((i = theBlock.searchWord('G', 180U)) >= 0) && (static_cast<uint32_t>(axis::nmbrAxis) >= 3))        // G18 G19 only make sense on a 3D system so axis::nmbrAxis >= 3
        {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Plane)] = static_cast<uint32_t>(modalGroupPlane::G18);
            theBlock.removeWord(i);
        } else if (((i = theBlock.searchWord('G', 190U)) >= 0) && (static_cast<uint32_t>(axis::nmbrAxis) >= 3)) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Plane)] = static_cast<uint32_t>(modalGroupPlane::G19);
            theBlock.removeWord(i);
        }
    }
}

void gCode::setOverrideMode() {
    if (theBlock.hasWords()) {
        int32_t i;
        //	enable or disable overrides (M48, M49)
        if ((i = theBlock.searchWord('M', 480U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::ManualOverride)] = static_cast<uint32_t>(modalGroupOverrides::M48);
            theBlock.removeWord(i);
        } else if ((i = theBlock.searchWord('M', 490U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::ManualOverride)] = static_cast<uint32_t>(modalGroupOverrides::M49);
            theBlock.removeWord(i);
        }
    }
}

void gCode::setCoolantMode() {
    if (theBlock.hasWords()) {
        int32_t i;
        //	Coolant on or off (M7, M8, M9)
        if ((i = theBlock.searchWord('M', 70U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoolantFlood)] = static_cast<uint32_t>(modalGroupCoolantFlood::M7);
            theBlock.removeWord(i);
        }
        if ((i = theBlock.searchWord('M', 80U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoolantMist)] = static_cast<uint32_t>(modalGroupCoolantMist::M8);
            theBlock.removeWord(i);
        }
        if ((i = theBlock.searchWord('M', 90U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoolantFlood)] = static_cast<uint32_t>(modalGroupCoolantFlood::M9);
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoolantMist)]  = static_cast<uint32_t>(modalGroupCoolantMist::M9);
            theBlock.removeWord(i);
        }
    }
}

void gCode::setSpindleMode() {
    if (theBlock.hasWords()) {
        int32_t i;
        //	spindle on or off (M3, M4, M5)
        if ((i = theBlock.searchWord('M', 30U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Spindle)] = static_cast<uint32_t>(modalGroupSpindle::M3);
            theBlock.removeWord(i);
        } else if ((i = theBlock.searchWord('M', 40U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Spindle)] = static_cast<uint32_t>(modalGroupSpindle::M4);
            theBlock.removeWord(i);
        } else if ((i = theBlock.searchWord('M', 50U)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Spindle)] = static_cast<uint32_t>(modalGroupSpindle::M5);
            theBlock.removeWord(i);
        }
    }
}

void gCode::setSpindleSpeed() {
    if (theBlock.hasWords()) {
        int32_t i = theBlock.searchWord('S');
        if (i >= 0) {
            if (theBlock.gCodeWords[i].number >= 0) {
                theState.letterValueState[static_cast<uint32_t>(gCodeLetter::S)] = theBlock.gCodeWords[i].number;
                theBlock.removeWord(i);
            } else {
                theResult = gCodeParseResultType::Error;
                theError  = gCodeParseError::InvalidSpindleSpeed;
                return;
            }
        }
    }
}

void gCode::setFeedrateValue() {
    if (theBlock.hasWords()) {
        int32_t i;
        //	Set feed rate (F) value
        i = theBlock.searchWord('F');
        if (i >= 0) {
            if (theBlock.gCodeWords[i].number > 0)        // Feedrate <= 0 is not allowed / makes no sense
            {
                theState.letterValueState[static_cast<uint32_t>(gCodeLetter::F)] = theBlock.gCodeWords[i].number;
                theBlock.removeWord(i);
            } else {
                theResult = gCodeParseResultType::Error;
                theError  = gCodeParseError::InvalidFeedrate;
                return;
            }
        } else {
            if (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::FeedRate)] == static_cast<uint32_t>(modalGroupFeedRate::G93)) {
                theResult = gCodeParseResultType::Error;
                theError  = gCodeParseError::MissingFeedrate;
                return;
            }
        }
    }
}

void gCode::setFeedrateMode() {
    if (theBlock.hasWords()) {
        //	Set feed rate mode (G93, G94 — inverse time or per minute)
        int32_t i;
        if ((i = theBlock.searchWord('G', 930)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::FeedRate)] = static_cast<uint32_t>(modalGroupFeedRate::G93);
            theState.letterValueState[static_cast<uint32_t>(gCodeLetter::F)]       = 0.0;        // when changing feedrate mode, an F-word must be present - enforced by initializing to an invalid value..
        } else if ((i = theBlock.searchWord('G', 940)) >= 0) {
            theState.modalGroupsState[static_cast<uint32_t>(modalGroup::FeedRate)] = static_cast<uint32_t>(modalGroupFeedRate::G94);
            theState.letterValueState[static_cast<uint32_t>(gCodeLetter::F)]       = 0.0;        // when changing feedrate mode, an F-word must be present - enforced by initializing to an invalid value..
        }
    }
}

void gCode::getAxisOffsetRadiusWords() {
    // Step 2 : get all (ABC)XYZ - IJK - R words first ? Is there at least one ?
    if (theBlock.hasAxis()) {
        int32_t i;
        if ((i = theBlock.searchWord('X')) >= 0) {
            theState.letterValueState[static_cast<uint32_t>(gCodeLetter::X)] = theBlock.gCodeWords[i].number;
            theBlock.removeWord(i);
        }
        if ((i = theBlock.searchWord('Y')) >= 0) {
            theState.letterValueState[static_cast<uint32_t>(gCodeLetter::Y)] = theBlock.gCodeWords[i].number;
            theBlock.removeWord(i);
        }
        if ((i = theBlock.searchWord('Z')) >= 0) {
            theState.letterValueState[static_cast<uint32_t>(gCodeLetter::Z)] = theBlock.gCodeWords[i].number;
            theBlock.removeWord(i);
        }
    }
    if (theBlock.hasOffset()) {
        int32_t i;
        if ((i = theBlock.searchWord('I')) >= 0) {
            theState.letterValueState[static_cast<uint32_t>(gCodeLetter::I)] = theBlock.gCodeWords[i].number;
            theBlock.removeWord(i);
        }
        if ((i = theBlock.searchWord('J')) >= 0) {
            theState.letterValueState[static_cast<uint32_t>(gCodeLetter::J)] = theBlock.gCodeWords[i].number;
            theBlock.removeWord(i);
        }
        if ((i = theBlock.searchWord('K')) >= 0) {
            theState.letterValueState[static_cast<uint32_t>(gCodeLetter::K)] = theBlock.gCodeWords[i].number;
            theBlock.removeWord(i);
        }
    }
    if (theBlock.hasRadius()) {
        int32_t i;
        if ((i = theBlock.searchWord('R')) >= 0) {
            theState.letterValueState[static_cast<uint32_t>(gCodeLetter::R)] = theBlock.gCodeWords[i].number;
            theBlock.removeWord(i);
        }
    }
}

void gCode::getLineNumberWord() {
    // Check if there is an N block, if so, it should be the first // -1=not found, 0=found as first word in block, 1+ = found further down the block.. not allowed
    int32_t i = theBlock.searchWord('N');
    if (i > 0) {
        theResult = gCodeParseResultType::Error;
        theError  = gCodeParseError::InvalidLineNumber;
        return;
    } else if (0 == i) {
        if (theBlock.gCodeWords[i].number >= 0) {
            theState.letterValueState[static_cast<uint32_t>(gCodeLetter::N)] = theBlock.gCodeWords[i].number;
            theBlock.removeWord(i);
        } else {
            theResult = gCodeParseResultType::Error;
            theError  = gCodeParseError::InvalidLineNumber;
            return;
        }
    }
}

void gCode::calcMotion(simplifiedMotion &theMotion) {
    calcNextPosition(theMotion);
    setArcAxisFromPlane(theMotion);
    calcArcCenterAndRadius(theMotion);
    calcArcAngles(theMotion);
    calcTrajectoryLength(theMotion);
    calcSpeedAndOrDuration(theMotion);
    calcPeripherals(theMotion);
    updatePosition();
}

void gCode::calcNextPosition(simplifiedMotion &theMotion) {
    for (uint32_t anAxis = 0; anAxis < nmbrAxis; ++anAxis) {
        double tmpValue = theState.letterValueState[anAxis];
        if (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Unit)] == static_cast<uint32_t>(modalGroupUnit::G20)) {
            tmpValue = tmpValue * inches2mm;        // convert to mm
            // TODO - what if the axis is rotary in degrees / radians ?
        }

        if (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Distance)] == static_cast<uint32_t>(modalGroupDistance::G91)) {
            tmpValue = (theState.currentPosition[anAxis] + tmpValue);        // in case of relative coordinates
        }
        theState.nextPosition[anAxis] = theState.WCSorigin[theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)]][anAxis] + tmpValue;

        theMotion.trajectory.delta[anAxis]         = theState.nextPosition[anAxis] - theState.currentPosition[anAxis];
        theMotion.trajectory.startPosition[anAxis] = theState.currentPosition[anAxis];
    }
}

void gCode::setArcAxisFromPlane(simplifiedMotion &theMotion) {
    if ((theMotion.type == motionType::feedHelicalCW) || (theMotion.type == motionType::feedHelicalCCW)) {
        switch ((modalGroupPlane)theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Plane)]) {
            case modalGroupPlane::G17:
            default:
                theMotion.trajectory.arcAxis[0] = axis::X;
                theMotion.trajectory.arcAxis[1] = axis::Y;
                break;

            case modalGroupPlane::G18:
                theMotion.trajectory.arcAxis[0] = axis::Y;
                theMotion.trajectory.arcAxis[1] = axis::Z;
                break;

            case modalGroupPlane::G19:
                theMotion.trajectory.arcAxis[0] = axis::Z;
                theMotion.trajectory.arcAxis[1] = axis::X;
                break;
        }
    } else {
        theMotion.trajectory.arcAxis[0] = axis::nmbrAxis;        // not a helical move -> axis are undefined
        theMotion.trajectory.arcAxis[1] = axis::nmbrAxis;        // idem
    }
}

void gCode::calcArcCenterAndRadius(simplifiedMotion &theMotion) {
    double tmpRadius1 = 0.0;        // calculated between currentPosition and arcCenter [mm]
    double tmpRadius2 = 0.0;        // calculated between nextPosition and arcCenter [mm]

    if (theBlock.hasOffset()) {        // IJK Mode
        for (uint32_t i = 0; i < 2; ++i) {
            double tmpValue = 0.0;                                                                                                                              // used in all kinds of calculations to hold intermediate results
            tmpValue        = theState.letterValueState[static_cast<uint32_t>(theMotion.trajectory.arcAxis[i]) + static_cast<uint32_t>(gCodeLetter::I)];        // calculate arcCenter from XYZ values + IJK offsets
            if (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Unit)] == static_cast<uint32_t>(modalGroupUnit::G20))                               // Inches ??
            {
                tmpValue = tmpValue * inches2mm;        // convert to mm
            }
            theMotion.trajectory.arcCenter[i] = theState.currentPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[i])] + tmpValue;
            tmpRadius1                        = tmpRadius1 + (theState.currentPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[i])] - theMotion.trajectory.arcCenter[i]) * (theState.currentPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[i])] - theMotion.trajectory.arcCenter[i]);        // Calculate radius as distance between currentPosition and arcCenter
            tmpRadius2                        = tmpRadius2 + (theState.nextPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[i])] - theMotion.trajectory.arcCenter[i]) * (theState.nextPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[i])] - theMotion.trajectory.arcCenter[i]);              // Calculate radius as distance between nextPosition and arcCenter
        }
        tmpRadius1 = sqrt(tmpRadius1);
        tmpRadius2 = sqrt(tmpRadius2);
        if (fabs(tmpRadius1 - tmpRadius2) > 0.05)        // TODO - have smarter upper limit for the radius difference
        {
            // error - start and endpoint are not same radius distance from arccenter
            // TODO : lastError = gCodeError::InvalidArcParameters;
        }
        theMotion.trajectory.radius = tmpRadius1;
    } else if (theBlock.hasRadius()) {                                                                                               // R mode
        tmpRadius1 = theState.letterValueState[static_cast<uint32_t>(gCodeLetter::R)];                                               // get the R lettervalue
        if (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Unit)] == static_cast<uint32_t>(modalGroupUnit::G20))        // Inches
        {
            tmpRadius1 = tmpRadius1 * inches2mm;        // convert to mm
        }

        if (
            (4 * tmpRadius1 * tmpRadius1) <
            (theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])] *
             theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])]) -
                (theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])] *
                 theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])])) {
            // error - distance from start to endpoint is more than 2 * radius.. no ARC can be found
            // lastError = gCodeError::InvalidArcParameters;
        }

        if (
            (theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])] == 0) &&
            (theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])] == 0)) {
            // error - startpoint is same as endpoint -> undefined arccenter for R mode arcs
            // lastError = gCodeError::InvalidArcParameters;
        }

        double tmpH;                  // used in calculation of the arcCenter for radius mode
        double tmpD;                  // used in calculation of the arcCenter for radius mode
        double tmpValue = 0.0;        // used in all kinds of calculations to hold intermediate results

        tmpD = sqrt(
            theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])] *
                theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])] +
            theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])] *
                theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])]);

        tmpH = sqrt(
                   (4 * tmpRadius1 * tmpRadius1) -
                   (theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])] *
                    theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])]) -
                   (theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])] *
                    theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])])) *
               0.5;

        tmpValue = tmpH / tmpD;        // TODO : Catch Divide by Zero...

        // now set tmpValue sign according to G2/G3 or sign of Radius
        if (motionType::feedHelicalCW == theMotion.type) {
            tmpValue = -tmpValue;
        }

        if (tmpRadius1 < 0) {
            tmpValue   = -tmpValue;
            tmpRadius1 = -tmpRadius1;
        }

        theMotion.trajectory.arcCenter[0] = theState.currentPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])] + ((theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])] * 0.5f) - (theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])] * tmpValue));
        theMotion.trajectory.arcCenter[1] = theState.currentPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])] + ((theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])] * 0.5f) + (theMotion.trajectory.delta[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])] * tmpValue));
        theMotion.trajectory.radius       = tmpRadius1;
    } else {
        // Error : no offsets AND no radius..
        // lastError = gCodeError::InvalidArcParameters;
    }
}

void gCode::calcArcAngles(simplifiedMotion &theMotion) {
    if ((motionType::feedHelicalCW == theMotion.type) || (motionType::feedHelicalCCW == theMotion.type)) {
        theMotion.trajectory.startAngle = atan2(theState.currentPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])] - theMotion.trajectory.arcCenter[1], theState.currentPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])] - theMotion.trajectory.arcCenter[0]);
        double endAngle                 = atan2(theState.nextPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])] - theMotion.trajectory.arcCenter[1], theState.nextPosition[static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])] - theMotion.trajectory.arcCenter[0]);
        theMotion.trajectory.deltaAngle = endAngle - theMotion.trajectory.startAngle;
        if (motionType::feedHelicalCW == theMotion.type) {
            if (theMotion.trajectory.deltaAngle >= 0) {
                theMotion.trajectory.deltaAngle -= (2 * pi);
            }
        } else {
            if (theMotion.trajectory.deltaAngle <= 0) {
                theMotion.trajectory.deltaAngle += (2 * pi);
            }
        }
    } else {
        theMotion.trajectory.startAngle = 0.0F;
        theMotion.trajectory.deltaAngle = 0.0F;
    }
}

void gCode::calcTrajectoryLength(simplifiedMotion &theMotion) {
    double arcLength            = theMotion.trajectory.radius * fabs(theMotion.trajectory.deltaAngle);        // length of the ARC in [mm]
    theMotion.trajectory.length = arcLength * arcLength;                                                      // start with length of arc (squared), could be zero for linear move
    for (uint32_t anAxis = 0; anAxis < nmbrAxis; ++anAxis) {
        if ((anAxis != static_cast<uint32_t>(theMotion.trajectory.arcAxis[0])) && (anAxis != static_cast<uint32_t>(theMotion.trajectory.arcAxis[1])))        // for all axis, except for the 2 of the arcPlane
        {
            theMotion.trajectory.length = theMotion.trajectory.length + (theMotion.trajectory.delta[anAxis] * theMotion.trajectory.delta[anAxis]);        // add square length
        }
    }
    theMotion.trajectory.length = sqrt(theMotion.trajectory.length);
}

void gCode::calcSpeedAndOrDuration(simplifiedMotion &theMotion) {
    // 	####################################################
    //	4.	speed [mm/s] and/or Duration of the motion [s]
    // 	####################################################

    // Calculate Duration of the Motion :
    // G0, G1, G2, G3 : derive it from length and feedrate
    // G4 : duration is given directly, length = 0
    double tmpValue = 0.0;        // used in all kinds of calculations to hold intermediate results

    if (motionType::traverse == theMotion.type) {
        theMotion.speedProfile.vFeed    = std::numeric_limits<double>::infinity();        // [mm/s] - G93 F indicates: complete the motion in 1/F minutes,= 60 / F seconds
        theMotion.speedProfile.duration = theMotion.trajectory.length / theMotion.speedProfile.vFeed;
    } else if ((motionType::feedLinear == theMotion.type) ||
               (motionType::feedHelicalCW == theMotion.type) ||
               (motionType::feedHelicalCCW == theMotion.type)) {
        tmpValue = theState.letterValueState[static_cast<uint32_t>(gCodeLetter::F)];        // Note - check for valid feedrate is done when processing the F-word
        if (tmpValue <= 0)                                                                  // here we check for the scenario where the F-word is missing, so F is still to its default of zero
        {
            // lastError = gCodeError::InvalidFeedrate;
        }
        if (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::FeedRate)] == static_cast<uint32_t>(modalGroupFeedRate::G93))        // Inverse Feedrate
        {
            theMotion.speedProfile.vFeed    = (theMotion.trajectory.length * tmpValue) / 60.0;        // [mm/s] - G93 F indicates: complete the motion in 1/F minutes,= 60 / F seconds
            theMotion.speedProfile.duration = theMotion.trajectory.length / theMotion.speedProfile.vFeed;
        } else {
            if (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Unit)] == static_cast<uint32_t>(modalGroupUnit::G20))        // Inches
            {
                tmpValue = tmpValue * inches2mm;        // convert to mm
            }
            theMotion.speedProfile.vFeed    = tmpValue / 60.0;        // [mm/s] - G94 F indicates: do the motion at F mm (or inches) per minute.
            theMotion.speedProfile.duration = theMotion.trajectory.length / theMotion.speedProfile.vFeed;
        }
    } else {
        theMotion.speedProfile.vFeed    = 0.0;
        theMotion.speedProfile.duration = theState.letterValueState[static_cast<uint32_t>(gCodeLetter::P)];        // Get the duration of the Dwell from P letter value
    }
}

void gCode::calcPeripherals(simplifiedMotion &theMotion) {
    // 	######################################
    //	5.	Spindle and Coolant
    // 	######################################

    theMotion.peripherals.spindleSpeed     = 0;
    theMotion.peripherals.spindledirection = rotationDirection::clockwise;
    if (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Spindle)] != static_cast<uint32_t>(modalGroupSpindle::M5)) {
        theMotion.peripherals.spindleSpeed = theState.letterValueState[static_cast<uint32_t>(gCodeLetter::S)];
        if (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::Spindle)] != static_cast<uint32_t>(modalGroupSpindle::M4)) {
            theMotion.peripherals.spindledirection = rotationDirection::clockwise;
        } else {
            theMotion.peripherals.spindledirection = rotationDirection::counterClockwise;
        }
    }

    theMotion.peripherals.coolantFlood = (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoolantFlood)] == static_cast<uint32_t>(modalGroupCoolantFlood::M7));
    theMotion.peripherals.coolantMist  = (theState.modalGroupsState[static_cast<uint32_t>(modalGroup::CoolantMist)] == static_cast<uint32_t>(modalGroupCoolantMist::M8));
}

void gCode::updatePosition() {
    for (uint32_t anAxis = 0; anAxis < nmbrAxis; ++anAxis) {
        theState.currentPosition[anAxis] = theState.nextPosition[anAxis];
    }
}

void gCode::getBlockFromString(const uint8_t *gCodeAsString) {
    theBlock.getBlockFromString(gCodeAsString);
}
void gCode::getBlockFromString(const char *gCodeAsString) {
    theBlock.getBlockFromString(reinterpret_cast<const uint8_t *>(gCodeAsString));
}

// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "logging.h"
#include <math.h>
#include "gcode.h"

extern Logger logger;

//#define log_gCode_parser


uint8_t gCode::getNmbrWords() const
    {
    return theBlock.nmbrWords;
    };

void gCode::countModalGroupAndOthers(uint8_t i)
    {
    if ('G' == theBlock.gCodeWords[i].letter)
        {
        switch (theBlock.gCodeWords[i].intNumber)
            {
            case 0:
            case 10:
            case 20:
            case 30:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::Motion];
                break;

            case 170:
            case 180:
            case 190:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::Plane];
                break;

            case 900:
            case 910:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::Distance];
                break;

            case 200:
            case 210:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::Unit];
                break;

            case 930:
            case 940:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::FeedRate];
                break;

            case 540:
            case 550:
            case 560:
            case 570:
            case 580:
            case 590:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::CoordinateSet];
                break;

            case 40:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::Motion];
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::Group0];
                break;

            case 100:
            case 280:
            case 300:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::Group0];
                break;

            case 530:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::CoordinateSet];
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::Group0];
                break;
            }
        }
    else if ('M' == theBlock.gCodeWords[i].letter)
        {
        switch (theBlock.gCodeWords[i].intNumber)
            {
            case 30:
            case 40:
            case 50:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::Spindle];
                break;

            case 70:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::CoolantMist];
                break;

            case 80:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::CoolantFlood];
                break;

            case 90:
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::CoolantFlood];
                ++theBlock.modalGroupCount[(uint8_t) modalGroup::CoolantMist];
                break;
            }
        }
    else
        {
        switch (theBlock.gCodeWords[i].letter)
            {
            // TODO, add all other valueLetters
            case 'F':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::F];
                break;
            case 'L':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::L];
                break;
            case 'N':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::N];
                break;
            case 'P':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::P];
                break;
            case 'R':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::R];
                theBlock.hasRadius = true;
                break;
            case 'S':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::S];
                break;
            case 'X':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::X];
                theBlock.hasAxis = true;
                break;
            case 'Y':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::Y];
                theBlock.hasAxis = true;
                break;
            case 'Z':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::Z];
                theBlock.hasAxis = true;
                break;
            case 'I':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::I];
                theBlock.hasOffset = true;
                break;
            case 'J':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::J];
                theBlock.hasOffset = true;
                break;
            case 'K':
                ++theBlock.valueWordCount[(uint8_t) gCodeLetter::K];
                theBlock.hasOffset = true;
                break;
            }
        }
    };

void gCode::getBlock(char* commandLine)
    {
    uint8_t currentByte;
    uint8_t readPos = 0;
    uint8_t writePos = 0;
    bool inComment = false;
    char number[16] = "";
    uint8_t writePos2 = 0;

    theBlock.nmbrWords = 0;			// reset counter for counting words inside a block
    theBlock.hasAxis = false;		// X Y Z A B C
    theBlock.hasOffset = false;		// I J K
    theBlock.hasRadius = false;		// R

#ifdef log_gCode_parser
    logger.logNow("getBlock : '");
    logger.logNow(commandLine);
    logger.logNow("'\n");
#endif

    // reset counts for words part of modalGroups. Only maximum one G- or M-code for each of these blocks is allowed
    for (uint8_t i = 0; i < (uint8_t)modalGroup::nmbrModalGroups; ++i)
        {
        theBlock.modalGroupCount[i] = 0;
        }

    // reset counts for all non G and M words, called valueWords
    for (uint8_t i = 0; i < (uint8_t)gCodeLetter::nmbrLetters; ++i)
        {
        theBlock.valueWordCount[i] = 0;
        }

    // 1. Strip all Whitespace and Comments
    while ((currentByte = commandLine[readPos]))										// read new byte from source for as long as they are not zero, which indicated rxBuffer underrun
        {
        if ('\n' == currentByte)													// if linefeed...
            {
            break;																// ... is 'end of line' -> break out of loop ...
            }
        switch (currentByte)
            {
            case '(':															// detect beginning of a comment
                inComment = true;													// remember by setting flag
                break;

            case ')':															// detect end of a comment
                inComment = false;												// reset flag
                break;

            case ' ':															// strip spaces
            case '\t':															// strip TABs
                break;

            default:
                if (!inComment)													// if NOT inside comment...
                    {
                    if ((currentByte >= 'a') && (currentByte <= 'z'))					// if any lowercase...
                        {
                        currentByte -= ('a' - 'A');									// ... convert to uppercase to simplify parsing later-on
                        }
                    gCodeLine[writePos] = currentByte;							// ... write to output
                    writePos++;													// move to next position in destination
                    }
                break;
            }
        ++readPos;
        }
    gCodeLine[writePos] = 0; // Terminate

#ifdef log_gCode_parser
    logger.logNow("getBlock cleaned to : '");
    logger.logNow(gCodeLine);
    logger.logNow("'\n");
#endif

    // 2. Now parse the line of text and convert into an array of gCodeWords
    readPos = 0;
    while (gCodeLine[readPos] && (theBlock.nmbrWords < theBlock.MaxGCodeWordsPerGCodeBlock))
        {
        // 1. Proceed to next A-Z
        if (!(('A' <= gCodeLine[readPos]) && ('Z' >= gCodeLine[readPos])))
            {
            ++readPos;
            }
        else
            {
            // Get the letter..
            theBlock.gCodeWords[theBlock.nmbrWords].letter = gCodeLine[readPos];
            // 2. Then after the 'letter', expect the number
            // For strtod to work 100% reliable, we need to really isolate the number part into a separate string...
            ++readPos;
            writePos2 = 0;
            while (gCodeLine[readPos] && (writePos2 < 15) && (!(('A' <= gCodeLine[readPos]) && ('Z' >= gCodeLine[readPos]))))
                {
                number[writePos2] = gCodeLine[readPos];
                ++readPos;
                ++writePos2;
                }
            number[writePos2] = 0;	// Terminate the new number string

            theBlock.gCodeWords[theBlock.nmbrWords].number = strtod(number, NULL);


            if (('G' == theBlock.gCodeWords[theBlock.nmbrWords].letter) || ('M' == theBlock.gCodeWords[theBlock.nmbrWords].letter))
                {
                theBlock.gCodeWords[theBlock.nmbrWords].intNumber = (uint32_t)(theBlock.gCodeWords[theBlock.nmbrWords].number * 10);
                }
            else
                {
                theBlock.gCodeWords[theBlock.nmbrWords].intNumber = 0;
                }
            countModalGroupAndOthers(theBlock.nmbrWords);

#ifdef log_gCode_parser
			snprintf(logger.logLine, 127, "  %c %f %d\n", theBlock.gCodeWords[theBlock.nmbrWords].letter, theBlock.gCodeWords[theBlock.nmbrWords].number, theBlock.gCodeWords[theBlock.nmbrWords].intNumber);
			logger.logNow();
#endif

            ++theBlock.nmbrWords;
            }
        }
#ifdef log_gCode_parser
	snprintf(logger.logLine, 127, "  %d words detected\n", theBlock.nmbrWords);
	logger.logNow();
#endif
    };

int8_t gCode::searchWord(uint8_t aLetter, uint32_t anIntNumber = 0) const
    {
    if (('G' == aLetter) || ('M' == aLetter))	// for G and M
        {
        for (uint8_t i = 0; i < theBlock.nmbrWords; ++i)
            {
            if ((theBlock.gCodeWords[i].letter == aLetter) && (theBlock.gCodeWords[i].intNumber == anIntNumber))
                {
                return i;
                }
            }
        }
    else
        {
        for (uint8_t i = 0; i < theBlock.nmbrWords; ++i)
            {
            if (theBlock.gCodeWords[i].letter == aLetter)
                {
                return i;
                }
            }
        }
    return -1;
    }

void gCode::removeWord(uint8_t i)
    {
    if (theBlock.nmbrWords > 0)
        {
        if (theBlock.nmbrWords > 1)
            {
            theBlock.gCodeWords[i].letter = theBlock.gCodeWords[theBlock.nmbrWords - 1].letter;
            theBlock.gCodeWords[i].number = theBlock.gCodeWords[theBlock.nmbrWords - 1].number;
            theBlock.gCodeWords[i].intNumber = theBlock.gCodeWords[theBlock.nmbrWords - 1].intNumber;
            }
        --theBlock.nmbrWords;
        }
    };

gCode::gCode()
    {
#ifdef log_gCode_class						// logging output for the gCode parser
    logger.logNow("gCode constructor start\n");
#endif

    // Initialize all Model groups
    theState.modalGroupsState[(uint8_t)modalGroup::Motion] = (uint8_t)modalGroupMotion::G0;
    theState.modalGroupsState[(uint8_t)modalGroup::FeedRate] = (uint8_t)modalGroupFeedRate::G94;
    theState.modalGroupsState[(uint8_t)modalGroup::Spindle] = (uint8_t)modalGroupSpindle::M5;
    theState.modalGroupsState[(uint8_t)modalGroup::Unit] = (uint8_t)modalGroupUnit::G21;
    theState.modalGroupsState[(uint8_t)modalGroup::Unit] = (uint8_t)modalGroupUnit::G21;
    theState.modalGroupsState[(uint8_t)modalGroup::Plane] = (uint8_t)modalGroupPlane::G17;
    theState.modalGroupsState[(uint8_t)modalGroup::Distance] = (uint8_t)modalGroupDistance::G90;
    theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet] = (uint8_t)modalGroupCoordinateSet::G54;
    theState.modalGroupsState[(uint8_t)modalGroup::CoolantFlood] = (uint8_t)modalGroupCoolantFlood::M9;
    theState.modalGroupsState[(uint8_t)modalGroup::CoolantMist] = (uint8_t)modalGroupCoolantMist::M9;
    theState.modalGroupsState[(uint8_t)modalGroup::ManualOverride] = (uint8_t)modalGroupOverrides::M48;

    // initialize all letterValues
    for (uint8_t theGCodeLetter = (uint8_t)gCodeLetter::X; theGCodeLetter < (uint8_t)gCodeLetter::nmbrLetters; theGCodeLetter++)
        {
        theState.letterValueState[theGCodeLetter] = 0.0;	// Array to hold value for each used gCode letter
        }

    for (uint8_t theAxis = 0; theAxis < (uint8_t)axis::nmbrAxis; ++theAxis)						// iterate over all axis
        {
        theState.currentPosition[theAxis] = 0.0;
        }


    // start from compile-time defaults
    for (uint8_t theModalGroupCoordinateSet = 0; theModalGroupCoordinateSet < (uint8_t)modalGroupCoordinateSet::nmbrModes; theModalGroupCoordinateSet++)
        {
        for (uint8_t theAxis = (uint8_t)axis::X; theAxis < (uint8_t)axis::nmbrAxis; theAxis++)
            {
            theState.WCSorigin[theModalGroupCoordinateSet][theAxis] = 0;
            }
        }

    // Test Only Defaults
    theState.WCSorigin[(uint8_t)modalGroupCoordinateSet::G54][(uint8_t)axis::X] = 0;
    theState.WCSorigin[(uint8_t)modalGroupCoordinateSet::G54][(uint8_t)axis::Y] = 0;
    theState.WCSorigin[(uint8_t)modalGroupCoordinateSet::G54][(uint8_t)axis::Z] = 0;

    // read Machine settings from SD-Card to override the defaults
    // Todo

#ifdef log_gCode_class
    logger.logNow("gCode constructor end\n");
#endif
    };

void gCode::parseBlock(gCodeParserResult &theParseResult)
    {
    int8_t i, j, k;										// indexes of gcode words in the array. int io uint, as 'not found' returns -1. All Words inside the block are executed in the correct order - no matter what order they appear on the line. Any word found is executed, and then removed from the block. At the same time, all syntax errors are being checked
    theParseResult.theParseResultType = gCodeParserResult::ParseResultType::EmptyBlock;
	theParseResult.motion.theMotionType = MotionType::None;
    theParseResult.error = gCodeParserResult::Error::None;

    // If we are in inverse feedmode or G91, we should reset F and axis for each block
    // even going to these modes in  a block, we should do that.. before reading any axis words
    // TODO : put some extra tests in the code, depending on the number of Axis
    // If 1 axis -> no helical possible
    // if 2 axis -> only 1 plane possible
    // speed optimization : put code in blocks to only execute if there are still gCode words - most lines will contain only axis words, and there is no need to search for all possible options if there ar eno more words..
    // even if this search returns immediately when nmbrWord == 0
    // G0 or G1 without axis : could be error or just set modal group motion..

    // Step 1 : check for modalgroup and letterword violations...
    for (uint8_t i = 0; i < (uint8_t)modalGroup::nmbrModalGroups; ++i)
        {
        if (theBlock.modalGroupCount[i] > 1)
            {
            theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
            theParseResult.error = gCodeParserResult::Error::ModalGroupCollision;
            theBlock.nmbrWords = 0;
            return;
            }
        }
    // Check if Value words are present max 1 per block
    for (uint8_t i = 0; i < (uint8_t)gCodeLetter::nmbrLetters; ++i)
        {
        if (theBlock.valueWordCount[i] > 1)
            {
            theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
            theParseResult.error = gCodeParserResult::Error::ValueWordCollision;
            theBlock.nmbrWords = 0;
            return;
            }
        }

    // Step 2 : get all (ABC)XYZ - IJK - R words first ? Is there at least one ?
    if (theBlock.hasAxis)
        {
        if ((i = searchWord('X')) >= 0)
            {
            theState.letterValueState[(uint8_t) gCodeLetter::X] = theBlock.gCodeWords[i].number;
            removeWord(i);
            }
        if ((i = searchWord('Y')) >= 0)
            {
            theState.letterValueState[(uint8_t) gCodeLetter::Y] = theBlock.gCodeWords[i].number;
            removeWord(i);
            }
        if ((i = searchWord('Z')) >= 0)
            {
            theState.letterValueState[(uint8_t) gCodeLetter::Z] = theBlock.gCodeWords[i].number;
            removeWord(i);
            }
        }
    if (theBlock.hasOffset)
        {
        if ((i = searchWord('I')) >= 0)
            {
            theState.letterValueState[(uint8_t) gCodeLetter::I] = theBlock.gCodeWords[i].number;
            removeWord(i);
            }
        if ((i = searchWord('J')) >= 0)
            {
            theState.letterValueState[(uint8_t) gCodeLetter::J] = theBlock.gCodeWords[i].number;
            removeWord(i);
            }
        if ((i = searchWord('K')) >= 0)
            {
            theState.letterValueState[(uint8_t) gCodeLetter::K] = theBlock.gCodeWords[i].number;
            removeWord(i);
            }
        }
    if (theBlock.hasRadius)
        {
        if ((i = searchWord('R')) >= 0)
            {
            theState.letterValueState[(uint8_t) gCodeLetter::R] = theBlock.gCodeWords[i].number;
            removeWord(i);
            }
        }

    // Check if there is an N block, if so, it should be the first // -1=not found, 0=found as first word in block, 1+ = found further down the block.. not allowed
    i = searchWord('N');
    if (i > 0)
        {
        theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
        theParseResult.error = gCodeParserResult::Error::InvalidLineNumber;
        theBlock.nmbrWords = 0;
        return;
        }
    else if (0 == i)
        {
        if (theBlock.gCodeWords[i].number >= 0)
            {
            theState.letterValueState[(uint8_t) gCodeLetter::N] = theBlock.gCodeWords[i].number;
            removeWord(i);
            }
        else
            {
            theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
            theParseResult.error = gCodeParserResult::Error::InvalidLineNumber;
            theBlock.nmbrWords = 0;
            return;
            }
        }

    if (theBlock.nmbrWords > 0)
        {
        //	Set feed rate mode (G93, G94 — inverse time or per minute)
        if ((i = searchWord('G', 930)) >= 0)
            {
            theState.modalGroupsState[(uint8_t) modalGroup::FeedRate] = (uint8_t) modalGroupFeedRate::G93;
            theState.letterValueState[(uint8_t) gCodeLetter::F] = 0.0;	// when changing feedrate mode, an F-word must be present - enforced by initializing to an invalid value..
            removeWord(i);
            }
        else if ((i = searchWord('G', 940)) >= 0)
            {
            theState.modalGroupsState[(uint8_t) modalGroup::FeedRate] = (uint8_t) modalGroupFeedRate::G94;
            theState.letterValueState[(uint8_t) gCodeLetter::F] = 0.0;	// when changing feedrate mode, an F-word must be present - enforced by initializing to an invalid value..
            removeWord(i);
            }
        }

    if (theBlock.nmbrWords > 0)
        {
        //	Set feed rate (F) value
        i = searchWord('F');
        if (i >= 0)
            {
            if (theBlock.gCodeWords[i].number > 0)	// Feedrate <= 0 is not allowed / makes no sense
                {
                theState.letterValueState[(uint8_t)gCodeLetter::F] = theBlock.gCodeWords[i].number;
                removeWord(i);
                }
            else
                {
                theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
                theParseResult.error = gCodeParserResult::Error::InvalidFeedrate;
                theBlock.nmbrWords = 0;
                return;
                }
            }
        else
            {
            if (theState.modalGroupsState[(uint8_t)modalGroup::FeedRate] == (uint8_t)modalGroupFeedRate::G93)
                {
                theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
                theParseResult.error = gCodeParserResult::Error::MissingFeedrate;
                theBlock.nmbrWords = 0;
                return;
                }
            }
        }

    if (theBlock.nmbrWords > 0)
        {
        //	Set spindle speed (S) value
        i = searchWord('S');
        if (i >= 0)
            {
            if (theBlock.gCodeWords[i].number >= 0)
                {
                theState.letterValueState[(uint8_t)gCodeLetter::S] = theBlock.gCodeWords[i].number;
                removeWord(i);
                }
            else
                {
                theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
                theParseResult.error = gCodeParserResult::Error::InvalidSpindleSpeed;
                theBlock.nmbrWords = 0;
                return;
                }
            }
        }

    if (theBlock.nmbrWords > 0)
        {
        //	spindle on or off (M3, M4, M5)
        if ((i = searchWord('M', 30)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::Spindle] = (uint8_t)modalGroupSpindle::M3;
            removeWord(i);
            }
        else if ((i = searchWord('M', 40)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::Spindle] = (uint8_t)modalGroupSpindle::M4;
            removeWord(i);
            }
        else if ((i = searchWord('M', 50)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::Spindle] = (uint8_t)modalGroupSpindle::M5;
            removeWord(i);
            }
        }

    if (theBlock.nmbrWords > 0)
        {
        //	Coolant on or off (M7, M8, M9)
        if ((i = searchWord('M', 70)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoolantFlood] = (uint8_t)modalGroupCoolantFlood::M7;
            removeWord(i);
            }
        if ((i = searchWord('M', 80)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoolantMist] = (uint8_t)modalGroupCoolantMist::M8;
            removeWord(i);
            }
        if ((i = searchWord('M', 90)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoolantFlood] = (uint8_t)modalGroupCoolantFlood::M9;
            theState.modalGroupsState[(uint8_t)modalGroup::CoolantMist] = (uint8_t)modalGroupCoolantMist::M9;
            removeWord(i);
            }
        }

    if (theBlock.nmbrWords > 0)
        {

        //	enable or disable overrides (M48, M49)
        if ((i = searchWord('M', 480)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::ManualOverride] = (uint8_t)modalGroupOverrides::M48;
            removeWord(i);
            }
        else if ((i = searchWord('M', 490)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::ManualOverride] = (uint8_t)modalGroupOverrides::M49;
            removeWord(i);
            }
        }

    if (theBlock.nmbrWords > 0)
        {
        //	dwell (G4)
        i = searchWord('G', 40);
        if (i >= 0)
            {
            j = searchWord('P');
            if (j >= 0)
                {
                if (theBlock.gCodeWords[j].number >= 0)
                    {
                    theState.letterValueState[(uint8_t)gCodeLetter::P] = theBlock.gCodeWords[j].number;
                    theParseResult.theParseResultType = gCodeParserResult::ParseResultType::OkContextUpdateAndMotion;
                    theParseResult.motion.theMotionType = MotionType::PauseAndResume;
                    calcMotion(theParseResult);
                    removeWord(i);
                    removeWord(j);
                    return;
                    }
                else
                    {
                    theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
                    theParseResult.error = gCodeParserResult::Error::InvalidDwellTime;
                    theBlock.nmbrWords = 0;
                    return;
                    }
                }
            else
                {
                theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
                theParseResult.error = gCodeParserResult::Error::MissingPForG4;
                theBlock.nmbrWords = 0;
                return;
                }
            }
        }


    if (theBlock.nmbrWords > 0)
        {
        // Set active plane (G17, G18, G19)
        if ((i = searchWord('G', 170)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::Plane] = (uint8_t)modalGroupPlane::G17;
            removeWord(i);
            }
        else if (((i = searchWord('G', 180)) >= 0) && ((uint8_t)axis::nmbrAxis >= 3))			// G18 G19 only make sense on a 3D system so axis::nmbrAxis >= 3
            {
            theState.modalGroupsState[(uint8_t)modalGroup::Plane] = (uint8_t)modalGroupPlane::G18;
            removeWord(i);
            }
        else if (((i = searchWord('G', 190)) >= 0) && ((uint8_t)axis::nmbrAxis >= 3))
            {
            theState.modalGroupsState[(uint8_t)modalGroup::Plane] = (uint8_t)modalGroupPlane::G19;
            removeWord(i);
            }
        }


    if (theBlock.nmbrWords > 0)
        {
        //	Set length units (G20, G21)
        if ((i = searchWord('G', 200)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::Unit] = (uint8_t)modalGroupUnit::G20;
            removeWord(i);
            }
        else if ((i = searchWord('G', 210)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::Unit] = (uint8_t)modalGroupUnit::G21;
            removeWord(i);
            }
        }

    if (theBlock.nmbrWords > 0)
        {
        //	Coordinate system selection (G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3)
        if ((i = searchWord('G', 540)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet] = (uint8_t)modalGroupCoordinateSet::G54;
            removeWord(i);
            }
        else if ((i = searchWord('G', 550)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet] = (uint8_t)modalGroupCoordinateSet::G55;
            removeWord(i);

            }
        else if ((i = searchWord('G', 560)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet] = (uint8_t)modalGroupCoordinateSet::G56;
            removeWord(i);

            }
        else if ((i = searchWord('G', 570)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet] = (uint8_t)modalGroupCoordinateSet::G57;
            removeWord(i);

            }
        else if ((i = searchWord('G', 580)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet] = (uint8_t)modalGroupCoordinateSet::G58;
            removeWord(i);

            }
        else if ((i = searchWord('G', 590)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet] = (uint8_t)modalGroupCoordinateSet::G59;
            removeWord(i);

            }
        else if ((i = searchWord('G', 591)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet] = (uint8_t)modalGroupCoordinateSet::G59_1;
            removeWord(i);

            }
        else if ((i = searchWord('G', 592)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet] = (uint8_t)modalGroupCoordinateSet::G59_2;
            removeWord(i);
            }
        else if ((i = searchWord('G', 593)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet] = (uint8_t)modalGroupCoordinateSet::G59_3;
            removeWord(i);
            }
        }

    if (theBlock.nmbrWords > 0)
        {
        //	set distance mode (G90, G91)
        if ((i = searchWord('G', 900)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::Distance] = (uint8_t)modalGroupDistance::G90;
            removeWord(i);
            }
        else if ((i = searchWord('G', 910)) >= 0)
            {
            theState.modalGroupsState[(uint8_t)modalGroup::Distance] = (uint8_t)modalGroupDistance::G91;
            removeWord(i);
            }
        }

    if (theBlock.nmbrWords > 0)
        {

        //	Change coordinate system data (G10)
        if ((i = searchWord('G', 100)) >= 0)
            {
            if ((j = searchWord('L', 20)) >= 0)
                {
                if ((k = searchWord('P')) >= 0)
                    {
                    // TODO : Check P integer 1 .. 9
                    }
                else
                    {
                    theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
                    theParseResult.error = gCodeParserResult::Error::MissingPForG10;
                    theBlock.nmbrWords = 0;
                    return;
                    }
                }
            else if ((j = searchWord('L', 200)) >= 0)
                {
                if ((k = searchWord('P')) >= 0)
                    {
                    // TODO : Check P integer 1 .. 9
                    }
                else
                    {
                    theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
                    theParseResult.error = gCodeParserResult::Error::MissingPForG10;
                    theBlock.nmbrWords = 0;
                    return;
                    }
                }
            else
                {
                theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
                theParseResult.error = gCodeParserResult::Error::MissingLForG10;
                theBlock.nmbrWords = 0;
                return;
                }
            }
        }

    if ((i = searchWord('G', 530)) >= 0)
        {
        // NonModal G53 move
        uint8_t currentCoordinateSet = theState.modalGroupsState[(uint8_t) modalGroup::CoordinateSet];						// remember which WCS is active
        theState.modalGroupsState[(uint8_t) modalGroup::CoordinateSet] = (uint8_t) modalGroupCoordinateSet::G53;			// just for this move, use machine coordinates

        if ((j = searchWord('G', 0)) >= 0)
            {
            // non-modal move at traverse rate
            theParseResult.motion.theMotionType = MotionType::Traverse;
            calcMotion(theParseResult);
            removeWord(j);
            }
        else if ((j = searchWord('G', 10)) >= 0)
            {
            // non-modal move at feed rate
            theParseResult.motion.theMotionType = MotionType::FeedLinear;
            // check if feedrate > 0
            calcMotion(theParseResult);

            removeWord(j);
            }
        else
            {
            //non-modal move at current G0/G1 rate - determine if G0 or G1 and add another one
            switch ((modalGroupMotion) modalGroup::Motion)
                {
                case modalGroupMotion::G0:
                    theParseResult.motion.theMotionType = MotionType::Traverse;
                    calcMotion(theParseResult);
                    removeWord(j);
                    break;

                case modalGroupMotion::G1:
                    theParseResult.motion.theMotionType = MotionType::FeedLinear;
                    // check if feedrate > 0
                    calcMotion(theParseResult);
                    removeWord(j);
                    break;

                default:
                    // Error : G53 without G0 or G1 and modalGroup::Motion = G2 or G3..
                    break;
                }
            }
        removeWord(i);
        theState.modalGroupsState[(uint8_t) modalGroup::CoordinateSet] = currentCoordinateSet;	// go back to whatever WCS was active

        }
    else
        {
        // No G53, so normal modal moves
        if ((i = searchWord('G', 0)) >= 0)
            {
            theState.modalGroupsState[(uint8_t) modalGroup::Motion] = (uint8_t) modalGroupMotion::G0;

            theParseResult.theParseResultType = gCodeParserResult::ParseResultType::OkContextUpdateAndMotion;
            theParseResult.error = gCodeParserResult::Error::None;
            theParseResult.motion.theMotionType = MotionType::Traverse;
            calcMotion(theParseResult);
            removeWord(i);
            return;
            }
        else if ((i = searchWord('G', 10)) >= 0)
            {
            theState.modalGroupsState[(uint8_t) modalGroup::Motion] = (uint8_t) modalGroupMotion::G1;
            theParseResult.motion.theMotionType = MotionType::FeedLinear;
            if (theState.letterValueState[(uint8_t)gCodeLetter::F] > 0)
                {
                calcMotion(theParseResult);
                removeWord(i);
                if (theState.modalGroupsState[(uint8_t)modalGroup::FeedRate] == (uint8_t)modalGroupFeedRate::G93)
                    {
                    theState.letterValueState[(uint8_t)gCodeLetter::F] = 0;
                    }
                theParseResult.error = gCodeParserResult::Error::None;
                theParseResult.theParseResultType = gCodeParserResult::ParseResultType::OkContextUpdateAndMotion;
                return;
                }
            else
                {
                theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
                theParseResult.error = gCodeParserResult::Error::InvalidFeedrate;
                theBlock.nmbrWords = 0;
                return;
                }
            }
        else if ((i = searchWord('G', 20)) >= 0)
            {
            theState.modalGroupsState[(uint8_t) modalGroup::Motion] = (uint8_t) modalGroupMotion::G2;
            theParseResult.motion.theMotionType = MotionType::FeedHelicalCW;
            // check if feedrate > 0
            calcMotion(theParseResult);
            removeWord(i);
            theParseResult.error = gCodeParserResult::Error::None;
            theParseResult.theParseResultType = gCodeParserResult::ParseResultType::OkContextUpdateAndMotion;
            return;
            }
        else if ((i = searchWord('G', 30)) >= 0)
            {
            theState.modalGroupsState[(uint8_t) modalGroup::Motion] = (uint8_t) modalGroupMotion::G3;
            theParseResult.motion.theMotionType = MotionType::FeedHelicalCCW;
            // check if feedrate > 0
            calcMotion(theParseResult);
            removeWord(i);
            theParseResult.error = gCodeParserResult::Error::None;
            theParseResult.theParseResultType = gCodeParserResult::ParseResultType::OkContextUpdateAndMotion;
            return;
            }
        else if (theBlock.hasAxis)	// no G-code, but axis words -> continue the current mode - TODO : do I need additional checks ?
            {
            switch (theState.modalGroupsState[(uint8_t)modalGroup::Motion])
                {
                case (uint8_t) modalGroupMotion::G0:
                    theParseResult.motion.theMotionType = MotionType::Traverse;
                    break;
                case (uint8_t)modalGroupMotion::G1:
                    theParseResult.motion.theMotionType = MotionType::FeedLinear;
                    break;
                case (uint8_t)modalGroupMotion::G2:
                    theParseResult.motion.theMotionType = MotionType::FeedHelicalCW;
                    break;
                case (uint8_t)modalGroupMotion::G3:
                    theParseResult.motion.theMotionType = MotionType::FeedHelicalCCW;
                    break;
                default:
                    // Todo : Error ?
                    break;
                }

            // check if feedrate > 0
            calcMotion(theParseResult);
            theParseResult.error = gCodeParserResult::Error::None;
            theParseResult.theParseResultType = gCodeParserResult::ParseResultType::OkContextUpdateAndMotion;
            return;
            }
        }

    if (theBlock.nmbrWords > 0)
        {
        //	stop (M0, M1, M2, M30, M60)
        if ((i = searchWord('M', 0)) >= 0)
            {
            // pause until resume
            }
        else if ((i = searchWord('M', 10)) >= 0)
            {
            // pause until resume
            }
        else if ((i = searchWord('M', 20)) >= 0)
            {
            // program ends.. ignore all subsequent lines of gCode - reset parser to default state
            }
        else if ((i = searchWord('M', 300)) >= 0)
            {
            // program ends.. ignore all subsequent lines of gCode - reset parser to default state
            }
        else if ((i = searchWord('M', 600)) >= 0)
            {
            // program ends.. ignore all subsequent lines of gCode - reset parser to default state
            }
        }
    // Now check for any unused words - normally they should be unsupported
    if (theBlock.nmbrWords > 0)
        {
        theParseResult.theParseResultType = gCodeParserResult::ParseResultType::Error;
        theParseResult.error = gCodeParserResult::Error::UnsupportedgCodeWord;
        theBlock.nmbrWords = 0;
        }

    theParseResult.error = gCodeParserResult::Error::None;
    theParseResult.theParseResultType = gCodeParserResult::ParseResultType::OkContextUpdateOnly;
    return;
    };

// calculateMotion takes care of the gCode-part of calculating a motion
// type of themotion : linear - helical (arc)
// target position
// translate G2/G3 parameters into an ARC
// speeds as defined by the gCode
// As this function is not real-time constrained, all calculations can be done in double precision

// afterwards, Motion::append() will do all further calculations beyond the gCode
// restriction of speed, acceleration within machine boundaries
// translate to parameters for efficient real-time calculation
// the real-time calculations will use (hardware) float (single precision)


void gCode::calcMotion(gCodeParserResult &theParseResult)
    {
    // 	################################
    //	1.	Calculate Next Position [mm]
    // 	################################
    //	take into account units (G20 G21), distance mode (G90 G91), machine or work coordinate set (G53 G54 ..), ...

    for (uint8_t i = 0; i < (uint8_t)axis::nmbrAxis; ++i)
        {
        double tmpValue = 0.0;																// used in all kinds of calculations to hold intermediate results
        tmpValue = theState.letterValueState[i];
        if (theState.modalGroupsState[(uint8_t)modalGroup::Unit] == (uint8_t)modalGroupUnit::G20)	// Inches ??
            {
            tmpValue = tmpValue * inches2mm;												// convert to mm - TODO - what if the axis is rotary in degrees / radians ?
            }

        if (theState.modalGroupsState[(uint8_t)modalGroup::Distance] == (uint8_t)modalGroupDistance::G91)
            {
            tmpValue = (theState.currentPosition[i] + tmpValue);										// in case of relative coordinates
            }
        theState.nextPosition[i] = theState.WCSorigin[theState.modalGroupsState[(uint8_t)modalGroup::CoordinateSet]][i] + tmpValue;

        theParseResult.motion.trajectory.delta[i] = theState.nextPosition[i] - theState.currentPosition[i];
        theParseResult.motion.trajectory.startPosition[i] = theState.currentPosition[i];
        }


    // 	###############################
    //	2.	Calculate Helical movements
    // 	###############################
    double arcLength = 0.0;												// length of the ARC in [mm]
    theParseResult.motion.trajectory.arcAxis[0] = axis::nmbrAxis;							// not a helical move -> axis are undefined
    theParseResult.motion.trajectory.arcAxis[1] = axis::nmbrAxis;							// idem

    //	2.1 Determine the plane in which the ARC runs
    if ((theParseResult.motion.theMotionType == MotionType::FeedHelicalCW) || (theParseResult.motion.theMotionType == MotionType::FeedHelicalCCW))
        {
        switch ((modalGroupPlane)theState.modalGroupsState[(uint8_t)modalGroup::Plane])
            {
            case modalGroupPlane::G17:
            default:
                theParseResult.motion.trajectory.arcAxis[0] = axis::X;
                theParseResult.motion.trajectory.arcAxis[1] = axis::Y;
                break;

            case modalGroupPlane::G18:
                theParseResult.motion.trajectory.arcAxis[0] = axis::Y;
                theParseResult.motion.trajectory.arcAxis[1] = axis::Z;
                break;

            case modalGroupPlane::G19:
                theParseResult.motion.trajectory.arcAxis[0] = axis::Z;
                theParseResult.motion.trajectory.arcAxis[1] = axis::X;
                break;
            }

        //	2.2 Determine the arcCenter and radius of the ARC
        double radius1 = 0.0;												// calculated between currentPosition and arcCenter [mm]
        double radius2 = 0.0;												// calculated between nextPosition and arcCenter [mm]

        if (theBlock.hasOffset)
            // IJK Mode
            {
            for (uint8_t i = 0; i < 2; ++i)
                {
                double tmpValue = 0.0;																				// used in all kinds of calculations to hold intermediate results
                tmpValue = theState.letterValueState[(uint8_t)theParseResult.motion.trajectory.arcAxis[i] + (uint8_t)gCodeLetter::I];					// calculate arcCenter from XYZ values + IJK offsets
                if (theState.modalGroupsState[(uint8_t)modalGroup::Unit] == (uint8_t)modalGroupUnit::G20)					// Inches ??
                    {
                    tmpValue = tmpValue * inches2mm;																// convert to mm
                    }
                theParseResult.motion.trajectory.arcCenter[i] = theState.currentPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[i]] + tmpValue;
                radius1 = radius1 + (theState.currentPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[i]] - theParseResult.motion.trajectory.arcCenter[i]) * (theState.currentPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[i]] - theParseResult.motion.trajectory.arcCenter[i]);	// Calculate radius as distance between currentPosition and arcCenter
                radius2 = radius2 + (theState.nextPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[i]] - theParseResult.motion.trajectory.arcCenter[i]) * (theState.nextPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[i]] - theParseResult.motion.trajectory.arcCenter[i]);		// Calculate radius as distance between nextPosition and arcCenter
                }
            radius1 = sqrt(radius1);
            radius2 = sqrt(radius2);
            if (fabs(radius1 - radius2) > 0.05)	// TODO - have smarter upper limit for the radius difference
                {
                // error - start and endpoint are not same radius distance from arccenter
                //lastError = gCodeError::InvalidArcParameters;
                }
            theParseResult.motion.trajectory.radius = radius1;
            }
        else if (theBlock.hasRadius)
            // R mode
            {
            radius1 = theState.letterValueState[(uint8_t)gCodeLetter::R];											// get the R lettervalue
            if (theState.modalGroupsState[(uint8_t)modalGroup::Unit] == (uint8_t)modalGroupUnit::G20)						// Inches
                {
                radius1 = radius1 * inches2mm;																		// convert to mm
                }

            if ((4 * radius1 * radius1) < (theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]] * theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]]) - (theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]] * theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]]))
                {
                // error - distance from start to endpoint is more than 2 * radius.. no ARC can be found
                //lastError = gCodeError::InvalidArcParameters;
                }

            if ((theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]] == 0) && (theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]] == 0))
                {
                // error - startpoint is same as endpoint -> undefined arccenter for R mode arcs
                //lastError = gCodeError::InvalidArcParameters;
                }

            double tmpH;															// used in calculation of the arcCenter for radius mode
            double tmpD;															// used in calculation of the arcCenter for radius mode
            double tmpValue = 0.0;													// used in all kinds of calculations to hold intermediate results

            tmpD = sqrt((theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]] * theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]]) + (theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]] * theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]]));
            tmpH = sqrt((4 * radius1 * radius1) - (theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]] * theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]]) - (theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]] * theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]])) * 0.5;
            tmpValue = tmpH / tmpD;				// TODO : Catch Divide by Zero...

            // now set tmpValue sign according to G2/G3 or sign of Radius
            if (MotionType::FeedHelicalCW == theParseResult.motion.theMotionType)
                {
                tmpValue = -tmpValue;
                }

            if (radius1 < 0)
                {
                tmpValue = -tmpValue;
                radius1 = -radius1;
                }

            theParseResult.motion.trajectory.arcCenter[0] = theState.currentPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]] + ((theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]] * 0.5f) - (theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]] * tmpValue));
            theParseResult.motion.trajectory.arcCenter[1] = theState.currentPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]] + ((theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]] * 0.5f) + (theParseResult.motion.trajectory.delta[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]] * tmpValue));
            theParseResult.motion.trajectory.radius = radius1;
            }
        else
            {
            // Error : no offsets AND no radius..
            //lastError = gCodeError::InvalidArcParameters;
            }

        //	2.3 Determine start-, end- and deltaAngle of the ARC
        double endAngle = 0.0;												// [rad]

        theParseResult.motion.trajectory.startAngle = atan2(theState.currentPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]] - theParseResult.motion.trajectory.arcCenter[1], theState.currentPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]] - theParseResult.motion.trajectory.arcCenter[0]);
        endAngle = atan2(theState.nextPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[1]] - theParseResult.motion.trajectory.arcCenter[1], theState.nextPosition[(uint8_t)theParseResult.motion.trajectory.arcAxis[0]] - theParseResult.motion.trajectory.arcCenter[0]);

        if (MotionType::FeedHelicalCW == theParseResult.motion.theMotionType)
            {
            theParseResult.motion.trajectory.deltaAngle = theParseResult.motion.trajectory.startAngle - endAngle;
            if (theParseResult.motion.trajectory.deltaAngle < 0)
                {
                theParseResult.motion.trajectory.deltaAngle = theParseResult.motion.trajectory.deltaAngle + 2 * pi;
                }
            theParseResult.motion.trajectory.deltaAngle = -theParseResult.motion.trajectory.deltaAngle;
            }
        else
            {
            theParseResult.motion.trajectory.deltaAngle = endAngle - theParseResult.motion.trajectory.startAngle;
            if (theParseResult.motion.trajectory.deltaAngle < 0)
                {
                theParseResult.motion.trajectory.deltaAngle = theParseResult.motion.trajectory.deltaAngle + 2 * pi;
                }
            }

        //	2.4 Determine length of the ARC
        arcLength = radius1 * fabs(theParseResult.motion.trajectory.deltaAngle);
        }


    // 	#############################
    //	3.	Length of the motion [mm]
    // 	#############################

    theParseResult.motion.trajectory.length = arcLength * arcLength;												// start with length of arc, could be zero for linear move
    for (uint8_t i = 0; i < (uint8_t)axis::nmbrAxis; ++i)						// iterate over all axis
        {
        if ((i != (uint8_t)theParseResult.motion.trajectory.arcAxis[0]) && (i != (uint8_t)theParseResult.motion.trajectory.arcAxis[1]))			// for all axis, except for the 2 of the arcPlane
            {
            theParseResult.motion.trajectory.length = theParseResult.motion.trajectory.length + (theParseResult.motion.trajectory.delta[i] * theParseResult.motion.trajectory.delta[i]);							// add square length
            }
        }
    theParseResult.motion.trajectory.length = sqrt(theParseResult.motion.trajectory.length);														//


    // 	####################################################
    //	4.	speed [mm/s] and/or Duration of the motion [s]
    // 	####################################################

    // Calculate Duration of the Motion :
    // G0, G1, G2, G3 : derive it from length and feedrate
    // G4 : duration is given directly, length = 0
    double tmpValue = 0.0;																			// used in all kinds of calculations to hold intermediate results

    if (MotionType::Traverse == theParseResult.motion.theMotionType)
        {
        theParseResult.motion.speedProfile.vFeed = largeValue;													// [mm/s] - G93 F indicates: complete the motion in 1/F minutes,= 60 / F seconds
        theParseResult.motion.speedProfile.duration = theParseResult.motion.trajectory.length / theParseResult.motion.speedProfile.vFeed;
        }
    else if ((MotionType::FeedLinear == theParseResult.motion.theMotionType) ||
             (MotionType::FeedHelicalCW == theParseResult.motion.theMotionType) ||
             (MotionType::FeedHelicalCCW == theParseResult.motion.theMotionType))
        {
        tmpValue = theState.letterValueState[(uint8_t)gCodeLetter::F];								// Note - check for valid feedrate is done when processing the F-word
        if (tmpValue <= 0)																			// here we check for the scenario where the F-word is missing, so F is still to its default of zero
            {
            //lastError = gCodeError::InvalidFeedrate;
            }
        if (theState.modalGroupsState[(uint8_t)modalGroup::FeedRate] == (uint8_t)modalGroupFeedRate::G93)	// Inverse Feedrate
            {
            theParseResult.motion.speedProfile.vFeed = (theParseResult.motion.trajectory.length * tmpValue) / 60.0;						// [mm/s] - G93 F indicates: complete the motion in 1/F minutes,= 60 / F seconds
            theParseResult.motion.speedProfile.duration = theParseResult.motion.trajectory.length / theParseResult.motion.speedProfile.vFeed;
            }
        else
            {
            if (theState.modalGroupsState[(uint8_t)modalGroup::Unit] == (uint8_t)modalGroupUnit::G20)		// Inches
                {
                tmpValue = tmpValue * inches2mm;													// convert to mm
                }
            theParseResult.motion.speedProfile.vFeed = tmpValue / 60.0;												// [mm/s] - G94 F indicates: do the motion at F mm (or inches) per minute.
            theParseResult.motion.speedProfile.duration = theParseResult.motion.trajectory.length / theParseResult.motion.speedProfile.vFeed;
            }
        }
    else
        {
        theParseResult.motion.speedProfile.vFeed = 0.0;
        theParseResult.motion.speedProfile.duration = theState.letterValueState[(uint8_t)gCodeLetter::P];								// Get the duration of the Dwell from P letter value
        }

    // 	######################################
    //	5.	Spindle and Coolant
    // 	######################################

    theParseResult.motion.peripherals.spindleSpeed = 0;
    theParseResult.motion.peripherals.spindledirection = rotationDirection::Clockwise;
    if (theState.modalGroupsState[(uint8_t)modalGroup::Spindle] != (uint8_t)modalGroupSpindle::M5)
        {
        theParseResult.motion.peripherals.spindleSpeed = theState.letterValueState[(uint8_t)gCodeLetter::S];
        if (theState.modalGroupsState[(uint8_t)modalGroup::Spindle] != (uint8_t)modalGroupSpindle::M4)
            {
            theParseResult.motion.peripherals.spindledirection = rotationDirection::Clockwise;
            }
        else
            {
            theParseResult.motion.peripherals.spindledirection = rotationDirection::CounterClockwise;
            }
        }

    theParseResult.motion.peripherals.coolantFlood = (theState.modalGroupsState[(uint8_t)modalGroup::CoolantFlood] == (uint8_t)modalGroupCoolantFlood::M7);
    theParseResult.motion.peripherals.coolantMist = (theState.modalGroupsState[(uint8_t)modalGroup::CoolantMist] == (uint8_t)modalGroupCoolantMist::M8);

    // Ready with all the gCode calculations for this Motion. Push it on the motionBuffer and do further machine-dependent stuff in Motion::append()

    //theMotion.append(type, trajectory, speedProfile, peripherals);

    // 	######################################
    //	6.	Update Actual gCode position [mm]
    // 	######################################
    // Update gCode position : after this move, targetPosition becomes now new currentPosition

    for (uint8_t i = 0; i < (uint8_t)axis::nmbrAxis; ++i)
        {
        theState.currentPosition[i] = theState.nextPosition[i];
        }
    }

void gCode::saveState()
    {
    }

void gCode::restoreState()
    {
    }


void gCodeParserResult::toString()
    {
    logger.logNow("gCodeParserResult:\n");
    switch (theParseResultType)
        {
        case gCodeParserResult::ParseResultType::EmptyBlock:
            logger.logNow(" EmptyBlock\n");
            break;
        case gCodeParserResult::ParseResultType::OkContextUpdateOnly:
            logger.logNow(" OkContextUpdateOnly\n");
            break;
        case gCodeParserResult::ParseResultType::OkContextUpdateAndMotion:
            logger.logNow(" OkContextUpdateAndMotion\n");

            switch (motion.theMotionType)
                {
                case MotionType::Traverse:
                    logger.logNow(" Traverse\n");
                    break;
                case MotionType::FeedLinear:
                    logger.logNow(" FeedLinear\n");
                    break;
                case MotionType::FeedHelicalCW:
                    logger.logNow(" FeedHelicalCW\n");
                    break;
                case MotionType::FeedHelicalCCW:
                    logger.logNow(" FeedHelicalCCW\n");
                    break;
                case MotionType::PauseAndResume:
                    logger.logNow(" PauseAndResume\n");
                    break;
                case MotionType::Pause:
                    logger.logNow(" Pause\n");
                    break;
                default:
                    logger.logNow("*** ERROR : unknown MotionType ***\n");
                    break;
                }

            switch (motion.theMotionType)
                {
                case MotionType::Traverse:
                case MotionType::FeedLinear:
                case MotionType::FeedHelicalCW:
                case MotionType::FeedHelicalCCW:
                    snprintf(logger.logLine, 127, "  length = %f mm\n", motion.trajectory.length);
                    logger.logNow();
                    snprintf(logger.logLine, 127, "  startPosition : (%f, %f, %f) mm\n", motion.trajectory.startPosition[0], motion.trajectory.startPosition[1], motion.trajectory.startPosition[2]);
                    logger.logNow();
                    snprintf(logger.logLine, 127, "  delta : (%f, %f, %f) mm\n", motion.trajectory.delta[0], motion.trajectory.delta[1], motion.trajectory.delta[2]);
                    logger.logNow();
                    break;
                default:
                    break;
                }

            switch (motion.theMotionType)
                {
                case MotionType::FeedHelicalCW:
                case MotionType::FeedHelicalCCW:
                    snprintf(logger.logLine, 127, "  radius = %f\n", motion.trajectory.radius);
                    logger.logNow();
                    snprintf(logger.logLine, 127, "  startAngle = %f\n", motion.trajectory.startAngle);
                    logger.logNow();
                    snprintf(logger.logLine, 127, "  deltaAngle = %f\n", motion.trajectory.deltaAngle);
                    logger.logNow();
                    break;
                default:
                    break;
                }

            snprintf(logger.logLine, 127, " feedrate : %f mm/s\n", motion.speedProfile.vFeed);
            logger.logNow();
            snprintf(logger.logLine, 127, " duration : %f s\n", motion.speedProfile.duration);
            logger.logNow();

            break;
        case gCodeParserResult::ParseResultType::WarningContextUpdateOnly:
            logger.logNow(" WarningContextUpdateOnly\n");
            break;
        case gCodeParserResult::ParseResultType::WarningContextUpdateAndMotion:
            logger.logNow(" WarningContextUpdateAndMotion\n");
            break;
        case gCodeParserResult::ParseResultType::Error:
            logger.logNow(" Error:");
            switch (error)
                {
                case gCodeParserResult::Error::None:
                    logger.logNow("None\n");
                    break;
                case gCodeParserResult::Error::TooManyWordsInBlock:
                    logger.logNow("TooManyWordsInBlock\n");
                    break;
                case gCodeParserResult::Error::InvalidArcParameters:
                    logger.logNow("InvalidArcParameters\n");
                    break;
                case gCodeParserResult::Error::ModalGroupCollision:
                    logger.logNow("ModalGroupCollision\n");
                    break;
                case gCodeParserResult::Error::ValueWordCollision:
                    logger.logNow("ValueWordCollision\n");
                    break;
                case gCodeParserResult::Error::InvalidLineNumber:
                    logger.logNow("InvalidLineNumber\n");
                    break;
                case gCodeParserResult::Error::InvalidFeedrate:
                    logger.logNow("InvalidFeedrate\n");
                    break;
                case gCodeParserResult::Error::MissingFeedrate:
                    logger.logNow("MissingFeedrate\n");
                    break;
                case gCodeParserResult::Error::InvalidSpindleSpeed:
                    logger.logNow("InvalidSpindleSpeed\n");
                    break;
                case gCodeParserResult::Error::MissingPForG4:
                    logger.logNow("MissingPForG4\n");
                    break;
                case gCodeParserResult::Error::InvalidDwellTime:
                    logger.logNow("InvalidDwellTime\n");
                    break;
                case gCodeParserResult::Error::MissingLForG10:
                    logger.logNow("MissingLForG10\n");
                    break;
                case gCodeParserResult::Error::MissingPForG10:
                    logger.logNow("MissingPForG10\n");
                    break;
                case gCodeParserResult::Error::InvalidPForG10:
                    logger.logNow("InvalidPForG10\n");
                    break;
                case gCodeParserResult::Error::MissingAxisWord:
                    logger.logNow("MissingAxisWord\n");
                    break;
                case gCodeParserResult::Error::MissingOffsetWord:
                    logger.logNow("MissingOffsetWord\n");
                    break;
                case gCodeParserResult::Error::UnsupportedgCodeWord:
                    logger.logNow("UnsupportedgCodeWord\n");
                    break;
                case gCodeParserResult::Error::MOTION_BUFFER_OVERFLOW:
                    logger.logNow("MOTION_BUFFER_OVERFLOW\n");
                    break;
                case gCodeParserResult::Error::SoftLimits:
                    logger.logNow("SoftLimits\n");
                    break;
                case gCodeParserResult::Error::ConfigFileError:
                    logger.logNow("ConfigFileError\n");
                    break;
                default:
                    break;
                }
            break;
        default:
            logger.logNow("*** ERROR : unknown gCodeParserResult::Type ***\n");
            break;
        }
    }

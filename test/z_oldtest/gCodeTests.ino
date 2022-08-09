// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <Arduino.h>
#include <UnitTest.h>
#include "gcode.h"

UnitTest theTest;

void setup()
    {
        {
        theTest.init();
        theTest.start("gCodeWord Unit Tests");
        gCodeWord word;
        theTest.printValue(static_cast<int32_t>(sizeof(word)), "memory footprint of gCodeWord [bytes]");
        // so far this class has no member functions. Could be a struct but I took a class to be future-safe
        }

        {
        theTest.section("gCodeBlock Unit Tests");
        gCodeBlock block;
        theTest.printValue(static_cast<int32_t>(sizeof(block)), "memory footprint of gCodeBlock [bytes]");
        theTest.assert(1, block.nmbrWords == 0, "nmbrBlocks initialized to 0");
        }

        {
        theTest.section("gCodeState Unit Tests");
        gCodeState state;
        theTest.printValue(static_cast<int32_t>(sizeof(state)), "memory footprint of gCodeState [bytes]");
		// save state
		// restore state
        }

        {
        theTest.section("gCodeParserResult Unit Tests");
        gCodeParserResult result;
        theTest.printValue(static_cast<int32_t>(sizeof(result)), "memory footprint of gCodeParserResult [bytes]");
        }

        {
        theTest.section("gCode Parser Unit Tests");
        gCode theParser;
		gCodeParserResult result;
        theTest.printValue(static_cast<int32_t>(sizeof(theParser)), "memory footprint of gCode Parser [bytes]");

		theParser.initialize();
		theParser.getBlock("G21 G17 G54 F1000 G1 X100");
		theTest.assert(40, 2 == theParser.searchWord('G', 540), "find 'G54' in block");
		theTest.assert(41, 5 == theParser.searchWord('X'), "finds 'X100' in block");
		theTest.assert(42, -1 == theParser.searchWord('Y'), "can't find 'Y' in block");
		theTest.assert(43, -1 == theParser.searchWord('G', 530), "can't find 'G53' in block");
		theParser.initialize();
		theTest.assert(44, -1 == theParser.searchWord('G'), "can't find 'G' in empty block");
		theTest.assert(45, -1 == theParser.searchWord('X'), "can't find 'X' in empty block");

		// removing words, test nmbr before and after, remove when not found or when empty


		theParser.initialize();
		theParser.getBlock("G0 G1");
        theParser.parseBlock(result);
		theTest.assert(50, result.error == gCodeParserResult::Error::ModalGroupCollision, "modalGroup Motion Collision");

		theParser.initialize();
		theParser.getBlock("G21 N100 F1000");
		theParser.parseBlock(result);
		theTest.assert(51, result.error == gCodeParserResult::Error::InvalidLineNumber, "invalid line number");

		theParser.initialize();
		theParser.getBlock("G21 G17 G54 F1000 G1 X100");
		theParser.parseBlock(result);
		theTest.assert(52, result.error == gCodeParserResult::Error::None, "no errors");
		theTest.assert(53, result.motion.theMotionType == MotionType::FeedLinear, "check motionType");
		theTest.assert(54, result.motion.trajectory.length == 100.0, "lenght");
		theTest.assert(55, result.motion.speedProfile.duration == 6.0, "duration");

		//int8_t searchWord(uint8_t, uint32_t) const;							// does the gCodeWords array contain a word with letter / number ? If found, returns index into gCodeWords[]
		//void removeWord(uint8_t);											// removes a word [x] from gCodeWords[] by swapping it with the last one and decrementing nmbrWords
		//void countModalGroupAndOthers(uint8_t);								// Keep count of how many words from each ModalGroup are found, also sets hasAxis, hasOffset, hasRadius flags



		result.print();
		//calcMotion(gCodeParserResult &theParseResult)
		theTest.end();
        }
    }

void loop()
    {
    }


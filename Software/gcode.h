// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#ifndef  WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif

#include "general.h"
#include "gcodegeneral.h"
#include "gcodeword.h"
#include "gcodeblock.h"
#include "gcodestate.h"
#include "gcodeparseresult.h"


class gCode
    {
    public:
        gCode();															// Constructor

        void initialize();													// (Re-)initialize the parser
        void saveState();													// Save the state of the Parser, so we can do some interim stuff and afterwards restore the state back to what is was..
        void restoreState();												// Restore the state from previous saveState

        void getBlock(const char *);										// Read a line of input and process as gCode block - This is the input of this module.. it receives a c-style zero terminated string
        void parseBlock(gCodeParserResult &theParseResult);					// Process all words in block, update gCode context and spawn a motion if the block contains one..
        void calcMotion(gCodeParserResult &theParseResult);					// calculate all the details for a motion resulting from a G0, G1, G2, G3, G4 block..

        uint32_t getNmbrWords(void) const;									// return number of actually used words in the block

//	private:
		static constexpr double pi{ 3.141592653589793238463 };				// constant for calculations in radians
		static constexpr double inches2mm{ 25.4 };							// 25.4 mm/inch

		char gCodeLine[256];

		gCodeBlock theBlock;
		gCodeState theState;												// gCode parser context : current position, value of all modal groups, etc.

		int32_t searchWord(uint8_t, uint32_t = 0) const;					// does the gCodeWords array contain a word with letter / number ? If found, returns index into gCodeWords[]
		void removeWord(int32_t);											// removes a word [x] from gCodeWords[] by swapping it with the last one and decrementing nmbrWords
		void countModalGroupAndOthers(uint8_t);								// Keep count of how many words from each ModalGroup are found, also sets hasAxis, hasOffset, hasRadius flags
    };


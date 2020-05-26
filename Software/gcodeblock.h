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
#include "gcodeword.h"
#include "gcodegeneral.h"

struct gCodeBlock
    {
    public:

        void initialize();

        static constexpr uint32_t MaxGCodeWordsPerGCodeBlock = 16;			// Size of the array of gCode Words - max 127 as it's indexed by (signed!) int8_t, The reason is that gCode::searchWord returns -1 (a signed int8_t) in case it
        gCodeWord gCodeWords[MaxGCodeWordsPerGCodeBlock];					// array holding all gCodeWords of one block
        uint32_t nmbrWords = 0;												// number of actually used words in the block, so nmbrWords-1 is last element used in the array
        bool hasAxis, hasOffset, hasRadius;									// remembers if the block has any ABCXYZ, IJK or R words
        uint8_t modalGroupCount[(uint8_t) modalGroup::nmbrModalGroups];		// counts how many words from each modal group as found - needed to check modalgroup violations
        uint8_t valueWordCount[(uint8_t) gCodeLetter::nmbrLetters];			// counts how many words from each axis, as all these letters should appear maximum once in a block
    };

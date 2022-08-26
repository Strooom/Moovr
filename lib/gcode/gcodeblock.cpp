// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <math.h>
#include <stdlib.h>
#include "gcodeblock.h"

void gCodeBlock::initialize() {
    nmbrWords   = 0;        // reset counter for counting words inside a block
    nmbrAxis    = 0;        // X Y Z A B C
    nmbrOffsets = 0;        // I J K
    nmbrRadius  = 0;        // R
    // reset counts for words part of modalGroups. Only maximum one G- or M-code for each of these blocks is allowed
    for (uint32_t i = 0; i < (uint8_t)modalGroup::nmbrModalGroups; ++i) {
        modalGroupCount[i] = 0;
    }
    // reset counts for all non G and M words, called valueWords
    for (uint32_t i = 0; i < (uint8_t)gCodeLetter::nmbrLetters; ++i) {
        valueWordCount[i] = 0;
    }
}

void gCodeBlock::getBlockFromString(const uint8_t *gCodeAsString) {
    uint32_t currentByte;
    uint32_t readPos  = 0;
    uint32_t writePos = 0;
    bool inComment    = false;
    char number[16]   = "";
    uint8_t writePos2 = 0;

    initialize();

    // 1. Strip all Whitespace and Comments
    while ((currentByte = gCodeAsString[readPos]))        // read new byte from source for as long as they are not zero, which indicated rxBuffer underrun
    {
        if ('\n' == currentByte)        // if linefeed...
        {
            break;        // ... is 'end of line' -> break out of loop ...
        }
        switch (currentByte) {
            case '(':                    // detect beginning of a comment
                inComment = true;        // remember by setting flag
                break;

            case ')':                     // detect end of a comment
                inComment = false;        // reset flag
                break;

            case ' ':         // strip spaces
            case '\t':        // strip TABs
                break;

            default:
                if (!inComment)        // if NOT inside comment...
                {
                    if ((currentByte >= 'a') && (currentByte <= 'z'))        // if any lowercase...
                    {
                        currentByte -= ('a' - 'A');        // ... convert to uppercase to simplify parsing later-on
                    }
                    gCodeLine[writePos] = currentByte;        // ... write to output
                    writePos++;                               // move to next position in destination
                }
                break;
        }
        ++readPos;
    }
    gCodeLine[writePos] = 0;        // Terminate

    // 2. Now parse the line of text and convert into an array of gCodeWords
    readPos = 0;
    while (gCodeLine[readPos] && (nmbrWords < MaxGCodeWordsPerGCodeBlock)) {
        // 1. Proceed to next A-Z
        if (!(('A' <= gCodeLine[readPos]) && ('Z' >= gCodeLine[readPos]))) {
            ++readPos;
        } else {
            // Get the letter..
            gCodeWords[nmbrWords].letter = gCodeLine[readPos];
            // 2. Then after the 'letter', expect the number
            // For strtod to work 100% reliable, we need to really isolate the number part into a separate string...
            ++readPos;
            writePos2 = 0;
            while (gCodeLine[readPos] && (writePos2 < 15) && (!(('A' <= gCodeLine[readPos]) && ('Z' >= gCodeLine[readPos])))) {
                number[writePos2] = gCodeLine[readPos];
                ++readPos;
                ++writePos2;
            }
            number[writePos2] = 0;        // Terminate the new number string

            gCodeWords[nmbrWords].number = strtod(number, NULL);

            if (('G' == gCodeWords[nmbrWords].letter) || ('M' == gCodeWords[nmbrWords].letter)) {
                gCodeWords[nmbrWords].intNumber = (uint32_t)(gCodeWords[nmbrWords].number * 10);
            } else {
                gCodeWords[nmbrWords].intNumber = 0;
            }
            countModalGroupAndOthers(nmbrWords);

            ++nmbrWords;
        }
    }
}

void gCodeBlock::getBlockFromString(const char *gCodeAsString) {
    getBlockFromString(reinterpret_cast<const uint8_t *>(gCodeAsString));
};

uint32_t gCodeBlock::getNmbrWords() const {
    return nmbrWords;
}

bool gCodeBlock::hasWords() const {
    return (getNmbrWords() > 0);
}

int32_t gCodeBlock::searchWord(uint8_t aLetter, uint32_t anIntNumber) const {        // if (('G' == aLetter) || ('M' == aLetter))        // for G and M
    for (uint32_t i = 0; i < nmbrWords; ++i) {
        if ((gCodeWords[i].letter == aLetter) && (gCodeWords[i].intNumber == anIntNumber)) {
            return i;
        }
    }
    return -1;
}

int32_t gCodeBlock::searchWord(uint8_t aLetter) const {
    for (uint32_t i = 0; i < nmbrWords; ++i) {
        if (gCodeWords[i].letter == aLetter) {
            return i;
        }
    }
    return -1;
}

void gCodeBlock::removeWord(int32_t indexToBeRemoved) {
    if ((indexToBeRemoved >= 0) && (indexToBeRemoved < (int32_t)nmbrWords))        // only if index is within meaningful range
    {
        if (nmbrWords > 1U)        // if it's not the last word, swap the last item with the to be removed item
        {
            gCodeWords[indexToBeRemoved].letter    = gCodeWords[nmbrWords - 1].letter;
            gCodeWords[indexToBeRemoved].number    = gCodeWords[nmbrWords - 1].number;
            gCodeWords[indexToBeRemoved].intNumber = gCodeWords[nmbrWords - 1].intNumber;
        }
        --nmbrWords;
    }
}

void gCodeBlock::countModalGroupAndOthers(uint32_t i) {
    if ('G' == gCodeWords[i].letter) {
        switch (gCodeWords[i].intNumber) {
            case 0U:
            case 10U:
            case 20U:
            case 30U:
                ++modalGroupCount[(uint32_t)modalGroup::Motion];
                break;

            case 170U:
            case 180U:
            case 190U:
                ++modalGroupCount[(uint32_t)modalGroup::Plane];
                break;

            case 900U:
            case 910U:
                ++modalGroupCount[(uint32_t)modalGroup::Distance];
                break;

            case 200U:
            case 210U:
                ++modalGroupCount[(uint32_t)modalGroup::Unit];
                break;

            case 930U:
            case 940U:
                ++modalGroupCount[(uint32_t)modalGroup::FeedRate];
                break;

            case 540U:
            case 550U:
            case 560U:
            case 570U:
            case 580U:
            case 590U:
                ++modalGroupCount[(uint32_t)modalGroup::CoordinateSet];
                break;

            case 40U:
                ++modalGroupCount[(uint32_t)modalGroup::Motion];
                ++modalGroupCount[(uint32_t)modalGroup::Group0];
                break;

            case 100U:
            case 280U:
            case 300U:
                ++modalGroupCount[(uint32_t)modalGroup::Group0];
                break;

            case 530U:
                ++modalGroupCount[(uint32_t)modalGroup::CoordinateSet];
                ++modalGroupCount[(uint32_t)modalGroup::Group0];
                break;
        }
    } else if ('M' == gCodeWords[i].letter) {
        switch (gCodeWords[i].intNumber) {
            case 30U:
            case 40U:
            case 50U:
                ++modalGroupCount[(uint32_t)modalGroup::Spindle];
                break;

            case 70U:
                ++modalGroupCount[(uint32_t)modalGroup::CoolantMist];
                break;

            case 80U:
                ++modalGroupCount[(uint32_t)modalGroup::CoolantFlood];
                break;

            case 90U:
                ++modalGroupCount[(uint32_t)modalGroup::CoolantFlood];
                ++modalGroupCount[(uint32_t)modalGroup::CoolantMist];
                break;
        }
    } else {
        switch (gCodeWords[i].letter) {
            // TODO, add all other valueLetters
            case 'F':
                ++valueWordCount[(uint32_t)gCodeLetter::F];
                break;
            case 'L':
                ++valueWordCount[(uint32_t)gCodeLetter::L];
                break;
            case 'N':
                ++valueWordCount[(uint32_t)gCodeLetter::N];
                break;
            case 'P':
                ++valueWordCount[(uint32_t)gCodeLetter::P];
                break;
            case 'R':
                ++valueWordCount[(uint32_t)gCodeLetter::R];
                ++nmbrRadius;
                break;
            case 'S':
                ++valueWordCount[(uint32_t)gCodeLetter::S];
                break;
            case 'X':
                ++valueWordCount[(uint32_t)gCodeLetter::X];
                ++nmbrAxis;
                break;
            case 'Y':
                ++valueWordCount[(uint32_t)gCodeLetter::Y];
                ++nmbrAxis;
                break;
            case 'Z':
                ++valueWordCount[(uint32_t)gCodeLetter::Z];
                ++nmbrAxis;
                break;
            case 'I':
                ++valueWordCount[(uint32_t)gCodeLetter::I];
                ++nmbrOffsets;
                break;
            case 'J':
                ++valueWordCount[(uint32_t)gCodeLetter::J];
                ++nmbrOffsets;
                break;
            case 'K':
                ++valueWordCount[(uint32_t)gCodeLetter::K];
                ++nmbrOffsets;
                break;
        }
    }
}

bool gCodeBlock::hasModalGroupViolation() const {
    for (uint32_t i = 0; i < (uint8_t)modalGroup::nmbrModalGroups; ++i) {
        if (modalGroupCount[i] > 1) {
            return true;
        }
    }
    return false;
}

bool gCodeBlock::hasLetterWordViolation() const {
    for (uint32_t i = 0; i < (uint8_t)gCodeLetter::nmbrLetters; ++i) {
        if (valueWordCount[i] > 1) {
            return true;
        }
    }
    return false;
}

bool gCodeBlock::hasAxis() const {
    return (nmbrAxis > 0);
}

bool gCodeBlock::hasOffset() const {
    return (nmbrOffsets > 0);
}

bool gCodeBlock::hasRadius() const {
    return (nmbrRadius > 0);
}

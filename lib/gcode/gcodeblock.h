// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "gcodemodalgroup.h"
#include "gcodeletter.h"
#include "gcodeword.h"

struct gCodeBlock {
  public:
    void initialize();

    static constexpr uint32_t MaxGCodeWordsPerGCodeBlock = 16;            // Size of the array of gCode Words - max 127 as it's indexed by (signed!) int8_t, The reason is that gCode::searchWord returns -1 (a signed int8_t) in case it
    gCodeWord gCodeWords[MaxGCodeWordsPerGCodeBlock];                     // array holding all gCodeWords of one block
    uint8_t modalGroupCount[(uint8_t)modalGroup::nmbrModalGroups];        // counts how many words from each modal group as found - needed to check modalgroup violations
    uint8_t valueWordCount[(uint8_t)gCodeLetter::nmbrLetters];            // counts how many words from each axis, as all these letters should appear maximum once in a block

    void getBlockFromString(const uint8_t *gCodeAsString);
    void getBlockFromString(const char *gCodeAsString);
    uint32_t getNmbrWords() const;
    bool hasWords() const;

    friend class gCode;        // gCode (parser) is allowed to access privates of gCodeBlock

    bool hasAxis() const;          // remembers if the block has any ABCXYZ,
    bool hasOffset() const;        // remembers if the block has any IJK words
    bool hasRadius() const;        // remembers if the block has an R word

    bool hasModalGroupViolation() const;
    bool hasLetterWordViolation() const;

#ifndef unitTesting
  private:
#endif
    friend class gCode;                                  // gCode parser is allowed to access privates of gCodeBlock
    uint32_t nmbrAxis, nmbrOffsets, nmbrRadius;        // counts the numbers of ABCXYZ, IJK and R words
    uint32_t nmbrWords{0};                               // number of actually used words in the block, so nmbrWords-1 is last element used in the array
    char gCodeLine[256]{};

    int32_t searchWord(uint8_t aLetter, uint32_t anIntNumber) const;        // does the gCodeWords array contain a word with letter / number ? If found, returns index into gCodeWords[], else returns -1
    int32_t searchWord(uint8_t aLetter) const;                              // does the gCodeWords array contain a word with letter / number ? If found, returns index into gCodeWords[], else returns -1
    void removeWord(int32_t indexToBeRemoved);
    void countModalGroupAndOthers(uint32_t i);
};

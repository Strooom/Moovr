#include "gcodeparseerror.h"

const char* toString(gCodeParseError anError) {
    switch (anError) {
        case gCodeParseError::None:
            return "None";
            break;

        case gCodeParseError::TooManyWordsInBlock:
            return "TooManyWordsInBlock";
            break;

        case gCodeParseError::InvalidArcParameters:
            return "InvalidArcParameters";
            break;

        case gCodeParseError::ModalGroupCollision:
            return "ModalGroupCollision";
            break;

        case gCodeParseError::ValueWordCollision:
            return "ValueWordCollision";
            break;

        case gCodeParseError::InvalidLineNumber:
            return "InvalidLineNumber";
            break;

        case gCodeParseError::InvalidFeedrate:
            return "InvalidFeedrate";
            break;

        case gCodeParseError::MissingFeedrate:
            return "MissingFeedrate";
            break;

        case gCodeParseError::InvalidSpindleSpeed:
            return "InvalidSpindleSpeed";
            break;

        case gCodeParseError::MissingPForG4:
            return "MissingPForG4";
            break;

        case gCodeParseError::InvalidDwellTime:
            return "InvalidDwellTime";
            break;

        case gCodeParseError::MissingLForG10:
            return "MissingLForG10";
            break;

        case gCodeParseError::MissingPForG10:
            return "MissingPForG10";
            break;

        case gCodeParseError::InvalidPForG10:
            return "InvalidPForG10";
            break;

        case gCodeParseError::MissingAxisWord:
            return "MissingAxisWord";
            break;

        case gCodeParseError::MissingOffsetWord:
            return "MissingOffsetWord";
            break;

        case gCodeParseError::UnsupportedgCodeWord:
            return "UnsupportedgCodeWord";
            break;

        default:
            return "unknown";
            break;
    }
}

#include "gcodeparseresult.h"

void gCodeParserResult::print(char* destBuffer)
    {
    //strcat(destBuffer, "gCodeParserResult:\n");
    //switch (theParseResultType)
    //    {
    //    case gCodeParserResult::ParseResultType::EmptyBlock:
    //        strcat(destBuffer, " EmptyBlock\n");
    //        break;
    //    case gCodeParserResult::ParseResultType::OkContextUpdateOnly:
    //        strcat(destBuffer, " OkContextUpdateOnly\n");
    //        break;
    //    case gCodeParserResult::ParseResultType::OkContextUpdateAndMotion:
    //        strcat(destBuffer, " OkContextUpdateAndMotion\n");

    //        switch (motion.theMotionType)
    //            {
    //            case MotionType::Traverse:
    //                strcat(destBuffer, " Traverse\n");
    //                break;
    //            case MotionType::FeedLinear:
    //                strcat(destBuffer, " FeedLinear\n");
    //                break;
    //            case MotionType::FeedHelicalCW:
    //                strcat(destBuffer, " FeedHelicalCW\n");
    //                break;
    //            case MotionType::FeedHelicalCCW:
    //                strcat(destBuffer, " FeedHelicalCCW\n");
    //                break;
    //            case MotionType::PauseAndResume:
    //                strcat(destBuffer, " PauseAndResume\n");
    //                break;
    //            case MotionType::Pause:
    //                strcat(destBuffer, " Pause\n");
    //                break;
    //            default:
    //                strcat(destBuffer, "*** ERROR : unknown MotionType ***\n");
    //                break;
    //            }

    //        switch (motion.theMotionType)
    //            {
    //            case MotionType::Traverse:
    //            case MotionType::FeedLinear:
    //            case MotionType::FeedHelicalCW:
    //            case MotionType::FeedHelicalCCW:
    //                strcat(destBuffer, "  length = ");
    //                strcat(destBuffer, motion.trajectory.length);
    //                strcat(destBuffer, " mm\n");

    //                strcat(destBuffer, "   startPosition : ");
    //                for (uint8_t x = 0; x < nmbrAxis; x++)
    //                    {
    //                    strcat(destBuffer, motion.trajectory.startPosition[x]);
    //                    strcat(destBuffer, "  ");
    //                    }
    //                strcat(destBuffer, "mm\n");

    //                strcat(destBuffer, "   delta : ");
    //                for (uint8_t x = 0; x < nmbrAxis; x++)
    //                    {
    //                    strcat(destBuffer, motion.trajectory.delta[x]);
    //                    strcat(destBuffer, "  ");
    //                    }
    //                strcat(destBuffer, "mm\n");
    //                break;
    //            default:
    //                break;
    //            }

    //        switch (motion.theMotionType)
    //            {
    //            case MotionType::FeedHelicalCW:
    //            case MotionType::FeedHelicalCCW:
    //                strcat(destBuffer, "  radius = ");
    //                strcat(destBuffer, motion.trajectory.radius);
    //                strcat(destBuffer, " mm\n");
    //                strcat(destBuffer, "  startAngle = ");
    //                strcat(destBuffer, motion.trajectory.startAngle);
    //                strcat(destBuffer, " ??\n");
    //                strcat(destBuffer, "  deltaAngle = ");
    //                strcat(destBuffer, motion.trajectory.deltaAngle);
    //                strcat(destBuffer, " ??\n");
    //                break;
    //            default:
    //                break;
    //            }

    //        strcat(destBuffer, "  feedrate = ");
    //        strcat(destBuffer, motion.speedProfile.vFeed);
    //        strcat(destBuffer, " mm/s\n");
    //        strcat(destBuffer, "  duration = ");
    //        strcat(destBuffer, motion.speedProfile.duration);
    //        strcat(destBuffer, " s\n");

    //        break;
    //    case gCodeParserResult::ParseResultType::WarningContextUpdateOnly:
    //        strcat(destBuffer, " WarningContextUpdateOnly\n");
    //        break;
    //    case gCodeParserResult::ParseResultType::WarningContextUpdateAndMotion:
    //        strcat(destBuffer, " WarningContextUpdateAndMotion\n");
    //        break;
    //    case gCodeParserResult::ParseResultType::Error:
    //        strcat(destBuffer, " Error:");
    //        switch (error)
    //            {
    //            case gCodeParserResult::Error::None:
    //                strcat(destBuffer, "None\n");
    //                break;
    //            case gCodeParserResult::Error::TooManyWordsInBlock:
    //                strcat(destBuffer, "TooManyWordsInBlock\n");
    //                break;
    //            case gCodeParserResult::Error::InvalidArcParameters:
    //                strcat(destBuffer, "InvalidArcParameters\n");
    //                break;
    //            case gCodeParserResult::Error::ModalGroupCollision:
    //                strcat(destBuffer, "ModalGroupCollision\n");
    //                break;
    //            case gCodeParserResult::Error::ValueWordCollision:
    //                strcat(destBuffer, "ValueWordCollision\n");
    //                break;
    //            case gCodeParserResult::Error::InvalidLineNumber:
    //                strcat(destBuffer, "InvalidLineNumber\n");
    //                break;
    //            case gCodeParserResult::Error::InvalidFeedrate:
    //                strcat(destBuffer, "InvalidFeedrate\n");
    //                break;
    //            case gCodeParserResult::Error::MissingFeedrate:
    //                strcat(destBuffer, "MissingFeedrate\n");
    //                break;
    //            case gCodeParserResult::Error::InvalidSpindleSpeed:
    //                strcat(destBuffer, "InvalidSpindleSpeed\n");
    //                break;
    //            case gCodeParserResult::Error::MissingPForG4:
    //                strcat(destBuffer, "MissingPForG4\n");
    //                break;
    //            case gCodeParserResult::Error::InvalidDwellTime:
    //                strcat(destBuffer, "InvalidDwellTime\n");
    //                break;
    //            case gCodeParserResult::Error::MissingLForG10:
    //                strcat(destBuffer, "MissingLForG10\n");
    //                break;
    //            case gCodeParserResult::Error::MissingPForG10:
    //                strcat(destBuffer, "MissingPForG10\n");
    //                break;
    //            case gCodeParserResult::Error::InvalidPForG10:
    //                strcat(destBuffer, "InvalidPForG10\n");
    //                break;
    //            case gCodeParserResult::Error::MissingAxisWord:
    //                strcat(destBuffer, "MissingAxisWord\n");
    //                break;
    //            case gCodeParserResult::Error::MissingOffsetWord:
    //                strcat(destBuffer, "MissingOffsetWord\n");
    //                break;
    //            case gCodeParserResult::Error::UnsupportedgCodeWord:
    //                strcat(destBuffer, "UnsupportedgCodeWord\n");
    //                break;
    //            case gCodeParserResult::Error::MOTION_BUFFER_OVERFLOW:
    //                strcat(destBuffer, "MOTION_BUFFER_OVERFLOW\n");
    //                break;
    //            case gCodeParserResult::Error::SoftLimits:
    //                strcat(destBuffer, "SoftLimits\n");
    //                break;
    //            case gCodeParserResult::Error::ConfigFileError:
    //                strcat(destBuffer, "ConfigFileError\n");
    //                break;
    //            default:
    //                break;
    //            }
    //        break;
    //    default:
    //        strcat(destBuffer, "*** ERROR : unknown gCodeParserResult::Type ***\n");
    //        break;
    //    }
    //strcat(destBuffer, ("\n");
    }

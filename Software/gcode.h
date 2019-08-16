// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#ifndef gCode_h
#define gCode_h

#ifndef WIN32
#include <Arduino.h>
#else
#include <inttypes.h>
#include <cstdlib>
#endif


enum class axis : uint8_t
    {
    X, Y, Z,											///< need a minimum of 3 axis (because of G17 G18 G19)
    //	A, B, C,										///< could go to 6 axis (in fact in software it could go up to 16 axis, but the Teensy3.x only has pins for 6 axis)
    nmbrAxis											///< axis::nmbrAxis can be used to loop over all axis
    };
enum class arcPlane : uint8_t { XY, YZ, ZX, nmbrArcPlanes };
enum class rotationDirection : uint8_t { Clockwise, CounterClockwise, nmbrRotationDirections };

enum class gCodeLetter : uint8_t
    {
    //	All AXIS letters are put at the top, in the SAME order as in 'axis'. This way we can use loops to copy gCode letterValues to axis..
    X, Y, Z,
    //	A, B, C,
    //	D, E,
    //	U, V, W,

    //	After the AXI letters, I put the OFFSET letters, so we can easily loop over those as well..
    I, J, K,
    L,
    F,
    //	G, H,
    //	M,
    N,
    //	O,
    P,
    //	Q,
    R, S, T,
    nmbrLetters
    };

enum class modalGroup : uint8_t
    {
    Group0,
    Motion,
    Plane,
    Distance,
    Spindle,
    Unit,
    FeedRate,
    CoordinateSet,
    CoolantFlood,
    CoolantMist,
    ManualOverride,
    // ArcDistance,
    // CutterCompensation,
    // ToolLengthOffset,
    // CannedCycles
    // Coolant
    // ProgramFlow
    nmbrModalGroups
    };

enum class modalGroup0 : uint8_t
    {
    G4, G10, G28, G30, G53,
    //  G92, G92_1, G92_2, G92_3
    nmbrModes
    };

enum class modalGroupMotion : uint8_t
    {
    G0, G1, G2, G3,
    nmbrModes
    };

enum class modalGroupFeedRate : uint8_t
    {
    G94, G93,
    nmbrModes
    };

enum class modalGroupSpindle : uint8_t
    {
    M5, M3, M4,
    nmbrModes
    };

enum class modalGroupUnit : uint8_t
    {
    G21, G20,
    nmbrModes
    };

enum class modalGroupPlane : uint8_t
    {
    G17, G18, G19,
    nmbrModes
    };

enum class modalGroupDistance : uint8_t
    {
    G90, G91,
    nmbrModes
    };

enum class modalGroupCoordinateSet : uint8_t
    {
    G53, G54, G55, G56, G57, G58, G59, G59_1, G59_2, G59_3,
    nmbrModes
    };

enum class modalGroupOverrides : uint8_t
    {
    M48, M49,
    nmbrModes
    };

enum class modalGroupCoolantFlood : uint8_t
    {
    M9, M7,
    nmbrModes
    };

enum class modalGroupCoolantMist : uint8_t
    {
    M9, M8,
    nmbrModes
    };

struct gCodeWord
    {
    uint8_t letter;						// gCode letter,eg 'G', 'M', 'X', etc
    double number;						// value after the letter
    uint32_t intNumber;					// for G or M letter words, contains number * 10, as integer , eg G38.1 holds 381, this simplifies lookup via switch statements
    };

struct gCodeBlock
    {
    static constexpr uint8_t MaxGCodeWordsPerGCodeBlock = 16;			// Size of the array of gCode Words - max 127 as it's indexed by (signed!) int8_t, The reason is that gCode::searchWord returns -1 (a signed int8_t) in case it
    gCodeWord gCodeWords[MaxGCodeWordsPerGCodeBlock];					// array holding all gCodeWords of one block
    uint8_t nmbrWords = 0;												// number of actually used words in the block, so nmbrWords-1 is last element used in the array
    bool hasAxis, hasOffset, hasRadius;									// remembers if the block has any ABCXYZ, IJK or R words
    uint8_t modalGroupCount[(uint8_t)modalGroup::nmbrModalGroups];		// counts how many words from each modal group as found - needed to check modalgroup violations
    uint8_t valueWordCount[(uint8_t)gCodeLetter::nmbrLetters];			// counts how many words from each axis, as all these letters should appear maximum once in a block
    };

struct gCodeState
    {
    double WCSorigin[(uint8_t)modalGroupCoordinateSet::nmbrModes][(uint8_t)axis::nmbrAxis];
    uint8_t modalGroupsState[(uint8_t)modalGroup::nmbrModalGroups];		// Array to hold all model groups settings
    double letterValueState[(uint8_t)gCodeLetter::nmbrLetters];			// Array to hold value for each used gCode letter
    double currentPosition[(uint8_t)axis::nmbrAxis];					// current position [mm or degrees], before the motion
    double nextPosition[(uint8_t)axis::nmbrAxis];						// future position [mm or degrees], after the motion
    };

enum class MotionType : uint8_t
    {
    Traverse,			// 3D Linear move at maximum speed = G0
    FeedLinear,			// 3D Linear move at feedrate speed = G1
    FeedHelicalCW,		// Helical move at feedrate speed = G2
    FeedHelicalCCW,		// Helical move at feedrate speed = G3
    PauseAndResume,		// Pause for a defined time, then resume = G4
    Pause,				// Pause for undefined time, resume on resume commmand = M0 or M1
    None,				// When parsing a gCode block results in no motion, the motionType = None
    nmbrMotionTypes
    };


class gCodeParserResult
    {
    public:
        enum class ParseResultType : uint8_t
            {
            EmptyBlock,						// empty GCode block - no errors, no
            OkContextUpdateOnly,
            OkContextUpdateAndMotion,
            WarningContextUpdateOnly,
            WarningContextUpdateAndMotion,
            Error,
            nmbrResults
            } theParseResultType;
        struct Motion
            {
			MotionType theMotionType;
            struct Trajectory
                {
                double length;														// length of the motion [mm] - either as linear or as helical

                // for linear
                double startPosition[(uint8_t)axis::nmbrAxis];						// [mm], per axis
                double delta[(uint8_t)axis::nmbrAxis];								// delta = nextPosition - currentPosition [mm], per axis

                // for helical
                axis arcAxis[2];													// which Axis forms the plane for the ARC
                double arcCenter[2];												// [mm]
                double radius;														// [mm]
                double startAngle;													// [rad]
                double deltaAngle;													// [rad]
                } trajectory;														// struct holding all GCODE trajectory parameters, to be passed to Motion
            struct SpeedProfile
                {
                double vFeed;														// speed requested by gCode [mm/s]
                double duration;													// duration [s]
                } speedProfile;														// struct holding all GCODE speed/duration parameters, to be passed to Motion
            struct Peripherals
                {
                bool coolantFlood;
                bool coolantMist;
                double spindleSpeed;
                rotationDirection spindledirection;
                } peripherals;
            } motion;
        enum class Error : uint8_t
            {
            None = 0,
            TooManyWordsInBlock,
            InvalidArcParameters,
            ModalGroupCollision,
            ValueWordCollision,
            InvalidLineNumber,
            InvalidFeedrate,
            MissingFeedrate,
            InvalidSpindleSpeed,

            MissingPForG4,
            InvalidDwellTime,

            MissingLForG10,
            MissingPForG10,
            InvalidPForG10,

            MissingAxisWord,
            MissingOffsetWord,

            UnsupportedgCodeWord,

            MOTION_BUFFER_OVERFLOW,

            SoftLimits,

            ConfigFileError,

            nmbrErrors
            } error;
        void toString();
    };

class gCode
    {
    private:
        // Some constants used when doing gCode math
        static constexpr double pi{ 3.141592653589793238463 };				///< constant for calculations in radians
        static constexpr double inches2mm{ 25.4 };							///< 25.4 mm/inch
        static constexpr double largeValue{ 1000000.0 };					///< Large value to initialize some vars

        enum class ReportItems : uint8_t
            {
            blockInfo = 1 << 0,
            modalsInfo = 1 << 1
            };

        char gCodeLine[1024];

        gCodeBlock theBlock;
        gCodeState theState;												// gCode parser context : current position, value of all modal groups, etc.

        int8_t searchWord(uint8_t, uint32_t) const;							// does the gCodeWords array contain a word with letter / number ? If found, returns index into gCodeWords[]
        void removeWord(uint8_t);											// removes a word [x] from gCodeWords[] by swapping it with the last one and decrementing nmbrWords
        void countModalGroupAndOthers(uint8_t);								// Keep count of how many words from each ModalGroup are found, also sets hasAxis, hasOffset, hasRadius flags

        //char responseMsg[1024];												// string to build message to report back to host computer

    public:
        gCode();															// Constructor

        void initialize();													// (Re-)initialize the parser
        void saveState();													// Save the state of the Parser, so we can do some interim stuff and afterwards restore the state back to what is was..
        void restoreState();												// Restore the state from previous saveState

        void getBlock(char *);												// Read a line of input and process as gCode block - This is the input of this module.. it receives a string
        void parseBlock(gCodeParserResult &theParseResult);					// Process all words in block, update gCode context and spawn a motion if the block contains one..
        void calcMotion(gCodeParserResult &theParseResult);					// calculate all the details for a motion resulting from a G0, G1, G2, G3, G4 block..

        uint8_t getNmbrWords(void) const;									// return number of actually used words in the block

    };

#endif


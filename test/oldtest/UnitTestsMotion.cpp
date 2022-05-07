#include "CppUnitTest.h"
#include "motionbuffer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace motiontests
{
TEST_CLASS(motionItemTests)
    {
public:
    TEST_METHOD(constructorInitialization)
        {
		MotionItem anItem;
        }
    };

TEST_CLASS(motionBufferTests)
    {
public:
    TEST_METHOD(constructorInitialization)
        {
        MotionBuffer aBuffer;
        Assert::IsTrue(aBuffer.isEmpty());				// empty upon creation
        Assert::IsFalse(aBuffer.isFull());				// not full upon creation
        }

    TEST_METHOD(pushAndPop)
        {
        MotionBuffer aBuffer;
        Assert::IsTrue(0 == aBuffer.push());			// push one item on it
        Assert::IsFalse(aBuffer.isEmpty());				// no longer empty
        Assert::IsFalse(aBuffer.isFull());				// but also not yet full
        for (uint32_t x = 1; x < aBuffer.bufferLength; x++)
            {
            Assert::IsTrue(x == aBuffer.push());		// push one item on it, and check the returned index
            }
        Assert::IsTrue(aBuffer.isFull());				//
        aBuffer.pop();									// pop one item on it, and check the returned index
        Assert::IsFalse(aBuffer.isEmpty());				//
        Assert::IsFalse(aBuffer.isFull());				//
        for (uint32_t x = 1; x < aBuffer.bufferLength; x++)
            {
            aBuffer.pop();								// pop one item on it, and check the returned index
            }
        Assert::IsTrue(aBuffer.isEmpty());				//
        }
    };
}

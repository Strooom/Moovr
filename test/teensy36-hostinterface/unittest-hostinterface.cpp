// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

// HostInterface unit tests : put a loopback on the UART0 rx and tx pins :
//   pin 26-27 on the Teensy 3.5/3.6
//   pin 0 - 1 on the Teensy 4.0

#include <stdint.h>
#include <unity.h>
#include "hostinterface_uart.h"

#define extraBytes 16

hostInterfaceUart theHostInterface;        // creates a single global object - it cannot be local to the tests, as there is an interrupt handler involved..

void test_initialization() {
    theHostInterface.initialize();
    TEST_ASSERT_FALSE(theHostInterface.hasMessage());
    // TODO : should I further test the internals here ? normally they are private...
}

void test_send_receive() {
    theHostInterface.initialize();
    uint8_t messageToSend[] = "Hello World\n";
    theHostInterface.sendMessage(messageToSend);
    delay(10);
    TEST_ASSERT_TRUE(theHostInterface.hasMessage());

    // uint8_t testReceiverBuffer[theHostInterface.txBufferLength + extraBytes];        // storage to receive messages


    // for (uint16_t index = 0; index < (theHostInterface.txBufferLength + extraBytes); index++) {
    //     if ((index % 256) == 0) {
    //         testSendMsg[index] = 0xFF;
    //     } else {
    //         testSendMsg[index] = index;
    //     }
    //     testReceiverBuffer[index] = 0xFF;
    // }
}

void test_overflow_underflow() {
    theHostInterface.initialize();
}

void test_initialization() {
    theHostInterface.initialize();
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_initialization);
    RUN_TEST(test_send_receive);
    RUN_TEST(test_overflow_underflow);
    RUN_TEST(test_initialization);
    UNITY_END();
}

void loop() {
}

// // fill both with test-data

// theTest.start("HostInterface");

// theTest.printBuffer(testSendMsg, 20, "  sendMessage : head");
// theTest.printBuffer(testSendMsg + (theHostInterface.txBufferLength + extraBytes - 20), 20, "  sendMessage : tail");
// theTest.printBuffer(testReceiverBuffer, 20, "  receiveBuffer : head");
// theTest.printBuffer(testReceiverBuffer + (theHostInterface.rxBufferLength + extraBytes - 20), 20, "  receiveBuffer : tail");

// theHostInterface.initialize();        // set everything empty before starting a (new) batch of tests
// theTest.assert(10, theHostInterface.hasMessage() == 0, "bufferLines == 0");
// theTest.assert(11, theHostInterface.getLastError() == hostInterfaceError::none, "lastError == none");

// theHostInterface.getMessage(testReceiverBuffer);        // try to read a msg before we have one...
// theTest.assert(20, testReceiverBuffer[0] == 0x00, "correct handling read empty rxBuffer");
// theTest.assert(21, theHostInterface.getLastError() == hostInterfaceError::readWhenNoMessage, "detecting read empty rxBuffer");
// theTest.assert(22, theHostInterface.getLastError() == hostInterfaceError::none, "lastError cleared");

// testSendMsg[6] = '\n';        // newline = end of message marker
// testSendMsg[7] = 0x00;        // terminating zero
// theTest.printBuffer(testSendMsg, 8, "  to be sent Message");

// theHostInterface.sendMessage(testSendMsg);        // send it out
// delay(10);                                        // wait some time for the serial transmission/reception to complete 5 bytes @ 38400 baud

// theTest.assert(30, theHostInterface.hasMessage() == 1, "bufferLines == 1 before read");
// theTest.assert(31, theHostInterface.getLastError() == hostInterfaceError::none, "lastError == none");

// theHostInterface.getMessage(testReceiverBuffer);        // try to read a msg now...

// theTest.assert(32, (testReceiverBuffer[0] == 0xFF) && (testReceiverBuffer[6] == '\n') && (testReceiverBuffer[7] == 0x00), "received msg == sent msg");
// theTest.assert(33, testReceiverBuffer[8] == 0xFF, "not receiving more than what we've sent");
// theTest.printBuffer(testReceiverBuffer, 12, "  received Message");

// theTest.assert(34, theHostInterface.hasMessage() == 0, "bufferLines == 0 after read");
// theTest.assert(35, theHostInterface.getLastError() == hostInterfaceError::none, "lastError == none");

// testSendMsg[6]                                                = 6;
// testSendMsg[7]                                                = 7;
// testSendMsg[theHostInterface.txBufferLength + extraBytes - 1] = 0x00;
// theTest.printBuffer(testSendMsg, 20, "  to be sent Message : head");
// theTest.printBuffer(testSendMsg + (theHostInterface.txBufferLength + extraBytes - 20), 20, "  to be sent Message : tail");

// theHostInterface.sendMessage(testSendMsg);        // try to send a message longer than txBuffer, but because it's too large, it should be ignored.
// theTest.assert(40, theHostInterface.getLastError() == hostInterfaceError::txBufferOverflow, "detecting txBuffer Overflow");

// testSendMsg[theHostInterface.txBufferLength - 2] = '\n';
// testSendMsg[theHostInterface.txBufferLength - 1] = 0x00;
// theHostInterface.sendMessage(testSendMsg);        // send max length message, filling the rxBuffer
// delay(100);                                       // wait some time for the serial transmission/reception to complete

// theTest.assert(41, theHostInterface.getLastError() == hostInterfaceError::none, "no rxBufferoverflow yet");

// testSendMsg[6] = '\n';                            // newline = end of message marker
// testSendMsg[7] = 0x00;                            // terminating zero
// theHostInterface.sendMessage(testSendMsg);        // send extra bytes, should cause rxBuffer overflow
// delay(10);                                        // wait some time for the serial transmission/reception to complete
// theTest.assert(42, theHostInterface.getLastError() == hostInterfaceError::rxBufferOverflow, "rxBufferoverflow detected");

// theHostInterface.getMessage(testReceiverBuffer);        // read rxBuffer empty...
// theHostInterface.getMessage(testReceiverBuffer);        // read rxBuffer empty...
// theHostInterface.getMessage(testReceiverBuffer);        // read rxBuffer empty...
// (void)theHostInterface.getLastError();                  // clear any remaining errors

// theHostInterface.sendMessage(testSendMsg);        //
// delay(10);                                        // wait some time for the serial transmission/reception to complete 5 bytes @ 38400 baud
// theTest.assert(50, theHostInterface.hasMessage() == 1, "bufferLines == 1 before read");
// theTest.assert(51, theHostInterface.getLastError() == hostInterfaceError::none, "lastError == none");
// theTest.printBuffer(testReceiverBuffer, 12, "  received Message");
// theHostInterface.getMessage(testReceiverBuffer);        // try to read a msg now...
// theTest.printBuffer(testReceiverBuffer, 12, "  received Message");
// theTest.assert(52, (testReceiverBuffer[0] == 0xFF) && (testReceiverBuffer[6] == '\n') && (testReceiverBuffer[7] == 0x00), "received msg == sent msg");

// theTest.end();
// delay(2000);
// }

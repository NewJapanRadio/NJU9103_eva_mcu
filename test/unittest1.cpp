#include <limits.h>

#include "nju9103.h"

#include "gtest/gtest.h"

typedef struct {
    uint8_t expected;
    uint8_t pattern [PACKET_SIZE];
} TestCase;

TestCase testPattern[] = {
    0xFF, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    0xFE, { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    0xD2, { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 },
    0x2C, { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7 },
    0x08, { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};

TEST(Main, ChkSumByteArray) {
    for (int i = 0; i < sizeof(testPattern)/sizeof(TestCase); i++) {
        EXPECT_EQ(testPattern[i].expected, calculateChkSum(testPattern[i].pattern));
    }
}

TEST(Main, ChkSumPacket) {
    Packet packet;
    for (int i = 0; i < sizeof(testPattern)/sizeof(TestCase); i++) {
        packet.Header = testPattern[i].pattern[0];
        packet.Byte0 = testPattern[i].pattern[1];
        packet.Byte1 = testPattern[i].pattern[2];
        packet.Byte2 = testPattern[i].pattern[3];
        packet.Byte3 = testPattern[i].pattern[4];
        packet.Byte4 = testPattern[i].pattern[5];
        packet.Byte5 = testPattern[i].pattern[6];
        packet.Byte6 = testPattern[i].pattern[7];
        packet.Byte7 = testPattern[i].pattern[8];
        EXPECT_EQ(testPattern[i].expected, calculateChkSum(&packet));
    }
}

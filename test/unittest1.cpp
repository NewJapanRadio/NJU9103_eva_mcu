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

TEST(Main, CmdDecode) {
    uint8_t header;
    uint8_t opcode;
    Command cmd;

    // RegisterWrite8Bit
    cmd = 0;
    header = CommandHeader;
    opcode = OP_REGISTER_WRITE_8BIT;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_WRITE_8BIT, cmd);

    // RegisterRead8Bit
    cmd = 0;
    header = CommandHeader;
    opcode = OP_REGISTER_READ_8BIT;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_READ_8BIT, cmd);

    // RegisterWrite16Bit
    cmd = 0;
    header = CommandHeader;
    opcode = OP_REGISTER_WRITE_16BIT;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_WRITE_16BIT, cmd);

    // RegisterRead16Bit
    cmd = 0;
    header = CommandHeader;
    opcode = OP_REGISTER_READ_16BIT;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_READ_16BIT, cmd);

    // StartSingleConversion
    cmd = 0;
    header = CommandHeader;
    opcode = OP_START_SINGLE_CONVERSION;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_START_SINGLE, cmd);

    // StartContinuousConversion
    cmd = 0;
    header = CommandHeader;
    opcode = OP_START_CONTINUOUS_CONVERSION;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_START_CONTINUOUS, cmd);

    // StartContinuousConversion
    cmd = 0;
    header = CommandHeader;
    opcode = OP_START_ADC_DATA_DUMP;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_START_DUMP, cmd);

    // StopSingleConversion
    cmd = 0;
    header = CommandHeader;
    opcode = OP_STOP_SINGLE_CONVERSION;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_STOP_SINGLE, cmd);

    // StopContinuousConversion
    cmd = 0;
    header = CommandHeader;
    opcode = OP_STOP_CONTINUOUS_CONVERSION;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_STOP_CONTINUOUS, cmd);

    // StopContinuousConversion
    cmd = 0;
    header = CommandHeader;
    opcode = OP_STOP_ADC_DATA_DUMP;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_STOP_DUMP, cmd);

    // RegisterWrite8Bit
    cmd = 0;
    header = CommandHeader;
    opcode = OP_SPI_RESET;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_RESET, cmd);
}

TEST(Main, InvalidCommand) {
    uint8_t header;
    uint8_t opcode;
    Command cmd;

    cmd = 0;
    header = CommandHeader;
    opcode = 0xFF;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_UNKNOWN, cmd);
}

TEST(Main, UnexpectedPacketHeader) {
    uint8_t header;
    uint8_t opcode;
    Command cmd;

    // ResponseHeader
    cmd = 0;
    header = ResponseHeader;
    opcode = OP_REGISTER_WRITE_8BIT;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_UNKNOWN, cmd);

    // DataHeader
    cmd = 0;
    header = DataHeader;
    opcode = OP_REGISTER_WRITE_8BIT;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_UNKNOWN, cmd);

    // ErrorHeader
    cmd = 0;
    header = ErrorHeader;
    opcode = OP_REGISTER_WRITE_8BIT;
    decodeCommand(header, opcode, &cmd);
    EXPECT_EQ(CMD_UNKNOWN, cmd);
}

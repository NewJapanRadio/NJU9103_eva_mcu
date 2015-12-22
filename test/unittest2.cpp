#include <limits.h>

#include "nju9103.h"

#include "gtest/gtest.h"

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

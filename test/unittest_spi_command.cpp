#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

#define private public
#include "nju9103.h"
#undef private

TEST(SPICommand, SpiReset) {
    SPICommand spiCommand;
    {
        InSequence s;
        EXPECT_CALL(*spiCommand.spi, write(0x7F));
        EXPECT_CALL(*spiCommand.spi, write(0xFF));
        EXPECT_CALL(*spiCommand.spi, write(0xFF));
    }
    EXPECT_EQ(SPICommand::Success, spiCommand.SPIReset());
}

TEST(SPICommand, RegisterWrite8Bit) {
    SPICommand spiCommand;

    uint8_t addr = 0x0A;
    uint8_t data = 0xBC;

    {
        InSequence s;
        EXPECT_CALL(*spiCommand.spi, write(0xA0)); // { addr[3:0], RW = 0, BC = 0, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(0xBC));
    }

    EXPECT_EQ(SPICommand::Success, spiCommand.RegisterWrite8Bit(addr, data));
}

TEST(SPICommand, RegisterWrite16Bit) {
    SPICommand spiCommand;

    uint8_t  addr  = 0x01;
    uint16_t data = 0xCAFE;

    {
        InSequence s;
        EXPECT_CALL(*spiCommand.spi, write(0x14)); // { addr[3:0], RW = 0, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(0xCA));
        EXPECT_CALL(*spiCommand.spi, write(0xFE));
    }

    EXPECT_EQ(SPICommand::Success, spiCommand.RegisterWrite16Bit(addr, (uint8_t)(data >> 8), (uint8_t)((data & 0x00FF))));
}

TEST(SPICommand, RegisterRead8Bit) {
    SPICommand spiCommand;

    uint8_t addr = 0x04;
    uint8_t data;

    {
        InSequence s;
        EXPECT_CALL(*spiCommand.spi, write(0x48)); // { addr[3:0], RW = 1, BC = 0, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_))
            .WillOnce(Return(0x55));
    }

    EXPECT_EQ(SPICommand::Success, spiCommand.RegisterRead8Bit(addr, &data));
    EXPECT_EQ(data, 0x55);
}

TEST(SPICommand, RegisterRead16Bit) {
    SPICommand spiCommand;

    uint8_t addr = 0x04;
    uint8_t data0;
    uint8_t data1;

    {
        InSequence s;
        EXPECT_CALL(*spiCommand.spi, write(0x4C)); // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_))
            .WillOnce(Return(0x70));
        EXPECT_CALL(*spiCommand.spi, write(_))
            .WillOnce(Return(0xCA));
    }

    EXPECT_EQ(SPICommand::Success, spiCommand.RegisterRead16Bit(addr, &data0, &data1));
    EXPECT_EQ(data0, 0x70);
    EXPECT_EQ(data1, 0xCA);
}

TEST(SPICommand, StartSingleConversion) {
    SPICommand spiCommand;
    uint16_t rd;

    uint8_t chsel_mode =  0x22;

    // emulate rdyb wait
    EXPECT_CALL(*spiCommand.rdyb, read())
        .Times(AtLeast(3))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0));

    {
        InSequence s;
        EXPECT_CALL(*spiCommand.spi, write(0x00));       // WriteCommand, CTRL Register Address
        EXPECT_CALL(*spiCommand.spi, write(chsel_mode)); // CTRL Register Value
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_))
            .Times(Exactly(2))
            .WillOnce(Return(0x12))   // ADCDATA0 Register Value
            .WillOnce(Return(0x34));  // ADCDATA1 Register Value
    }

    EXPECT_EQ(SPICommand::Success, spiCommand.StartSingleConversion(chsel_mode, &rd));
    EXPECT_EQ(0x1234, rd);
}

TEST(SPICommand, StartContinuousConversion) {
    SPICommand spiCommand;
    uint16_t rd[7];
    uint16_t len;

    uint8_t chsel_mode =  0x23;

    // emulate rdyb wait
    EXPECT_CALL(*spiCommand.rdyb, read())
        .Times(AtLeast(3 * (0x6+1)))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 1
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 2
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 3
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 4
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 5
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 6
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0));// 7

    {
        InSequence s;
        EXPECT_CALL(*spiCommand.spi, write(0x00));       // WriteCommand, CTRL Register Address
        EXPECT_CALL(*spiCommand.spi, write(chsel_mode)); // CTRL Register Value
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0x01)).WillOnce(Return(0x23));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0x45)).WillOnce(Return(0x67));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0x89)).WillOnce(Return(0xAB));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0xCD)).WillOnce(Return(0xEF));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0xED)).WillOnce(Return(0xCB));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0xA9)).WillOnce(Return(0x87));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0x65)).WillOnce(Return(0x43));
    }

    EXPECT_EQ(SPICommand::Success, spiCommand.StartContinuousConversion(chsel_mode, rd, 0x6, &len));
    EXPECT_EQ(7, len);
    EXPECT_EQ(0x0123, rd[0]);
    EXPECT_EQ(0x4567, rd[1]);
    EXPECT_EQ(0x89AB, rd[2]);
    EXPECT_EQ(0xCDEF, rd[3]);
    EXPECT_EQ(0xEDCB, rd[4]);
    EXPECT_EQ(0xA987, rd[5]);
    EXPECT_EQ(0x6543, rd[6]);
}

TEST(SPICommand, StartIntermittentConversion) {
    SPICommand spiCommand;
    uint16_t rd[7];
    uint16_t len;

    uint8_t chsel_mode =  0x23;

    // emulate rdyb wait
    EXPECT_CALL(*spiCommand.rdyb, read())
        .Times(AtLeast(3 * (0x6+1)))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 1
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 2
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 3
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 4
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 5
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 6
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0));// 7

    EXPECT_CALL(*spiCommand.sleep, sleep(_))
        .Times(AnyNumber());
    EXPECT_CALL(*spiCommand.sleep, sleepMillisecond(_))
        .Times(AnyNumber());
    EXPECT_CALL(*spiCommand.sleep, sleepMicrosecond(_))
        .Times(AnyNumber());
    EXPECT_CALL(*spiCommand.stopwatch, start())
        .Times(AnyNumber());
    EXPECT_CALL(*spiCommand.stopwatch, stop())
        .Times(AnyNumber());
    EXPECT_CALL(*spiCommand.stopwatch, read())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*spiCommand.stopwatch, readMillisecond())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*spiCommand.stopwatch, readMicrosecond())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));

    {
        InSequence s;

        // data pattern is same as test of StartIntermittentConversion
        for (int i = 0x01; i <= 0xEF; i = i + 0x44) {
            EXPECT_CALL(*spiCommand.spi, write(0x00));
            EXPECT_CALL(*spiCommand.spi, write(chsel_mode));
            EXPECT_CALL(*spiCommand.spi, write(0x1C));
            EXPECT_CALL(*spiCommand.spi, write(_))
                .Times(Exactly(2))
                .WillOnce(Return(i))
                .WillOnce(Return(i+0x22));
        }
        for (int i = 0xED; i >= 0x43; i = i - 0x44) {
            EXPECT_CALL(*spiCommand.spi, write(0x00));
            EXPECT_CALL(*spiCommand.spi, write(chsel_mode));
            EXPECT_CALL(*spiCommand.spi, write(0x1C));
            EXPECT_CALL(*spiCommand.spi, write(_))
                .Times(Exactly(2))
                .WillOnce(Return(i))
                .WillOnce(Return(i-0x22));
        }
    }

    EXPECT_EQ(SPICommand::Success, spiCommand.StartIntermittentConversion(chsel_mode, rd, 1050, 0x6, &len));
    EXPECT_EQ(7, len);
    EXPECT_EQ(0x0123, rd[0]);
    EXPECT_EQ(0x4567, rd[1]);
    EXPECT_EQ(0x89AB, rd[2]);
    EXPECT_EQ(0xCDEF, rd[3]);
    EXPECT_EQ(0xEDCB, rd[4]);
    EXPECT_EQ(0xA987, rd[5]);
    EXPECT_EQ(0x6543, rd[6]);
}

TEST(SPICommand, WrongAddress_RegisterWrite8Bit) {
    SPICommand spiCommand;

    uint8_t addr = 0xA0;
    uint8_t data = 0xBC;

    EXPECT_CALL(*spiCommand.spi, write(_))
        .Times(Exactly(0));

    EXPECT_EQ(SPICommand::Error, spiCommand.RegisterWrite8Bit(addr, data));
}

TEST(SPICommand, WrongAddress_RegisterWrite16Bit) {
    SPICommand spiCommand;

    uint8_t  addr  = 0x10;
    uint16_t data = 0xCAFE;

    EXPECT_CALL(*spiCommand.spi, write(_))
        .Times(Exactly(0));

    EXPECT_EQ(SPICommand::Error, spiCommand.RegisterWrite16Bit(addr, (uint8_t)(data >> 8), (uint8_t)((data & 0x00FF))));
}

TEST(SPICommand, WrongAddress_RegisterRead8Bit) {
    SPICommand spiCommand;

    uint8_t addr = 0x40;
    uint8_t data = 0x00;

    EXPECT_CALL(*spiCommand.spi, write(_))
        .Times(Exactly(0));

    EXPECT_EQ(SPICommand::Error, spiCommand.RegisterRead8Bit(addr, &data));
    EXPECT_EQ(data, 0x00);
}

TEST(SPICommand, WrongAddress_RegisterRead16Bit) {
    SPICommand spiCommand;

    uint8_t addr = 0xF0;
    uint8_t data0 = 0x00;
    uint8_t data1 = 0x00;

    EXPECT_CALL(*spiCommand.spi, write(_))
        .Times(Exactly(0));

    EXPECT_EQ(SPICommand::Error, spiCommand.RegisterRead16Bit(addr, &data0, &data1));
    EXPECT_EQ(data0, 0x00);
    EXPECT_EQ(data1, 0x00);
}

TEST(SPICommand, Abort_StartSingleConversion) {
    SPICommand spiCommand;
    uint16_t rd = 0x00;

    uint8_t chsel_mode =  0x22;

    // emulate rdyb wait
    EXPECT_CALL(*spiCommand.rdyb, read())
        .WillRepeatedly(Return(1));

    {
        InSequence s;
        EXPECT_CALL(*spiCommand.spi, write(0x00));       // WriteCommand, CTRL Register Address
        EXPECT_CALL(*spiCommand.spi, write(chsel_mode)); // CTRL Register Value
        spiCommand.SetAbortRequest();
    }

    EXPECT_EQ(SPICommand::Abort, spiCommand.StartSingleConversion(chsel_mode, &rd));
    EXPECT_EQ(0x00, rd);
}

TEST(SPICommand, Abort_StartContinuousConversion) {
    SPICommand spiCommand;
    uint16_t rd[7] = { 0x00 };
    uint16_t len = 0;

    uint8_t chsel_mode =  0x23;

    // emulate rdyb wait
    EXPECT_CALL(*spiCommand.rdyb, read())
        .Times(AtLeast(3 * (0x6+1)))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 1
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 2
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 3
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 4
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 5
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 6
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(DoAll(InvokeWithoutArgs(&spiCommand, &SPICommand::SetAbortRequest), Return(1)));// 7

    {
        InSequence s;
        EXPECT_CALL(*spiCommand.spi, write(0x00));       // WriteCommand, CTRL Register Address
        EXPECT_CALL(*spiCommand.spi, write(chsel_mode)); // CTRL Register Value
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0x01)).WillOnce(Return(0x23));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0x45)).WillOnce(Return(0x67));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0x89)).WillOnce(Return(0xAB));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0xCD)).WillOnce(Return(0xEF));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0xED)).WillOnce(Return(0xCB));
        EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0xA9)).WillOnce(Return(0x87));
        // StartContinuousConversion will be aborted and followings are not called.
        // EXPECT_CALL(*spiCommand.spi, write(0x1C));       // { addr[3:0], RW = 1, BC = 1, 0b00 }
        // EXPECT_CALL(*spiCommand.spi, write(_)).Times(Exactly(2)).WillOnce(Return(0x65)).WillOnce(Return(0x43));
    }

    EXPECT_EQ(SPICommand::Abort, spiCommand.StartContinuousConversion(chsel_mode, rd, 0x6, &len));
    EXPECT_EQ(6, len);
    EXPECT_EQ(0x0123, rd[0]);
    EXPECT_EQ(0x4567, rd[1]);
    EXPECT_EQ(0x89AB, rd[2]);
    EXPECT_EQ(0xCDEF, rd[3]);
    EXPECT_EQ(0xEDCB, rd[4]);
    EXPECT_EQ(0xA987, rd[5]);
    EXPECT_EQ(0x0000, rd[6]);
}

TEST(SPICommand, Abort_StartIntermittentConversion) {
    SPICommand spiCommand;
    uint16_t rd[7] = { 0 };
    uint16_t len;

    uint8_t chsel_mode =  0x23;

    // emulate rdyb wait
    EXPECT_CALL(*spiCommand.rdyb, read())
        .Times(AtLeast((0x4+1)))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 1
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 2
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 3
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0)) // 4
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(DoAll(InvokeWithoutArgs(&spiCommand, &SPICommand::SetAbortRequest), Return(1)));

    EXPECT_CALL(*spiCommand.sleep, sleep(_))
        .Times(AnyNumber());
    EXPECT_CALL(*spiCommand.sleep, sleepMillisecond(_))
        .Times(AnyNumber());
    EXPECT_CALL(*spiCommand.sleep, sleepMicrosecond(_))
        .Times(AnyNumber());
    EXPECT_CALL(*spiCommand.stopwatch, start())
        .Times(AnyNumber());
    EXPECT_CALL(*spiCommand.stopwatch, stop())
        .Times(AnyNumber());
    EXPECT_CALL(*spiCommand.stopwatch, read())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*spiCommand.stopwatch, readMillisecond())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*spiCommand.stopwatch, readMicrosecond())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));

    {
        InSequence s;

        // data pattern is same as test of StartIntermittentConversion
        for (int i = 0x01; i <= 0xEF; i = i + 0x44) {
            EXPECT_CALL(*spiCommand.spi, write(0x00));
            EXPECT_CALL(*spiCommand.spi, write(chsel_mode));
            EXPECT_CALL(*spiCommand.spi, write(0x1C));
            EXPECT_CALL(*spiCommand.spi, write(_))
                .Times(Exactly(2))
                .WillOnce(Return(i))
                .WillOnce(Return(i+0x22));
        }
        EXPECT_CALL(*spiCommand.spi, write(0x00));
        EXPECT_CALL(*spiCommand.spi, write(chsel_mode));
        // for (int i = 0xED; i >= 0x43; i = i - 0x44) {
        //     EXPECT_CALL(*spiCommand.spi, write(0x00));
        //     EXPECT_CALL(*spiCommand.spi, write(chsel_mode));
        //     EXPECT_CALL(*spiCommand.spi, write(0x1C));
        //     EXPECT_CALL(*spiCommand.spi, write(_))
        //         .Times(Exactly(2))
        //         .WillOnce(Return(i))
        //         .WillOnce(Return(i-0x22));
        // }
    }

    EXPECT_EQ(SPICommand::Abort, spiCommand.StartIntermittentConversion(chsel_mode, rd, 1050, 0x6, &len));
    EXPECT_EQ(4, len);
    EXPECT_EQ(0x0123, rd[0]);
    EXPECT_EQ(0x4567, rd[1]);
    EXPECT_EQ(0x89AB, rd[2]);
    EXPECT_EQ(0xCDEF, rd[3]);
    EXPECT_EQ(0x0000, rd[4]);
    EXPECT_EQ(0x0000, rd[5]);
    EXPECT_EQ(0x0000, rd[6]);
}

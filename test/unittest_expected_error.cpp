#include <limits.h>

#define private public
#include "nju9103.h"
#undef private

#include "gtest/gtest.h"

#include <stdio.h>

using namespace ::testing;

extern Packet rxPacket;
extern ReceiveDataStatus receiveDataStatus;
extern Command command;

extern ::Serial uart;
extern ::Timer packetWatchTimer;
extern ::Dispatcher dispatcher;
extern ::ADCDataBuffer adcDataBuffer;

extern void (*fpIsrRx)(void);
extern void _attach(void (*fp)(void), ::Serial::IrqType type);

TEST(ExpectedError, CheckSumError) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x3, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0000, command);

    loop();
}

TEST(ExpectedError, UnknownCommand) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x08 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x8000, command);

    loop();
}

TEST(ExpectedError, RegisterWrite8Bit_InvalidAddress) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0x10, 0x10, 0x15, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB8 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFE, 0x10, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEC };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0001, command);

    loop();
}

TEST(ExpectedError, RegisterRead8Bit_InvalidAddress) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0x10, 0x20, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBC };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFE, 0x20, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDE };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0002, command);

    loop();
}

TEST(ExpectedError, RegisterWrite16Bit_InvalidAddress) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0x10, 0x30, 0xFA, 0x2F, 0x59, 0x00, 0x00, 0x00, 0x00, 0x3D };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFE, 0x30, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE7 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0004, command);

    loop();
}

TEST(ExpectedError, RegisterRead16Bit_InvalidAddress) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0x10, 0x40, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFE, 0x40, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0008, command);

    loop();
}

TEST(ExpectedError, StartSingleConversion_InvalidCtrl) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0x10, 0x50, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9E };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFE, 0x50, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0010, command);

    loop();
}

TEST(ExpectedError, StartContinuousConversion_InvalidCtrl) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0x10, 0x60, 0x0E, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x7D };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFE, 0x60, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA3 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0020, command);

    loop();
}

TEST(ExpectedError, StartContinuousConversion_BufferAllocError) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0x10, 0x60, 0x02, 0x40, 0x03, 0x00, 0x00, 0x00, 0x00, 0x4A };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFD, 0x60, 0x40, 0x03, 0x00, 0x00, 0x00, 0x00, 0x6F };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0020, command);

    loop();
}

TEST(ExpectedError, StartIntermittentConversion_InvalidCtrl) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0x10, 0x70, 0x0E, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x6D };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFE, 0x70, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0040, command);

    loop();
}

TEST(ExpectedError, StartIntermittentConversion_BufferAllocError) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0x10, 0x70, 0x02, 0x40, 0x03, 0x00, 0x00, 0x00, 0x00, 0x3A };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0xF0, 0xFD, 0x70, 0x40, 0x03, 0x00, 0x00, 0x00, 0x00, 0x5F };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i ++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0040, command);

    loop();
}

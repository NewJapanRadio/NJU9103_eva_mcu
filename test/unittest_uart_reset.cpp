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


TEST(UARTReset, Case1) {
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*3; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x50));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x12));
    }

    uint8_t cmd[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x10, 0x10, 0x05, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8 };
    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*3; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    EXPECT_CALL(uart, write(_)).Times(AnyNumber());

    setup();

    for (int i = 0; i < (PACKET_SIZE+1)*3; i ++) {
        fpIsrRx();
        isrPacketWatch();
    }

    loop();
}

TEST(UARTReset, Case2) {
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*4; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x50));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x12));
    }

    uint8_t cmd[] = { 0x00, 0x10, 0x00, 0x00, 0x00, 0x40, 0x00, 0x04, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x10, 0x10, 0x05, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8 };
    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*4; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    EXPECT_CALL(uart, write(_)).Times(AnyNumber());

    setup();

    for (int i = 0; i < (PACKET_SIZE+1)*4; i ++) {
        fpIsrRx();
        isrPacketWatch();
    }

    loop();
    loop();
}

TEST(UARTReset, Case3) {
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*5; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x50));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x12));
    }

    uint8_t cmd[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x10, 0x10, 0x05, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8 };
    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*5; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    EXPECT_CALL(uart, write(_)).Times(AnyNumber());

    setup();

    for (int i = 0; i < (PACKET_SIZE+1)*5; i ++) {
        fpIsrRx();
        isrPacketWatch();
    }

    loop();
    loop();
    loop();
}

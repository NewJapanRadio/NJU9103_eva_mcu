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

void (*fpIsrRx)(void);
void _attach(void (*fp)(void), ::Serial::IrqType type) {
    fpIsrRx = fp;
}

TEST(UART, SPIReset) {
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

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x7F));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF));
    }

    uint8_t cmd[] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDF };
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
    EXPECT_EQ(0x1000, command);

    loop();
}

TEST(UART, RegisterWrite8Bit) {
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

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x50));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x12));
    }

    uint8_t cmd[] = { 0x10, 0x10, 0x05, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x10, 0x05, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB8 };
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

TEST(UART, RegisterWrite16Bit) {
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

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xA4));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x2F));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x59));
    }

    uint8_t cmd[] = { 0x10, 0x30, 0x0A, 0x2F, 0x59, 0x00, 0x00, 0x00, 0x00, 0x2D };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x30, 0x0A, 0x2F, 0x59, 0x00, 0x00, 0x00, 0x00, 0x1D };
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

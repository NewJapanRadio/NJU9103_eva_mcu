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

TEST(UART, SPIReset) {
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

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x1000, command);

    loop();
}

TEST(UART, RegisterWrite8Bit) {
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

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0001, command);

    loop();
}

TEST(UART, RegisterRead8Bit) {
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

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x38));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .WillOnce(Return(0x45));
    }

    uint8_t cmd[] = { 0x10, 0x20, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x20, 0x03, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0002, command);

    loop();
}

TEST(UART, RegisterWrite16Bit) {
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

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0004, command);

    loop();
}

TEST(UART, RegisterRead16Bit) {
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

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x5C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .WillOnce(Return(0x67));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .WillOnce(Return(0x89));
    }

    uint8_t cmd[] = { 0x10, 0x40, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x40, 0x05, 0x67, 0x89, 0x00, 0x00, 0x00, 0x00, 0xAA };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0008, command);

    loop();
}

TEST(UART, StartSingleConversion) {
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

    EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
        .Times(AtLeast(3))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0));

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00)); // CTRL Register Address
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02)); // { chsel, mode }
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C)); // ADCDATA0 Register Address
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0xAB))   // ADCDATA0 Register Value
            .WillOnce(Return(0xCD));  // ADCDATA1 Register Value
    }

    uint8_t cmd[] = { 0x10, 0x50, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x50, 0x02, 0xAB, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x15 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0010, command);

    loop();
}

TEST(UART, StartContinuousConversion) {
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

    EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
        .Times(Exactly(15))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0));

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00)); // CTRL Register Address
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02)); // { chsel, mode }

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0x12))
            .WillOnce(Return(0x34));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0x56))
            .WillOnce(Return(0x78));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0x9A))
            .WillOnce(Return(0xBC));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0xDE))
            .WillOnce(Return(0xF1));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0x23))
            .WillOnce(Return(0x45));
    }

    uint8_t cmd[] = { 0x10, 0x60, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x89 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x60, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x79 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0020, command);

    loop();

    EXPECT_EQ(8, adcDataBuffer.allocatedSize);
    EXPECT_EQ(0x1234, adcDataBuffer.adcDataBuffer[0]);
    EXPECT_EQ(0x5678, adcDataBuffer.adcDataBuffer[1]);
    EXPECT_EQ(0x9ABC, adcDataBuffer.adcDataBuffer[2]);
    EXPECT_EQ(0xDEF1, adcDataBuffer.adcDataBuffer[3]);
    EXPECT_EQ(0x2345, adcDataBuffer.adcDataBuffer[4]);
    EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[5]);
}

TEST(UART, StartIntermittentConversion) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    EXPECT_CALL(*(dispatcher.spiCommand)->sleep, sleep(_))
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->sleep, sleepMillisecond(_))
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->sleep, sleepMicrosecond(_))
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, start())
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, stop())
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, read())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, readMillisecond())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, readMicrosecond())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
        .Times(Exactly(15))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0));

    {
        InSequence s;

        // data pattern is same as test of StartIntermittentConversion
        for (int i = 0x12; i <= 0xBC; i = i + 0x44) {
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00));
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02));
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(_))
                .Times(Exactly(2))
                .WillOnce(Return(i))
                .WillOnce(Return(i+0x22));
        }
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(_))
            .Times(Exactly(2))
            .WillOnce(Return(0xDE))
            .WillOnce(Return(0xF1));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(_))
            .Times(Exactly(2))
            .WillOnce(Return(0x23))
            .WillOnce(Return(0x45));
    }

    uint8_t cmd[] = { 0x10, 0x70, 0x02, 0x00, 0x04, 0x00, 0x0F, 0x42, 0x40, 0xE8 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x70, 0x02, 0x00, 0x04, 0x00, 0x0F, 0x42, 0x40, 0xD8 };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0040, command);

    loop();

    EXPECT_EQ(8, adcDataBuffer.allocatedSize);
    EXPECT_EQ(0x1234, adcDataBuffer.adcDataBuffer[0]);
    EXPECT_EQ(0x5678, adcDataBuffer.adcDataBuffer[1]);
    EXPECT_EQ(0x9ABC, adcDataBuffer.adcDataBuffer[2]);
    EXPECT_EQ(0xDEF1, adcDataBuffer.adcDataBuffer[3]);
    EXPECT_EQ(0x2345, adcDataBuffer.adcDataBuffer[4]);
    EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[5]);
}

TEST(UART, StartADCDataDump) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    adcDataBuffer.SetDataLength(5);
    adcDataBuffer.allocatedSize = 8;

    adcDataBuffer.adcDataBuffer[0] = 0x1234;
    adcDataBuffer.adcDataBuffer[1] = 0x5678;
    adcDataBuffer.adcDataBuffer[2] = 0x9ABC;
    adcDataBuffer.adcDataBuffer[3] = 0xDEF1;
    adcDataBuffer.adcDataBuffer[4] = 0x2345;
    adcDataBuffer.adcDataBuffer[5] = 0x0000;

    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd[] = { 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd[i]));
        }
    }

    uint8_t res[] = {
        0x20, 0x80, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x5B,
        0x30, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x96,
        0x30, 0x23, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67
    };
    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*3; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0080, command);

    loop();
}

void StopSingleConversion() {
    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }
    isrPacketWatch();
}
TEST(UART, StopSingleConversion) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
        .WillOnce(Return(1))
        .WillOnce(DoAll(InvokeWithoutArgs(StopSingleConversion), Return(1)))
        .WillRepeatedly(Return(1));

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00)); // CTRL Register Address
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02)); // { chsel, mode }
    }

    uint8_t cmd1[] = { 0x10, 0x50, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D };
    uint8_t cmd2[] = { 0x10, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9E };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd1[i]));
        }
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd2[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8E };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0010, command);

    // start single conversion
    loop();
    // abort
    loop();
}
TEST(UART, StopSingleConversion_By_SPIReset) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
        .WillOnce(Return(1))
        .WillOnce(DoAll(InvokeWithoutArgs(StopSingleConversion), Return(1)))
        .WillRepeatedly(Return(1));

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00)); // CTRL Register Address
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02)); // { chsel, mode }
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x7F)); // SPIResetCommand 0
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 1
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 2
    }

    uint8_t cmd1[] = { 0x10, 0x50, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D };
    uint8_t cmd2[] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd1[i]));
        }
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd2[i]));
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

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0010, command);

    // start single conversion
    loop();
    // abort
    loop();
}

void StopContinuousConversion() {
    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }
    isrPacketWatch();
}
TEST(UART, StopContinuousConversion) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
        .Times(Exactly(15))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(DoAll(InvokeWithoutArgs(StopContinuousConversion), Return(1)));

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00)); // CTRL Register Address
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02)); // { chsel, mode }

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0x12))
            .WillOnce(Return(0x34));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0x56))
            .WillOnce(Return(0x78));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0x9A))
            .WillOnce(Return(0xBC));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0xDE))
            .WillOnce(Return(0xF1));

        // This transaction will be aborted
        // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
        //     .Times(Exactly(2))
        //     .WillOnce(Return(0x23))
        //     .WillOnce(Return(0x45));
    }

    uint8_t cmd1[] = { 0x10, 0x60, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x89 };
    uint8_t cmd2[] = { 0x10, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8E };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd1[i]));
        }
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd2[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0020, command);

    loop();
    loop();

    EXPECT_EQ(8, adcDataBuffer.allocatedSize);
    EXPECT_EQ(0x1234, adcDataBuffer.adcDataBuffer[0]);
    EXPECT_EQ(0x5678, adcDataBuffer.adcDataBuffer[1]);
    EXPECT_EQ(0x9ABC, adcDataBuffer.adcDataBuffer[2]);
    EXPECT_EQ(0xDEF1, adcDataBuffer.adcDataBuffer[3]);
    EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[4]);
    EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[5]);
}
TEST(UART, StopContinuousConversion_By_SPIReset) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
        .Times(Exactly(15))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(DoAll(InvokeWithoutArgs(StopContinuousConversion), Return(1)));

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00)); // CTRL Register Address
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02)); // { chsel, mode }

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0x12))
            .WillOnce(Return(0x34));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0x56))
            .WillOnce(Return(0x78));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0x9A))
            .WillOnce(Return(0xBC));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
            .Times(Exactly(2))
            .WillOnce(Return(0xDE))
            .WillOnce(Return(0xF1));

        // This transaction will be aborted
        // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
        //     .Times(Exactly(2))
        //     .WillOnce(Return(0x23))
        //     .WillOnce(Return(0x45));

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x7F)); // SPIResetCommand 0
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 1
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 2
    }

    uint8_t cmd1[] = { 0x10, 0x60, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x89 };
    uint8_t cmd2[] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd1[i]));
        }
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd2[i]));
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

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0020, command);

    loop();
    loop();

    EXPECT_EQ(8, adcDataBuffer.allocatedSize);
    EXPECT_EQ(0x1234, adcDataBuffer.adcDataBuffer[0]);
    EXPECT_EQ(0x5678, adcDataBuffer.adcDataBuffer[1]);
    EXPECT_EQ(0x9ABC, adcDataBuffer.adcDataBuffer[2]);
    EXPECT_EQ(0xDEF1, adcDataBuffer.adcDataBuffer[3]);
    EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[4]);
    EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[5]);
}

void StopIntermittentConversion() {
    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }
    isrPacketWatch();
}
TEST(UART, StopIntermittentConversion) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    EXPECT_CALL(*(dispatcher.spiCommand)->sleep, sleep(_))
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->sleep, sleepMillisecond(_))
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->sleep, sleepMicrosecond(_))
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, start())
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, stop())
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, read())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, readMillisecond())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, readMicrosecond())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
        .Times(Exactly(15))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(DoAll(InvokeWithoutArgs(StopIntermittentConversion), Return(1)));

    {
        InSequence s;

        // data pattern is same as test of StartIntermittentConversion
        for (int i = 0x12; i <= 0xBC; i = i + 0x44) {
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00));
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02));
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(_))
                .Times(Exactly(2))
                .WillOnce(Return(i))
                .WillOnce(Return(i+0x22));
        }
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(_))
            .Times(Exactly(2))
            .WillOnce(Return(0xDE))
            .WillOnce(Return(0xF1));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02));
        // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(_))
        //     .Times(Exactly(2))
        //     .WillOnce(Return(0x23))
        //     .WillOnce(Return(0x45));
    }

    uint8_t cmd1[] = { 0x10, 0x70, 0x02, 0x00, 0x04, 0x00, 0x0F, 0x42, 0x40, 0xE8 };
    uint8_t cmd2[] = { 0x10, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd1[i]));
        }
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd2[i]));
        }
    }

    uint8_t res[] = { 0x20, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6E };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, write(res[i]))
                .Times(Exactly(1));
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0040, command);

    loop();
    loop();

    EXPECT_EQ(8, adcDataBuffer.allocatedSize);
    EXPECT_EQ(0x1234, adcDataBuffer.adcDataBuffer[0]);
    EXPECT_EQ(0x5678, adcDataBuffer.adcDataBuffer[1]);
    EXPECT_EQ(0x9ABC, adcDataBuffer.adcDataBuffer[2]);
    EXPECT_EQ(0xDEF1, adcDataBuffer.adcDataBuffer[3]);
    EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[4]);
    EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[5]);
}
TEST(UART, StopIntermittentConversion_By_SPIReset) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    EXPECT_CALL(*(dispatcher.spiCommand)->sleep, sleep(_))
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->sleep, sleepMillisecond(_))
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->sleep, sleepMicrosecond(_))
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, start())
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, stop())
        .Times(AnyNumber());
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, read())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, readMillisecond())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*(dispatcher.spiCommand)->stopwatch, readMicrosecond())
        .Times(AnyNumber())
        .WillRepeatedly(Return(100));

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
        .Times(Exactly(15))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
        .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(DoAll(InvokeWithoutArgs(StopIntermittentConversion), Return(1)));

    {
        InSequence s;

        // data pattern is same as test of StartIntermittentConversion
        for (int i = 0x12; i <= 0xBC; i = i + 0x44) {
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00));
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02));
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
            EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(_))
                .Times(Exactly(2))
                .WillOnce(Return(i))
                .WillOnce(Return(i+0x22));
        }
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(_))
            .Times(Exactly(2))
            .WillOnce(Return(0xDE))
            .WillOnce(Return(0xF1));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00));
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02));
        // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
        // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(_))
        //     .Times(Exactly(2))
        //     .WillOnce(Return(0x23))
        //     .WillOnce(Return(0x45));
        //

        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x7F)); // SPIResetCommand 0
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 1
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 2
    }

    uint8_t cmd1[] = { 0x10, 0x70, 0x02, 0x00, 0x04, 0x00, 0x0F, 0x42, 0x40, 0xE8 };
    uint8_t cmd2[] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd1[i]));
        }
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd2[i]));
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

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0040, command);

    loop();
    loop();

    EXPECT_EQ(8, adcDataBuffer.allocatedSize);
    EXPECT_EQ(0x1234, adcDataBuffer.adcDataBuffer[0]);
    EXPECT_EQ(0x5678, adcDataBuffer.adcDataBuffer[1]);
    EXPECT_EQ(0x9ABC, adcDataBuffer.adcDataBuffer[2]);
    EXPECT_EQ(0xDEF1, adcDataBuffer.adcDataBuffer[3]);
    EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[4]);
    EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[5]);
}

void StopADCDataDump() {
    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }
    isrPacketWatch();
}
TEST(UART, StopADCDataDump) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    adcDataBuffer.SetDataLength(16);
    adcDataBuffer.allocatedSize = 16;

    adcDataBuffer.adcDataBuffer[0] = 0x1234;
    adcDataBuffer.adcDataBuffer[1] = 0x5678;
    adcDataBuffer.adcDataBuffer[2] = 0x9ABC;
    adcDataBuffer.adcDataBuffer[3] = 0xDEF1;
    adcDataBuffer.adcDataBuffer[4] = 0x1234;
    adcDataBuffer.adcDataBuffer[5] = 0x5678;
    adcDataBuffer.adcDataBuffer[6] = 0x9ABC;
    adcDataBuffer.adcDataBuffer[7] = 0xDEF1;
    adcDataBuffer.adcDataBuffer[8] = 0x1234;
    adcDataBuffer.adcDataBuffer[9] = 0x5678;
    adcDataBuffer.adcDataBuffer[10] = 0x9ABC;
    adcDataBuffer.adcDataBuffer[11] = 0xDEF1;
    adcDataBuffer.adcDataBuffer[12] = 0x1234;
    adcDataBuffer.adcDataBuffer[13] = 0x5678;
    adcDataBuffer.adcDataBuffer[14] = 0x9ABC;
    adcDataBuffer.adcDataBuffer[15] = 0xDEF1;

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    uint8_t cmd1[] = { 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F };
    uint8_t cmd2[] = { 0x10, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6E };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd1[i]));
        }
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd2[i]));
        }
    }

    uint8_t res[] = {
        0x20, 0x80, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x50,
        0x30, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x96,
        0x30, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x96,
        0x20, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5E
    };
    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*4; i++) {
            if (i == 20) {
                EXPECT_CALL(uart, write(res[i]))
                    .Times(Exactly(1))
                    .WillOnce(InvokeWithoutArgs(StopADCDataDump));
            }
            else {
                EXPECT_CALL(uart, write(res[i]))
                    .Times(Exactly(1));
            }
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0080, command);

    loop();
    // dispatch StopADCDataDump
    loop();
}
TEST(UART, StopADCDataDump_By_SPIReset) {
    EXPECT_CALL(packetWatchTimer, attach(_, _)).Times(AnyNumber());
    EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
        .WillRepeatedly(Invoke(_attach));

    adcDataBuffer.SetDataLength(16);
    adcDataBuffer.allocatedSize = 16;

    adcDataBuffer.adcDataBuffer[0] = 0x1234;
    adcDataBuffer.adcDataBuffer[1] = 0x5678;
    adcDataBuffer.adcDataBuffer[2] = 0x9ABC;
    adcDataBuffer.adcDataBuffer[3] = 0xDEF1;
    adcDataBuffer.adcDataBuffer[4] = 0x1234;
    adcDataBuffer.adcDataBuffer[5] = 0x5678;
    adcDataBuffer.adcDataBuffer[6] = 0x9ABC;
    adcDataBuffer.adcDataBuffer[7] = 0xDEF1;
    adcDataBuffer.adcDataBuffer[8] = 0x1234;
    adcDataBuffer.adcDataBuffer[9] = 0x5678;
    adcDataBuffer.adcDataBuffer[10] = 0x9ABC;
    adcDataBuffer.adcDataBuffer[11] = 0xDEF1;
    adcDataBuffer.adcDataBuffer[12] = 0x1234;
    adcDataBuffer.adcDataBuffer[13] = 0x5678;
    adcDataBuffer.adcDataBuffer[14] = 0x9ABC;
    adcDataBuffer.adcDataBuffer[15] = 0xDEF1;

    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
            EXPECT_CALL(uart, readable())
                .Times(Exactly(2))
                .WillOnce(Return(1))
                .WillOnce(Return(0));
        }
    }

    {
        InSequence s;
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x7F)); // SPIResetCommand 0
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 1
        EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 2
    }

    uint8_t cmd1[] = { 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F };
    uint8_t cmd2[] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF };
    {
        InSequence s;
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd1[i]));
        }
        for (int i = 0; i < PACKET_SIZE+1; i++) {
            EXPECT_CALL(uart, read())
                .WillOnce(Return(cmd2[i]));
        }
    }

    uint8_t res[] = {
        0x20, 0x80, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x50,
        0x30, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x96,
        0x30, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x96,
        0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDF
    };
    {
        InSequence s;
        for (int i = 0; i < (PACKET_SIZE+1)*4; i++) {
            if (i == 20) {
                EXPECT_CALL(uart, write(res[i]))
                    .Times(Exactly(1))
                    .WillOnce(InvokeWithoutArgs(StopADCDataDump));
            }
            else {
                EXPECT_CALL(uart, write(res[i]))
                    .Times(Exactly(1));
            }
        }
    }

    setup();

    for (int i = 0; i < PACKET_SIZE+1; i++) {
        fpIsrRx();
    }

    EXPECT_EQ(0x2, receiveDataStatus);
    isrPacketWatch();
    EXPECT_EQ(0x0, receiveDataStatus);
    EXPECT_EQ(0x0080, command);

    loop();
    // dispatch SPIResetCommand
    loop();
}

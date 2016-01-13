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

// TEST(UART, StartADCDataDump) {
//     EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
//         .WillRepeatedly(Invoke(_attach));
// 
//     adcDataBuffer.SetDataLength(5);
//     adcDataBuffer.allocatedSize = 8;
// 
//     adcDataBuffer.adcDataBuffer[0] = 0x1234;
//     adcDataBuffer.adcDataBuffer[1] = 0x5678;
//     adcDataBuffer.adcDataBuffer[2] = 0x9ABC;
//     adcDataBuffer.adcDataBuffer[3] = 0xDEF1;
//     adcDataBuffer.adcDataBuffer[4] = 0x2345;
//     adcDataBuffer.adcDataBuffer[5] = 0x0000;
// 
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, readable())
//                 .Times(Exactly(2))
//                 .WillOnce(Return(1))
//                 .WillOnce(Return(0));
//         }
//     }
// 
//     uint8_t cmd[] = { 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd[i]));
//         }
//     }
// 
//     uint8_t res[] = {
//         0x20, 0x80, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x5B,
//         0x30, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x96,
//         0x30, 0x23, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67
//     };
//     {
//         InSequence s;
//         for (int i = 0; i < (PACKET_SIZE+1)*3; i++) {
//             EXPECT_CALL(uart, write(res[i]))
//                 .Times(Exactly(1));
//         }
//     }
// 
//     setup();
// 
//     for (int i = 0; i < PACKET_SIZE+1; i ++) {
//         fpIsrRx();
//     }
// 
//     EXPECT_EQ(0x2, receiveDataStatus);
//     isrPacketWatch();
//     EXPECT_EQ(0x0, receiveDataStatus);
//     EXPECT_EQ(0x0040, command);
// 
//     loop();
// }
// 
// void StopSingleConversion() {
//     for (int i = 0; i < PACKET_SIZE+1; i ++) {
//         fpIsrRx();
//     }
//     isrPacketWatch();
// }
// TEST(UART, StopSingleConversion) {
//     EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
//         .WillRepeatedly(Invoke(_attach));
// 
//     {
//         InSequence s;
//         for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
//             EXPECT_CALL(uart, readable())
//                 .Times(Exactly(2))
//                 .WillOnce(Return(1))
//                 .WillOnce(Return(0));
//         }
//     }
// 
//     EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
//         .WillOnce(Return(1))
//         .WillOnce(DoAll(InvokeWithoutArgs(StopSingleConversion), Return(1)))
//         .WillRepeatedly(Return(1));
// 
//     {
//         InSequence s;
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00)); // CTRL Register Address
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02)); // { chsel, mode }
//     }
// 
//     uint8_t cmd1[] = { 0x10, 0x50, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D };
//     uint8_t cmd2[] = { 0x10, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9E };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd1[i]));
//         }
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd2[i]));
//         }
//     }
// 
//     uint8_t res[] = { 0x20, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8E };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, write(res[i]))
//                 .Times(Exactly(1));
//         }
//     }
// 
//     setup();
// 
//     for (int i = 0; i < PACKET_SIZE+1; i ++) {
//         fpIsrRx();
//     }
// 
//     EXPECT_EQ(0x2, receiveDataStatus);
//     isrPacketWatch();
//     EXPECT_EQ(0x0, receiveDataStatus);
//     EXPECT_EQ(0x0010, command);
// 
//     // start single conversion
//     loop();
//     // abort
//     loop();
// }
// TEST(UART, StopSingleConversion_By_SPIReset) {
//     EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
//         .WillRepeatedly(Invoke(_attach));
// 
//     {
//         InSequence s;
//         for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
//             EXPECT_CALL(uart, readable())
//                 .Times(Exactly(2))
//                 .WillOnce(Return(1))
//                 .WillOnce(Return(0));
//         }
//     }
// 
//     EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
//         .WillOnce(Return(1))
//         .WillOnce(DoAll(InvokeWithoutArgs(StopSingleConversion), Return(1)))
//         .WillRepeatedly(Return(1));
// 
//     {
//         InSequence s;
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00)); // CTRL Register Address
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02)); // { chsel, mode }
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x7F)); // SPIResetCommand 0
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 1
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 2
//     }
// 
//     uint8_t cmd1[] = { 0x10, 0x50, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D };
//     uint8_t cmd2[] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd1[i]));
//         }
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd2[i]));
//         }
//     }
// 
//     uint8_t res[] = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDF };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, write(res[i]))
//                 .Times(Exactly(1));
//         }
//     }
// 
//     setup();
// 
//     for (int i = 0; i < PACKET_SIZE+1; i ++) {
//         fpIsrRx();
//     }
// 
//     EXPECT_EQ(0x2, receiveDataStatus);
//     isrPacketWatch();
//     EXPECT_EQ(0x0, receiveDataStatus);
//     EXPECT_EQ(0x0010, command);
// 
//     // start single conversion
//     loop();
//     // abort
//     loop();
// }
// 
// void StopContinuousConversion() {
//     for (int i = 0; i < PACKET_SIZE+1; i ++) {
//         fpIsrRx();
//     }
//     isrPacketWatch();
// }
// TEST(UART, StopContinuousConversion) {
//     EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
//         .WillRepeatedly(Invoke(_attach));
// 
//     {
//         InSequence s;
//         for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
//             EXPECT_CALL(uart, readable())
//                 .Times(Exactly(2))
//                 .WillOnce(Return(1))
//                 .WillOnce(Return(0));
//         }
//     }
// 
//     EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
//         .Times(Exactly(15))
//         .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
//         .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
//         .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
//         .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
//         .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(DoAll(InvokeWithoutArgs(StopContinuousConversion), Return(1)));
// 
//     {
//         InSequence s;
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00)); // CTRL Register Address
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02)); // { chsel, mode }
// 
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
//             .Times(Exactly(2))
//             .WillOnce(Return(0x12))
//             .WillOnce(Return(0x34));
// 
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
//             .Times(Exactly(2))
//             .WillOnce(Return(0x56))
//             .WillOnce(Return(0x78));
// 
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
//             .Times(Exactly(2))
//             .WillOnce(Return(0x9A))
//             .WillOnce(Return(0xBC));
// 
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
//             .Times(Exactly(2))
//             .WillOnce(Return(0xDE))
//             .WillOnce(Return(0xF1));
// 
//         // This transaction will be aborted
//         // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
//         // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
//         //     .Times(Exactly(2))
//         //     .WillOnce(Return(0x23))
//         //     .WillOnce(Return(0x45));
//     }
// 
//     uint8_t cmd1[] = { 0x10, 0x60, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x89 };
//     uint8_t cmd2[] = { 0x10, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8E };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd1[i]));
//         }
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd2[i]));
//         }
//     }
// 
//     uint8_t res[] = { 0x20, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, write(res[i]))
//                 .Times(Exactly(1));
//         }
//     }
// 
//     setup();
// 
//     for (int i = 0; i < PACKET_SIZE+1; i ++) {
//         fpIsrRx();
//     }
// 
//     EXPECT_EQ(0x2, receiveDataStatus);
//     isrPacketWatch();
//     EXPECT_EQ(0x0, receiveDataStatus);
//     EXPECT_EQ(0x0020, command);
// 
//     loop();
//     loop();
// 
//     EXPECT_EQ(8, adcDataBuffer.allocatedSize);
//     EXPECT_EQ(0x1234, adcDataBuffer.adcDataBuffer[0]);
//     EXPECT_EQ(0x5678, adcDataBuffer.adcDataBuffer[1]);
//     EXPECT_EQ(0x9ABC, adcDataBuffer.adcDataBuffer[2]);
//     EXPECT_EQ(0xDEF1, adcDataBuffer.adcDataBuffer[3]);
//     EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[4]);
//     EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[5]);
// }
// TEST(UART, StopContinuousConversion_By_SPIReset) {
//     EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
//         .WillRepeatedly(Invoke(_attach));
// 
//     {
//         InSequence s;
//         for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
//             EXPECT_CALL(uart, readable())
//                 .Times(Exactly(2))
//                 .WillOnce(Return(1))
//                 .WillOnce(Return(0));
//         }
//     }
// 
//     EXPECT_CALL(*(dispatcher.spiCommand)->rdyb, read())
//         .Times(Exactly(15))
//         .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
//         .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
//         .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
//         .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(Return(0))
//         .WillOnce(Return(1)).WillOnce(Return(1)).WillOnce(DoAll(InvokeWithoutArgs(StopContinuousConversion), Return(1)));
// 
//     {
//         InSequence s;
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00)); // CTRL Register Address
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x02)); // { chsel, mode }
// 
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
//             .Times(Exactly(2))
//             .WillOnce(Return(0x12))
//             .WillOnce(Return(0x34));
// 
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
//             .Times(Exactly(2))
//             .WillOnce(Return(0x56))
//             .WillOnce(Return(0x78));
// 
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
//             .Times(Exactly(2))
//             .WillOnce(Return(0x9A))
//             .WillOnce(Return(0xBC));
// 
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
//             .Times(Exactly(2))
//             .WillOnce(Return(0xDE))
//             .WillOnce(Return(0xF1));
// 
//         // This transaction will be aborted
//         // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x1C));
//         // EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x00))
//         //     .Times(Exactly(2))
//         //     .WillOnce(Return(0x23))
//         //     .WillOnce(Return(0x45));
// 
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x7F)); // SPIResetCommand 0
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 1
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 2
//     }
// 
//     uint8_t cmd1[] = { 0x10, 0x60, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x89 };
//     uint8_t cmd2[] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd1[i]));
//         }
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd2[i]));
//         }
//     }
// 
//     uint8_t res[] = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDF };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, write(res[i]))
//                 .Times(Exactly(1));
//         }
//     }
// 
//     setup();
// 
//     for (int i = 0; i < PACKET_SIZE+1; i ++) {
//         fpIsrRx();
//     }
// 
//     EXPECT_EQ(0x2, receiveDataStatus);
//     isrPacketWatch();
//     EXPECT_EQ(0x0, receiveDataStatus);
//     EXPECT_EQ(0x0020, command);
// 
//     loop();
//     loop();
// 
//     EXPECT_EQ(8, adcDataBuffer.allocatedSize);
//     EXPECT_EQ(0x1234, adcDataBuffer.adcDataBuffer[0]);
//     EXPECT_EQ(0x5678, adcDataBuffer.adcDataBuffer[1]);
//     EXPECT_EQ(0x9ABC, adcDataBuffer.adcDataBuffer[2]);
//     EXPECT_EQ(0xDEF1, adcDataBuffer.adcDataBuffer[3]);
//     EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[4]);
//     EXPECT_EQ(0x0000, adcDataBuffer.adcDataBuffer[5]);
// }
// 
// void StopADCDataDump() {
//     for (int i = 0; i < PACKET_SIZE+1; i ++) {
//         fpIsrRx();
//     }
//     isrPacketWatch();
// }
// TEST(UART, StopADCDataDump) {
//     EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
//         .WillRepeatedly(Invoke(_attach));
// 
//     adcDataBuffer.SetDataLength(16);
//     adcDataBuffer.allocatedSize = 16;
// 
//     adcDataBuffer.adcDataBuffer[0] = 0x1234;
//     adcDataBuffer.adcDataBuffer[1] = 0x5678;
//     adcDataBuffer.adcDataBuffer[2] = 0x9ABC;
//     adcDataBuffer.adcDataBuffer[3] = 0xDEF1;
//     adcDataBuffer.adcDataBuffer[4] = 0x1234;
//     adcDataBuffer.adcDataBuffer[5] = 0x5678;
//     adcDataBuffer.adcDataBuffer[6] = 0x9ABC;
//     adcDataBuffer.adcDataBuffer[7] = 0xDEF1;
//     adcDataBuffer.adcDataBuffer[8] = 0x1234;
//     adcDataBuffer.adcDataBuffer[9] = 0x5678;
//     adcDataBuffer.adcDataBuffer[10] = 0x9ABC;
//     adcDataBuffer.adcDataBuffer[11] = 0xDEF1;
//     adcDataBuffer.adcDataBuffer[12] = 0x1234;
//     adcDataBuffer.adcDataBuffer[13] = 0x5678;
//     adcDataBuffer.adcDataBuffer[14] = 0x9ABC;
//     adcDataBuffer.adcDataBuffer[15] = 0xDEF1;
// 
//     {
//         InSequence s;
//         for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
//             EXPECT_CALL(uart, readable())
//                 .Times(Exactly(2))
//                 .WillOnce(Return(1))
//                 .WillOnce(Return(0));
//         }
//     }
// 
//     uint8_t cmd1[] = { 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F };
//     uint8_t cmd2[] = { 0x10, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6E };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd1[i]));
//         }
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd2[i]));
//         }
//     }
// 
//     uint8_t res[] = {
//         0x20, 0x80, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x50,
//         0x30, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x96,
//         0x30, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x96,
//         0x20, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5E
//     };
//     {
//         InSequence s;
//         for (int i = 0; i < (PACKET_SIZE+1)*4; i++) {
//             if (i == 20) {
//                 EXPECT_CALL(uart, write(res[i]))
//                     .Times(Exactly(1))
//                     .WillOnce(InvokeWithoutArgs(StopADCDataDump));
//             }
//             else {
//                 EXPECT_CALL(uart, write(res[i]))
//                     .Times(Exactly(1));
//             }
//         }
//     }
// 
//     setup();
// 
//     for (int i = 0; i < PACKET_SIZE+1; i ++) {
//         fpIsrRx();
//     }
// 
//     EXPECT_EQ(0x2, receiveDataStatus);
//     isrPacketWatch();
//     EXPECT_EQ(0x0, receiveDataStatus);
//     EXPECT_EQ(0x0040, command);
// 
//     loop();
//     // dispatch StopADCDataDump
//     loop();
// }
// TEST(UART, StopADCDataDump_By_SPIReset) {
//     EXPECT_CALL(uart, attach(_, ::Serial::RxIrq))
//         .WillRepeatedly(Invoke(_attach));
// 
//     adcDataBuffer.SetDataLength(16);
//     adcDataBuffer.allocatedSize = 16;
// 
//     adcDataBuffer.adcDataBuffer[0] = 0x1234;
//     adcDataBuffer.adcDataBuffer[1] = 0x5678;
//     adcDataBuffer.adcDataBuffer[2] = 0x9ABC;
//     adcDataBuffer.adcDataBuffer[3] = 0xDEF1;
//     adcDataBuffer.adcDataBuffer[4] = 0x1234;
//     adcDataBuffer.adcDataBuffer[5] = 0x5678;
//     adcDataBuffer.adcDataBuffer[6] = 0x9ABC;
//     adcDataBuffer.adcDataBuffer[7] = 0xDEF1;
//     adcDataBuffer.adcDataBuffer[8] = 0x1234;
//     adcDataBuffer.adcDataBuffer[9] = 0x5678;
//     adcDataBuffer.adcDataBuffer[10] = 0x9ABC;
//     adcDataBuffer.adcDataBuffer[11] = 0xDEF1;
//     adcDataBuffer.adcDataBuffer[12] = 0x1234;
//     adcDataBuffer.adcDataBuffer[13] = 0x5678;
//     adcDataBuffer.adcDataBuffer[14] = 0x9ABC;
//     adcDataBuffer.adcDataBuffer[15] = 0xDEF1;
// 
//     {
//         InSequence s;
//         for (int i = 0; i < (PACKET_SIZE+1)*2; i++) {
//             EXPECT_CALL(uart, readable())
//                 .Times(Exactly(2))
//                 .WillOnce(Return(1))
//                 .WillOnce(Return(0));
//         }
//     }
// 
//     {
//         InSequence s;
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0x7F)); // SPIResetCommand 0
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 1
//         EXPECT_CALL(*(dispatcher.spiCommand)->spi, write(0xFF)); // SPIResetCommand 2
//     }
// 
//     uint8_t cmd1[] = { 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F };
//     uint8_t cmd2[] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF };
//     {
//         InSequence s;
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd1[i]));
//         }
//         for (int i = 0; i < PACKET_SIZE+1; i++) {
//             EXPECT_CALL(uart, read())
//                 .WillOnce(Return(cmd2[i]));
//         }
//     }
// 
//     uint8_t res[] = {
//         0x20, 0x80, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x50,
//         0x30, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x96,
//         0x30, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x96,
//         0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDF
//     };
//     {
//         InSequence s;
//         for (int i = 0; i < (PACKET_SIZE+1)*4; i++) {
//             if (i == 20) {
//                 EXPECT_CALL(uart, write(res[i]))
//                     .Times(Exactly(1))
//                     .WillOnce(InvokeWithoutArgs(StopADCDataDump));
//             }
//             else {
//                 EXPECT_CALL(uart, write(res[i]))
//                     .Times(Exactly(1));
//             }
//         }
//     }
// 
//     setup();
// 
//     for (int i = 0; i < PACKET_SIZE+1; i ++) {
//         fpIsrRx();
//     }
// 
//     EXPECT_EQ(0x2, receiveDataStatus);
//     isrPacketWatch();
//     EXPECT_EQ(0x0, receiveDataStatus);
//     EXPECT_EQ(0x0040, command);
// 
//     loop();
//     // dispatch SPIResetCommand
//     loop();
// }

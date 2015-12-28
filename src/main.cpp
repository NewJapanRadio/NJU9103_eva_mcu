#include <string.h>

#include "nju9103.h"

NJRC_STATIC Packet rxPacket;
NJRC_STATIC ReceiveDataStatus receiveDataStatus;
NJRC_STATIC Command command;

NJRC_STATIC ::Serial uart;
NJRC_STATIC ::Timer packetWatchTimer;
NJRC_STATIC ::Dispatcher dispatcher;
NJRC_STATIC ::ADCDataBuffer adcDataBuffer;

void setup() {
    receiveDataStatus = 0;
    command = 0;

    // set Rx interrupt
    uart.attach(isrRx, ::Serial::RxIrq);
    packetWatchTimer.attach(isrPacketWatch, 300);
}

void loop() {
    Packet packet;

    if (command != 0) {
        ::Dispatcher::Status status;
        status = dispatcher.Dispatch(&command, &rxPacket, &packet, &adcDataBuffer);

        if (status != ::Dispatcher::Abort) {
            sendPacket(&packet);
            if (packet.OpCode == OP_START_ADC_DATA_DUMP) {
                adcDataBuffer.Dump(sendPacket);
            }
        }
    }
}

void isrRx() {
    static char buf[PACKET_SIZE];
    static uint8_t count = 0;
    while (uart.readable()) {
        if (count < sizeof(buf)) {
            buf[count] = uart.read();
        }
        else if (count == sizeof(buf)) {
            receiveDataStatus = RX_STATUS_DATA_RECEIVED;
            if (uart.read() == calculateChkSum((uint8_t*)buf)) {
                rxPacket.Header = buf[0];
                rxPacket.OpCode = buf[1];
                rxPacket.Param  = buf[2];
                rxPacket.Byte2  = buf[3];
                rxPacket.Byte3  = buf[4];
                rxPacket.Byte4  = buf[5];
                rxPacket.Byte5  = buf[6];
                rxPacket.Byte6  = buf[7];
                rxPacket.Byte7  = buf[8];
            }
            else {
                receiveDataStatus |= RX_STATUS_CHKSUM_ERROR;
            }
        }
        count = (count + 1) % (sizeof(buf) + 1);
    }
}

void isrPacketWatch() {
    if (receiveDataStatus & RX_STATUS_DATA_RECEIVED) {
        receiveDataStatus ^= RX_STATUS_DATA_RECEIVED;
        if ((receiveDataStatus & RX_STATUS_CHKSUM_ERROR) == 0) {
            decodeCommand(&rxPacket, &command);
            if ((command & CMD_RESET) != 0) {
                if ((command & CMD_START_SINGLE) != 0) {
                    dispatcher.SetAbortRequest();
                }
                if ((command & CMD_START_CONTINUOUS) != 0) {
                    dispatcher.SetAbortRequest();
                }
                if ((command & CMD_START_DUMP) != 0) {
                    adcDataBuffer.SetAbortRequest();
                }
            }
            else if ((command & CMD_STOP_SINGLE) != 0) {
                if ((command & CMD_START_SINGLE) != 0) {
                    dispatcher.SetAbortRequest();
                }
            }
            else if ((command & CMD_STOP_CONTINUOUS) != 0) {
                if ((command & CMD_START_CONTINUOUS) != 0) {
                    dispatcher.SetAbortRequest();
                }
            }
            else if ((command & CMD_STOP_DUMP) != 0) {
                if ((command & CMD_START_DUMP) != 0) {
                    adcDataBuffer.SetAbortRequest();
                }
            }
        }
        else {
            receiveDataStatus ^= RX_STATUS_CHKSUM_ERROR;

            Packet packet;
            memset(&packet, 0, sizeof(Packet));
            packet.Header = ErrorHeader;
            packet.OpCode = OP_CHKSUM_ERROR;
            sendPacket(&packet);
        }
    }
}

uint8_t calculateChkSum(Packet *packet) {
    uint8_t chksum = 0;
    chksum = (chksum + packet->Header) & 0xFF;
    chksum = (chksum + packet->Byte0) & 0xFF;
    chksum = (chksum + packet->Byte1) & 0xFF;
    chksum = (chksum + packet->Byte2) & 0xFF;
    chksum = (chksum + packet->Byte3) & 0xFF;
    chksum = (chksum + packet->Byte4) & 0xFF;
    chksum = (chksum + packet->Byte5) & 0xFF;
    chksum = (chksum + packet->Byte6) & 0xFF;
    chksum = (chksum + packet->Byte7) & 0xFF;
    return ~chksum;
}

uint8_t calculateChkSum(uint8_t *data) {
    uint8_t chksum = 0;
    for (int i = 0; i < PACKET_SIZE; i++) {
        chksum = (chksum + *data++) & 0xFF;
    }
    return ~chksum;
}

void sendPacket(Packet *packet) {
    uart.write(packet->Header);
    uart.write(packet->Byte0);
    uart.write(packet->Byte1);
    uart.write(packet->Byte2);
    uart.write(packet->Byte3);
    uart.write(packet->Byte4);
    uart.write(packet->Byte5);
    uart.write(packet->Byte6);
    uart.write(packet->Byte7);
    uart.write(calculateChkSum(packet));
}

void decodeCommand(Packet *args, Command *cmd) {
    if (args->Header == CommandHeader) {
        switch (args->OpCode) {
            case OP_SPI_RESET :
                *cmd |= CMD_RESET;
                break;
            case OP_REGISTER_WRITE_8BIT :
                *cmd |= CMD_WRITE_8BIT;
                break;
            case OP_REGISTER_READ_8BIT :
                *cmd |= CMD_READ_8BIT;
                break;
            case OP_REGISTER_WRITE_16BIT :
                *cmd |= CMD_WRITE_16BIT;
                break;
            case OP_REGISTER_READ_16BIT :
                *cmd |= CMD_READ_16BIT;
                break;
            case OP_START_SINGLE_CONVERSION :
                *cmd |= CMD_START_SINGLE;
                break;
            case OP_START_CONTINUOUS_CONVERSION :
                *cmd |= CMD_START_CONTINUOUS;
                break;
            case OP_START_ADC_DATA_DUMP :
                *cmd |= CMD_START_DUMP;
                break;
            case OP_STOP_SINGLE_CONVERSION :
                *cmd |= CMD_STOP_SINGLE;
                break;
            case OP_STOP_CONTINUOUS_CONVERSION :
                *cmd |= CMD_STOP_CONTINUOUS;
                break;
            case OP_STOP_ADC_DATA_DUMP :
                *cmd |= CMD_STOP_DUMP;
                break;
            default :
                *cmd |= CMD_UNKNOWN;
                break;
        }
    }
    else {
        *cmd |= CMD_UNKNOWN;
    }
}

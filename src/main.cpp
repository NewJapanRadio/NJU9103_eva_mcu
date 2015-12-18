#include <new>

#include "nju9103_eva.h"
#include "serial_wrapper.h"
#include "command_dispatcher.h"
#include "timer_wrapper.h"

static uint8_t rx_buffer[PACKET_SIZE];
static ReceiveDataStatus receiveDataStatus;
static Command command;

static ::Serial *uart;
static ::Timer *packetWatchTimer;
static ::CommandDispatcher *dispatcher;

void setup() {
    receiveDataStatus = 0;
    command = 0;

    uart = new ::Serial(UART_BAUDRATE, UART_BITS, UART_PARITY, UART_STOP);
    packetWatchTimer = new ::Timer();
    dispatcher = new ::CommandDispatcher();

    // set Rx interrupt
    uart->attach(isrRx, ::Serial::RxIrq);
    packetWatchTimer->attach(isrPacketWatch, 300);
}

void loop() {
    dispatchPacket();
}

static uint8_t calculateChkSum(uint8_t *data) {
    uint8_t chksum = 0;
    for (int i = 0; i < PACKET_SIZE; i++) {
        chksum = (chksum + *data++) & 0xFF;
    }
    return ~chksum;
}

static void isrRx() {
    static char buf[PACKET_SIZE];
    static uint8_t count = 0;
    while (uart->readable()) {
        if (count < sizeof(buf)) {
            buf[count] = uart->read();
        }
        else if (count == sizeof(buf)) {
            receiveDataStatus = RX_STATUS_DATA_RECEIVED;
            if (uart->read() == calculateChkSum((uint8_t*)buf)) {
                memcpy(rx_buffer, buf, sizeof(uint8_t) * PACKET_SIZE);
            }
            else {
                receiveDataStatus |= RX_STATUS_CHKSUM_ERROR;
            }
        }
        count = (count + 1) % (sizeof(buf) + 1);
    }
}

static void isrPacketWatch() {
    if (receiveDataStatus & RX_STATUS_DATA_RECEIVED) {
        receiveDataStatus ^= RX_STATUS_DATA_RECEIVED;
        if ((receiveDataStatus & RX_STATUS_CHKSUM_ERROR) == 0) {
            uart->println("[PASS] receive data is valid.");
            switch (rx_buffer[0]) {
                case OP_REGISTER_WRITE_BYTE :
                    command |= CMD_WRITE_BYTE;
                    break;
                case OP_REGISTER_READ_BYTE :
                    command |= CMD_READ_BYTE;
                    break;
                case OP_REGISTER_WRITE_SHORT :
                    command |= CMD_WRITE_SHORT;
                    break;
                case OP_REGISTER_READ_SHORT :
                    command |= CMD_READ_SHORT;
                    break;
                case OP_START_SINGLE_CONVERSION :
                    command |= CMD_START_SINGLE;
                    break;
                case OP_START_CONTINUOUS_CONVERSION :
                    command |= CMD_START_CONTINUOUS;
                    break;
                case OP_START_ADC_DATA_DUMP :
                    command |= CMD_START_DUMP;
                    break;
                case OP_STOP_SINGLE_CONVERSION :
                    dispatcher->setAbortRequest();
                    command |= CMD_STOP_SINGLE;
                    break;
                case OP_STOP_CONTINUOUS_CONVERSION :
                    command |= CMD_STOP_CONTINUOUS;
                    break;
                case OP_STOP_ADC_DATA_DUMP :
                    command |= CMD_STOP_DUMP;
                    break;
                default :
                    command |= CMD_UNKNOWN;
                    break;
            }
        }
        else {
            receiveDataStatus ^= RX_STATUS_CHKSUM_ERROR;
            uart->println("[ERROR] receive data is invalid.");
        }
    }
}

static void dispatchPacket() {
    if (command != 0) {
        if (command & CMD_WRITE_BYTE) {
            command ^= CMD_WRITE_BYTE;
            uart->println("RegisterWriteByte command");
            dispatcher->RegisterWriteByte(rx_buffer[1], rx_buffer[2]);
        }
        else if (command & CMD_READ_BYTE) {
            command ^= CMD_READ_BYTE;
            uart->println("RegisterReadByte command");
            uint8_t rd = 0;
            dispatcher->RegisterReadByte(rx_buffer[1], &rd);
            char buf[16];
            sprintf(buf, "RD:0x%X", rd);
            uart->println(buf);
        }
        else if (command & CMD_WRITE_SHORT) {
            command ^= CMD_WRITE_SHORT;
            uart->println("RegisterWriteShort command");
            uint16_t wd = (uint16_t)(rx_buffer[2] + (rx_buffer[3] << 8));
            dispatcher->RegisterWriteShort(rx_buffer[1], wd);
        }
        else if (command & CMD_READ_SHORT) {
            command ^= CMD_READ_SHORT;
            uart->println("RegisterReadShort command");
            uint16_t rd = 0;
            dispatcher->RegisterReadShort(rx_buffer[1], &rd);
            char buf[16];
            sprintf(buf, "RD:0x%X", rd);
            uart->println(buf);
        }
        else if (command & CMD_START_SINGLE) {
            command ^= CMD_START_SINGLE;
            uart->println("StartSingleConversion command");
            uint16_t rd = 0;
            dispatcher->StartSingleConversion(rx_buffer[1], &rd);
            char buf[16];
            sprintf(buf, "ADCDATA:0x%X", rd);
            uart->println(buf);
        }
        else if (command & CMD_START_CONTINUOUS) {
            command ^= CMD_START_CONTINUOUS;
            uart->println("StartContinuousConversion command");
        }
        else if (command & CMD_START_DUMP) {
            command ^= CMD_START_DUMP;
            uart->println("StartADCDataDump command");
        }
        else if (command & CMD_STOP_SINGLE) {
            command ^= CMD_STOP_SINGLE;
            uart->println("StopSingleConversion command");
        }
        else if (command & CMD_STOP_CONTINUOUS) {
            command ^= CMD_STOP_CONTINUOUS;
            uart->println("StopContinuousConversion command");
        }
        else if (command & CMD_STOP_DUMP) {
            command ^= CMD_STOP_DUMP;
            uart->println("StopADCDataDump command");
        }
        else if (command & CMD_UNKNOWN) {
            command ^= CMD_UNKNOWN;
            uart->println("[ERROR] Invalid Command");
        }
    }
}

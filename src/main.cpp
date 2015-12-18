#include <new>

#include "nju9103_eva.h"
#include "serial_wrapper.h"
#include "spi_wrapper.h"
#include "timer_wrapper.h"

static uint8_t rx_buffer[PACKET_SIZE];
static ReceiveDataStatus receiveDataStatus;
static Command command;

static ::Serial *uart;
static ::SPI *spi;
static ::Timer *mainProc;
static ::Timer *subProc;

void setup() {
    receiveDataStatus = 0;
    command = 0;

    uart = new ::Serial(UART_BAUDRATE, UART_BITS, UART_PARITY, UART_STOP);
    spi = new ::SPI();
    spi->mode(::SPI::Mode1);
    spi->frequency(1E6);
    mainProc = new ::Timer();
    subProc = new ::Timer();

    // set Rx interrupt
    uart->attach(isrRx, ::Serial::RxIrq);
    // set Timer interrupt
    mainProc->attach(isrMainProc, 300);
    subProc->attach(isrSubProc, 300);
}

void loop() {
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

static void isrMainProc() {
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
                case OP_STOP_CONTINUOUS_CONVERSION :
                    command |= CMD_STOP_CONTINUOUS;
                    break;
                case OP_STOP_ADC_DATA_DUMP :
                    command |= CMD_STOP_DUMP;
                    break;
                default :
                    break;
            }
        }
        else {
            receiveDataStatus ^= RX_STATUS_CHKSUM_ERROR;
            uart->println("[ERROR] receive data is invalid.");
        }
    }
}

static void isrSubProc() {
    if (command != 0) {
        int isValidCommand = 0;
        if (command & CMD_WRITE_BYTE) {
            command ^= CMD_WRITE_BYTE;
            uart->println("RegisterWriteByte command");
            RegisterWriteByte(rx_buffer[1], rx_buffer[2]);
            isValidCommand = 1;
        }
        else if (command & CMD_READ_BYTE) {
            command ^= CMD_READ_BYTE;
            uart->println("RegisterReadByte command");
            uint8_t rd = 0;
            RegisterReadByte(rx_buffer[1], &rd);
            char buf[16];
            sprintf(buf, "RD:0x%X", rd);
            uart->println(buf);
            isValidCommand = 1;
        }
        else if (command & CMD_WRITE_SHORT) {
            command ^= CMD_WRITE_SHORT;
            uart->println("RegisterWriteShort command");
            isValidCommand = 1;
        }
        else if (command & CMD_READ_SHORT) {
            command ^= CMD_READ_SHORT;
            uart->println("RegisterReadShort command");
            isValidCommand = 1;
        }
        else if (command & CMD_START_SINGLE) {
            command ^= CMD_START_SINGLE;
            uart->println("StartSingleConversion command");
            isValidCommand = 1;
        }
        else if (command & CMD_START_CONTINUOUS) {
            command ^= CMD_START_CONTINUOUS;
            uart->println("StartContinuousConversion command");
            isValidCommand = 1;
        }
        else if (command & CMD_START_DUMP) {
            command ^= CMD_START_DUMP;
            uart->println("StartADCDataDump command");
            isValidCommand = 1;
        }
        else if (command & CMD_STOP_CONTINUOUS) {
            command ^= CMD_STOP_CONTINUOUS;
            uart->println("StopContinuousConversion command");
            isValidCommand = 1;
        }
        else if (command & CMD_STOP_DUMP) {
            command ^= CMD_STOP_DUMP;
            uart->println("StopADCDataDump command");
            isValidCommand = 1;
        }

        if ((isValidCommand == 0) || (command != 0)) {
            uart->println("[ERROR] Invalid Command");
        }
    }
}

static CommandStatus RegisterWriteByte(uint8_t address, uint8_t regData) {
    CommandStatus status = Error;
    if ((address & 0xF0) == 0) {
        spi->write((uint8_t)(address << 4));
        spi->write(regData);
        status = Success;
    }
    return status;
}

static CommandStatus RegisterReadByte(uint8_t address, uint8_t *regData) {
    CommandStatus status = Error;
    if ((address & 0xF0) == 0) {
        spi->write((uint8_t)(address << 4) + 0x08);
        *regData = spi->write(0xFF);
        status = Success;
    }
    return status;
}

// CommandStatus RegisterWriteShort(uint8_t address, uint16_t regData);
// CommandStatus RegisterReadShort(uint8_t address, uint16_t *regData);
// CommandStatus StartSingleConversion(uint8_t channel, uint16_t *adcData);
// CommandStatus StartContinuousConversion(uint8_t channel, uint16_t *buf, uint16_t length, uint16_t *resultLength);
// CommandStatus StartADCDataDump();
// CommandStatus StopContinuousConversion();
// CommandStatus StopADCDataDump();


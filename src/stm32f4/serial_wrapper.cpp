#include "serial_wrapper.h"

#define UART_BAUDRATE 115200
#define UART_BITS 8
#define UART_PARITY mbed::Serial::None
#define UART_STOP 1

NewJapanRadio::Serial::Serial() {
    serial = new mbed::Serial(USBTX, USBRX);
    serial->baud(UART_BAUDRATE);
    serial->format(UART_BITS, UART_PARITY, UART_STOP);
}

NewJapanRadio::Serial::~Serial() {
    delete serial;
}

uint8_t NewJapanRadio::Serial::read() {
    return serial->getc();
}

int8_t NewJapanRadio::Serial::readable() {
    return serial->readable();
}

int8_t NewJapanRadio::Serial::writeable() {
    return serial->writeable();
}

void NewJapanRadio::Serial::write(const uint8_t data) {
    serial->putc(data);
}

void NewJapanRadio::Serial::print(const char* str) {
    serial->printf(str);
}

void NewJapanRadio::Serial::println(const char* str) {
    serial->printf("%s\n", str);
}

void NewJapanRadio::Serial::attach(void (*fptr)(void), IrqType type = RxIrq) {
    mbed::Serial::IrqType mbedIrqType;
    switch (type) {
        case RxIrq:
            mbedIrqType = mbed::Serial::RxIrq;
            break;
        case TxIrq:
            mbedIrqType = mbed::Serial::TxIrq;
            break;
        default:
            mbedIrqType = mbed::Serial::RxIrq;
            break;
    }

    serial->attach(fptr, mbedIrqType);
}

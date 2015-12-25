#include "serial_wrapper.h"

#define UART_BAUDRATE 115200
#define UART_BITS 8
#define UART_PARITY mbed::Serial::None
#define UART_STOP 1

::Serial::Serial() {
    serial = new mbed::Serial(USBTX, USBRX);
    serial->baud(UART_BAUDRATE);
    serial->format(UART_BITS, UART_PARITY, UART_STOP);
}

::Serial::~Serial() {
    delete serial;
}

uint8_t ::Serial::read() {
    return serial->getc();
}

int8_t ::Serial::readable() {
    return serial->readable();
}

int8_t ::Serial::writeable() {
    return serial->writeable();
}

void ::Serial::write(const uint8_t data) {
    serial->putc(data);
}

void ::Serial::print(const char* str) {
    serial->printf(str);
}

void ::Serial::println(const char* str) {
    serial->printf("%s\n", str);
}

void ::Serial::attach(void (*fptr)(void), IrqType type = RxIrq) {
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

#include "serial_wrapper.h"

::Serial::Serial(int32_t baud, int32_t bits, Parity parity, int32_t stop) {
    mbed::Serial::Parity mbedParity;
    switch (parity) {
        case None:
            mbedParity = mbed::Serial::None;
            break;
        case Odd:
            mbedParity = mbed::Serial::Odd;
            break;
        case Even:
            mbedParity = mbed::Serial::Even;
            break;
        case Forced1:
            mbedParity = mbed::Serial::Forced1;
            break;
        case Forced0:
            mbedParity = mbed::Serial::Forced0;
            break;
        default:
            mbedParity = mbed::Serial::None;
            break;
    }

    serial = new mbed::Serial(USBTX, USBRX);
    serial->baud(baud);
    serial->format(bits, mbedParity, stop);
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

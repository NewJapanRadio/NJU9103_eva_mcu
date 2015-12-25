#include "serial_wrapper.h"

::Serial::Serial() {
}

::Serial::~Serial() {
}

uint8_t ::Serial::read() {
    return '0';
}

int8_t ::Serial::readable() {
    return 0;
}

int8_t ::Serial::writeable() {
    return 0;
}

void ::Serial::write(const uint8_t data) {
}

void ::Serial::print(const char* str) {
}

void ::Serial::println(const char* str) {
}

void ::Serial::attach(void (*fptr)(void), IrqType type = RxIrq) {
}

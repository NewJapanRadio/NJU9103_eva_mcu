#include "stopwatch_wrapper.h"

::Stopwatch::Stopwatch() {
    timer = new mbed::Timer();
}

::Stopwatch::~Stopwatch() {
    delete timer;
}

void ::Stopwatch::start() {
    timer->reset();
    timer->start();
}

void ::Stopwatch::stop() {
    timer->stop();
}

uint32_t ::Stopwatch::read() {
    return timer->read();
}

uint32_t ::Stopwatch::read_ms() {
    return timer->read_ms();
}

uint32_t ::Stopwatch::read_us() {
    return timer->read_us();
}

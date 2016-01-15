#include "stopwatch_wrapper.h"

NewJapanRadio::Stopwatch::Stopwatch() {
    timer = new mbed::Timer();
}

NewJapanRadio::Stopwatch::~Stopwatch() {
    delete timer;
}

void NewJapanRadio::Stopwatch::start() {
    timer->reset();
    timer->start();
}

void NewJapanRadio::Stopwatch::stop() {
    timer->stop();
}

uint32_t NewJapanRadio::Stopwatch::read() {
    return timer->read();
}

uint32_t NewJapanRadio::Stopwatch::readMillisecond() {
    return timer->read_ms();
}

uint32_t NewJapanRadio::Stopwatch::readMicrosecond() {
    return timer->read_us();
}

#include "timer_wrapper.h"

NewJapanRadio::Timer::Timer() {
    timer = new mbed::Ticker();
}

NewJapanRadio::Timer::~Timer() {
    detach();
    delete timer;
}

void NewJapanRadio::Timer::attach(void (*fptr)(void), uint64_t t_us) {
    timer->attach_us(fptr, t_us);
}

void NewJapanRadio::Timer::detach() {
    timer->detach();
}

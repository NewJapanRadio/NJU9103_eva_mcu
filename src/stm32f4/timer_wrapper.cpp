#include "timer_wrapper.h"

::Timer::Timer() {
    timer = new mbed::Ticker();
}

::Timer::~Timer() {
    detach();
    delete timer;
}

void ::Timer::attach(void (*fptr)(void), uint64_t t_us) {
    timer->attach_us(fptr, t_us);
}

void ::Timer::detach() {
    timer->detach();
}

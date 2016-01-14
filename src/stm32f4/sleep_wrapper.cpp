#include "sleep_wrapper.h"

::Sleep::Sleep() {
}

::Sleep::~Sleep() {
}

void ::Sleep::sleep(uint32_t t_s) {
    wait(t_s);
}

void ::Sleep::sleepMillisecond(uint32_t t_ms) {
    wait_ms(t_ms);
}

void ::Sleep::sleepMicrosecond(uint32_t t_us) {
    wait_us(t_us);
}

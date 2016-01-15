#include "sleep_wrapper.h"

NewJapanRadio::Sleep::Sleep() {
}

NewJapanRadio::Sleep::~Sleep() {
}

void NewJapanRadio::Sleep::sleep(uint32_t t_s) {
    wait(t_s);
}

void NewJapanRadio::Sleep::sleepMillisecond(uint32_t t_ms) {
    wait_ms(t_ms);
}

void NewJapanRadio::Sleep::sleepMicrosecond(uint32_t t_us) {
    wait_us(t_us);
}

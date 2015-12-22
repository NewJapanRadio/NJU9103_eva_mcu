#include "rdyb_wrapper.h"

::Rdyb::Rdyb() {
    pinLv = 1;
}

::Rdyb::~Rdyb() {
}

int ::Rdyb::read() {
    int pinLvOld = pinLv;
    pinLv = pinLv == 1 ? 0 : 1;
    return pinLvOld;
}

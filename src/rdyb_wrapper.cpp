#include "rdyb_wrapper.h"

#define PINMASK(pin)  (1 << STM_PIN(pin))
#define PORT(pin)     ((GPIO_TypeDef *)(GPIOA_BASE + 0x0400 * STM_PORT(pin)))
#define READ_PIN(pin) ((PORT(pin)->IDR & PINMASK(pin)) ? 1 : 0)

::Rdyb::Rdyb() {
}

::Rdyb::~Rdyb() {
}

int ::Rdyb::read() {
    return READ_PIN(pinRdyb());
}

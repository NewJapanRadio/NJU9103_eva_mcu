#include <mbed.h>

extern void setup();
extern void loop();

void setIrqPriorities() {
    NVIC_SetPriority(UART5_IRQn, 1);
    NVIC_SetPriority(TIM5_IRQn, 2);
    NVIC_SetPriority(TIM4_IRQn, 3);
}

int main() {
    setIrqPriorities();
    setup();
    for (;;) {
        loop();
    }
}

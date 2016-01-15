#include <mbed.h>

extern void setup();
extern void loop();

void setIrqPriorities() {
}

int main() {
    setIrqPriorities();
    setup();
    for (;;) {
        loop();
    }
}

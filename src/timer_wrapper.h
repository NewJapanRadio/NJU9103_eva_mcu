#ifndef TIMER_WRAPPER_H
#define TIMER_WRAPPER_H

#include <mbed.h>

class Timer {
    public:
        Timer();
        ~Timer();

        void attach(void (*fptr)(void), uint64_t t_us);
        void detach();

    private:
        mbed::Ticker *timer;
};

#endif


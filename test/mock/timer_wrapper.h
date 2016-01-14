#ifndef __TIMER_WRAPPER_H__
#define __TIMER_WRAPPER_H__

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Timer {
    public:
        Timer();
        ~Timer();

        void attach(void (*fptr)(void), uint64_t t_us);
        void detach();
};

#endif

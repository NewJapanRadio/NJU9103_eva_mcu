#ifndef __TIMER_WRAPPER_H__
#define __TIMER_WRAPPER_H__

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Timer {
    public:
        Timer() { }

        MOCK_METHOD2(attach,
            void(void (*fptr)(void), uint64_t t_us));

        MOCK_METHOD0(detach,
            void());
};

#endif

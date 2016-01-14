#ifndef __SLEEP_WRAPPER_H__
#define __SLEEP_WRAPPER_H__

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Sleep {
    public:
        Sleep() { }

        MOCK_METHOD1(sleep,
            void(uint32_t t_s));

        MOCK_METHOD1(sleepMillisecond,
            void(uint32_t t_ms));

        MOCK_METHOD1(sleepMicrosecond,
            void(uint32_t t_us));
};

#endif

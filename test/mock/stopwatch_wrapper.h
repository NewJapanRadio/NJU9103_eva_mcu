#ifndef __STOPWATCH_WRAPPER_H__
#define __STOPWATCH_WRAPPER_H__

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Stopwatch {
    public:
        Stopwatch() { }

        MOCK_METHOD0(start,
            void());

        MOCK_METHOD0(stop,
            void());

        MOCK_METHOD0(read,
            uint32_t());

        MOCK_METHOD0(read_ms,
            uint32_t());

        MOCK_METHOD0(read_us,
            uint32_t());
};

#endif

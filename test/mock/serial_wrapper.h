#ifndef __SERIAL_WRAPPER_H__
#define __SERIAL_WRAPPER_H__

#include <stdint.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;

class Serial {
    public:
        enum IrqType {
            RxIrq = 0,
            TxIrq
        };

        Serial() { }

        MOCK_METHOD0(read,
            uint8_t());
        MOCK_METHOD0(readable,
            int8_t());
        MOCK_METHOD0(writeable,
            int8_t());
        MOCK_METHOD1(write,
            void(const uint8_t data));
        MOCK_METHOD1(print,
            void(const char* str));
        MOCK_METHOD1(println,
            void(const char* str));
        MOCK_METHOD2(attach,
            void(void (*fptr)(void), IrqType type));
};

#endif

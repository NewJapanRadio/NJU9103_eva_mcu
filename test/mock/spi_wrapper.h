#ifndef __SPI_WRAPPER_H__
#define __SPI_WRAPPER_H__

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;

namespace NewJapanRadio {
    class SPI {
        public:
            enum SpiMode {
                Mode0 = 0,
                Mode1,
                Mode2,
                Mode3
            };

            SPI() {
                EXPECT_CALL(*this, mode(_));
                EXPECT_CALL(*this, frequency(_));
            }

            MOCK_METHOD1(write,
                    uint8_t(uint8_t wd));

            MOCK_METHOD1(mode,
                    void(SpiMode mode));

            MOCK_METHOD1(frequency,
                    void(uint64_t freq));
    };
}

#endif

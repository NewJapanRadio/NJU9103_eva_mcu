#ifndef __SPI_WRAPPER_H__
#define __SPI_WRAPPER_H__

#include <mbed.h>

class SPI {
    protected:
        int bytes() { return 8; };

    public:
        enum SpiMode {
            Mode0 = 0,
            Mode1,
            Mode2,
            Mode3
        };

        SPI();
        ~SPI();

        uint8_t write(uint8_t wd);
        void mode(SpiMode mode);
        void frequency(uint64_t freq);

    private:
        mbed::SPI *spi;
};

#endif

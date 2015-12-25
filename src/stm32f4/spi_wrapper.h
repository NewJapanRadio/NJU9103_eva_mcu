#ifndef __SPI_WRAPPER_H__
#define __SPI_WRAPPER_H__

#include <mbed.h>

/** @class SPI
    @brief SPI peripheral wrapper
*/
class SPI {
    protected:
        /**
         * @brief byte count of 1 transaction
         */
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

        /**
         * @brief send data
         *
         * @param [in] data
         *      data for send
         *
         * @return read data
         */
        uint8_t write(uint8_t wd);

        /**
         * @brief set SPI mode
         *
         * @param [in] mode
         *      SPI mode
         */
        void mode(SpiMode mode);

        /**
         * @brief set SCK frequency
         *
         * @param [in] freq
         *      SCK frequency
         */
        void frequency(uint64_t freq);

    private:
        mbed::SPI *spi;
};

#endif

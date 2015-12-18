#include "spi_wrapper.h"

::SPI::SPI() {
    spi = new mbed::SPI(SPI_MOSI, SPI_MISO, SPI_SCK);
}

::SPI::~SPI() {
    delete spi;
}

uint8_t ::SPI::write(uint8_t wd) {
    return (uint8_t)spi->write(wd);
}
void ::SPI::mode(SpiMode mode) {
    int mbedMode = 0;
    switch (mode) {
        case Mode0:
            mbedMode = 0;
            break;
        case Mode1:
            mbedMode = 1;
            break;
        case Mode2:
            mbedMode = 2;
            break;
        case Mode3:
            mbedMode = 3;
            break;
        default:
            break;
    }
    spi->format(bytes(), mbedMode);
}

void ::SPI::frequency(uint64_t freq) {
    spi->frequency(freq);
}

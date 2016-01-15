#include "spi_wrapper.h"

NewJapanRadio::SPI::SPI() {
    spi = new mbed::SPI(SPI_MOSI, SPI_MISO, SPI_SCK);
}

NewJapanRadio::SPI::~SPI() {
    delete spi;
}

uint8_t NewJapanRadio::SPI::write(uint8_t wd) {
    return (uint8_t)spi->write(wd);
}
void NewJapanRadio::SPI::mode(SpiMode mode) {
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

void NewJapanRadio::SPI::frequency(uint64_t freq) {
    spi->frequency(freq);
}

#include "command_dispatcher.h"
#include "rdyb_wrapper.h"

::CommandDispatcher::CommandDispatcher() {
    rdyb = new ::Rdyb();

    spi = new ::SPI();
    spi->mode(::SPI::Mode1);
    spi->frequency(1E6);
}

::CommandDispatcher::~CommandDispatcher() {
    delete spi;
}

::CommandDispatcher::Status ::CommandDispatcher::RegisterWriteByte(uint8_t address, uint8_t regData) {
    Status status = Error;
    if ((address & 0xF0) == 0) {
        uint8_t rw = 0;
        uint8_t bc = 0;
        uint8_t spicmd = uint8_t(((rw << 1) + bc) << 2);

        spi->write((uint8_t)(address << 4) + spicmd);
        spi->write(regData);
        status = Success;
    }
    return status;
}

::CommandDispatcher::Status ::CommandDispatcher::RegisterReadByte(uint8_t address, uint8_t *regData) {
    Status status = Error;
    if ((address & 0xF0) == 0) {
        uint8_t rw = 1;
        uint8_t bc = 0;
        uint8_t spicmd = uint8_t(((rw << 1) + bc) << 2);

        spi->write((uint8_t)(address << 4) + spicmd);
        *regData = spi->write(0x00);
        status = Success;
    }
    return status;
}

::CommandDispatcher::Status ::CommandDispatcher::RegisterWriteShort(uint8_t address, uint16_t regData) {
    Status status = Error;
    if ((address & 0xF0) == 0) {
        uint8_t rw = 0;
        uint8_t bc = 1;
        uint8_t spicmd = uint8_t(((rw << 1) + bc) << 2);

        uint8_t lowByte = (uint8_t)(regData & 0x00FF);
        uint8_t highByte = (uint8_t)(regData >> 8);

        spi->write((uint8_t)(address << 4) + spicmd);
        spi->write(lowByte);
        spi->write(highByte);
        status = Success;
    }
    return status;
}

::CommandDispatcher::Status ::CommandDispatcher::RegisterReadShort(uint8_t address, uint16_t *regData) {
    Status status = Error;
    if ((address & 0xF0) == 0) {
        uint8_t rw = 1;
        uint8_t bc = 1;
        uint8_t spicmd = uint8_t(((rw << 1) + bc) << 2);

        spi->write((uint8_t)(address << 4) + spicmd);
        *regData = spi->write(0x00);
        *regData += (uint16_t)(spi->write(0x00) << 8);
        status = Success;
    }
    return status;
}

::CommandDispatcher::Status ::CommandDispatcher::StartSingleConversion(uint8_t control, uint16_t *regData) {
    RegisterWriteByte(ADDR_CTRL, control);
    while (rdyb->read() == 1) { }
    return RegisterReadShort((uint8_t)0x0A, regData);
}

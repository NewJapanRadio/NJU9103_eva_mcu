#include "spi_command.h"
#include "rdyb_wrapper.h"

::SPICommand::SPICommand() {
    rdyb = new ::Rdyb();

    spi = new ::SPI();
    spi->mode(::SPI::Mode1);
    spi->frequency(1E6);

    abortRequest = false;
}

::SPICommand::~SPICommand() {
    delete spi;
}

::SPICommand::Status ::SPICommand::SPIReset() {
    Status status = Error;
    spi->write(0x7F);
    spi->write(0xFF);
    spi->write(0xFF);
    status = Success;
    return status;
}

::SPICommand::Status ::SPICommand::RegisterWrite8Bit(uint8_t address, uint8_t data) {
    Status status = Error;
    if ((address & 0xF0) == 0) {
        uint8_t rw = 0;
        uint8_t bc = 0;
        uint8_t spicmd = uint8_t(((rw << 1) + bc) << 2);

        spi->write((uint8_t)(address << 4) + spicmd);
        spi->write(data);
        status = Success;
    }
    return status;
}

::SPICommand::Status ::SPICommand::RegisterRead8Bit(uint8_t address, uint8_t *data) {
    Status status = Error;
    if ((address & 0xF0) == 0) {
        uint8_t rw = 1;
        uint8_t bc = 0;
        uint8_t spicmd = uint8_t(((rw << 1) + bc) << 2);

        spi->write((uint8_t)(address << 4) + spicmd);
        *data = spi->write(0x00);
        status = Success;
    }
    return status;
}

::SPICommand::Status ::SPICommand::RegisterWrite16Bit(uint8_t address, uint8_t data0, uint8_t data1) {
    Status status = Error;
    if ((address & 0xF0) == 0) {
        uint8_t rw = 0;
        uint8_t bc = 1;
        uint8_t spicmd = uint8_t(((rw << 1) + bc) << 2);

        spi->write((uint8_t)(address << 4) + spicmd);
        spi->write(data0);
        spi->write(data1);
        status = Success;
    }
    return status;
}

::SPICommand::Status ::SPICommand::RegisterRead16Bit(uint8_t address, uint8_t *data0, uint8_t *data1) {
    Status status = Error;
    if ((address & 0xF0) == 0) {
        uint8_t rw = 1;
        uint8_t bc = 1;
        uint8_t spicmd = uint8_t(((rw << 1) + bc) << 2);

        spi->write((uint8_t)(address << 4) + spicmd);
        *data0 = spi->write(0x00);
        *data1 = spi->write(0x00);
        status = Success;
    }
    return status;
}

static ::SPICommand::Status ReadADCData(::SPICommand *dispatcher, uint16_t *data) {
    uint8_t high, low;
    ::SPICommand::Status status = dispatcher->RegisterRead16Bit(ADDR_ADCDATA0, &high, &low);
    *data = (uint16_t)((high << 8) + low);
    return status;
}

::SPICommand::Status ::SPICommand::StartSingleConversion(uint8_t control, uint16_t *data) {
    RegisterWrite8Bit(ADDR_CTRL, control);
    while (rdyb->read() == 1) {
        if (abortRequest) {
            abortRequest = false;
            return Abort;
        }
    }
    return ReadADCData(this, data);
}

::SPICommand::Status ::SPICommand::StartContinuousConversion(uint8_t control, uint16_t buf[], uint16_t length, uint16_t *resultLength) {
    RegisterWrite8Bit(ADDR_CTRL, control);
    for (int i = 0; i <= length; i++) {
        while (rdyb->read() == 1) {
            if (abortRequest) {
                abortRequest = false;
                return Abort;
            }
        }
        if (Success != ReadADCData(this, &buf[i]))
        {
            return Fail;
        }
        *resultLength = i + 1;
    }
    return Success;
}

void ::SPICommand::SetAbortRequest() {
    abortRequest = true;
}

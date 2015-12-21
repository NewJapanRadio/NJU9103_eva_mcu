#ifndef __COMMAND_DISPATCHER_H__
#define __COMMAND_DISPATCHER_H__

#include "spi_wrapper.h"
#include "rdyb_wrapper.h"

#define ADDR_CTRL      0x0
#define ADDR_ADCDATA0  0x1
#define ADDR_ADCDATA1  0x2
#define ADDR_PGACONF   0x3
#define ADDR_CLKCONF   0x4
#define ADDR_DACCONF   0x5
#define ADDR_OPTION0   0x6
#define ADDR_OPTION1   0x7
#define ADDR_GAIN0     0x8
#define ADDR_GAIN1     0x9
#define ADDR_GAIN2     0xA
#define ADDR_OFFSET0   0xB
#define ADDR_OFFSET1   0xC
#define ADDR_OFFSET2   0xD

class CommandDispatcher {
    public:
        enum Status {
            Error = -1,
            Success = 0,
            Abort = 1,
            Fail = 2
        };

        CommandDispatcher();
        ~CommandDispatcher();

        Status SPIReset();
        Status RegisterWrite8Bit(uint8_t address, uint8_t data);
        Status RegisterRead8Bit(uint8_t address, uint8_t *data);
        Status RegisterWrite16Bit(uint8_t address, uint8_t data0, uint8_t data1);
        Status RegisterRead16Bit(uint8_t address, uint8_t *data0, uint8_t *data1);
        Status StartSingleConversion(uint8_t control, uint16_t *adcData);
        Status StartContinuousConversion(uint8_t control, uint16_t buf[], uint16_t length, uint16_t *resultLength);

        void SetAbortRequest();

    private:
        ::SPI *spi;
        ::Rdyb *rdyb;

        bool abortRequest;
};

#endif

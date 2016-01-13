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

/** @class SPICommand
    @brief send SPI command actually via SPI peripheral
*/
class SPICommand {
    public:
        enum Status {
            Error = -1,
            Success = 0,
            Abort = 1,
            Fail = 2
        };

        SPICommand();
        ~SPICommand();

        /**
         * @brief Device Reset via SPI
         * @return result status
         * @retval Success
         * @retval Error
         *      this value will not be returned
         */
        Status SPIReset();

        /**
         * @brief 8bit Register Write via SPI
         * @param [in] address
         *      register address
         * @param [in] data
         *      data byte for write
         * @return result status
         * @retval Success
         * @retval Error
         *      address range error
         */
        Status RegisterWrite8Bit(uint8_t address, uint8_t data);

        /**
         * @brief 8bit Register Read via SPI
         * @param [in] address
         *      register address
         * @param [out] *data
         *      data for read
         * @return result status
         * @retval Success
         * @retval Error
         *      address range error
         */
        Status RegisterRead8Bit(uint8_t address, uint8_t *data);

        /**
         * @brief 16bit Register Write via SPI
         * @param [in] address
         *      register address
         * @param [in] data0
         *      data byte for write to [address]
         * @param [in] data1
         *      data byte for write to [address + 1]
         * @return result status
         * @retval Success
         * @retval Error
         *      address range error
         */
        Status RegisterWrite16Bit(uint8_t address, uint8_t data0, uint8_t data1);

        /**
         * @brief 16bit Register Read via SPI
         * @param [in] address
         *      register address
         * @param [out] *data0
         *      data for read from [address]
         * @param [out] *data1
         *      data for read from [address + 1]
         * @return result status
         * @retval Success
         * @retval Error
         *      address range error
         */
        Status RegisterRead16Bit(uint8_t address, uint8_t *data0, uint8_t *data1);

        /**
         * @brief execute ADC conversion once
         * @param [in] control
         *      value of CTRL register
         * @param [out] *adcData
         *      conversion data
         * @return result status
         * @retval Success
         * @retval Abort
         *      a conversion is aborted with SetAbortRequest()
         * @retval Error
         *      this value will not be returned
         */
        Status StartSingleConversion(uint8_t control, uint16_t *adcData);

        /**
         * @brief execute ADC conversion continuously
         * @param [in] control
         *      value of CTRL register
         * @param [out] buf[]
         *      buffer for conversion data
         * @param [in] length
         *      number of conversion
         * @param [out] resultLength
         *      conversion count actually
         * @return result status
         * @retval Success
         * @retval Abort
         *      a conversion is aborted with SetAbortRequest()
         * @retval Error
         *      this value will not be returned
         */
        Status StartContinuousConversion(uint8_t control, uint16_t buf[], uint16_t length, uint16_t *resultLength);

        /**
         * @brief execute ADC conversion intermittently
         * @param [in] control
         *      value of CTRL register
         * @param [out] buf[]
         *      buffer for conversion data
         * @param [in] length
         *      number of conversion
         * @param [out] resultLength
         *      conversion count actually
         * @return result status
         * @retval Success
         * @retval Abort
         *      a conversion is aborted with SetAbortRequest()
         * @retval Error
         *      this value will not be returned
         */
        Status StartIntermittentConversion(uint8_t control, uint16_t buf[], uint32_t interval, uint16_t length, uint16_t *resultLength);

        /**
         * @brief set abort request to StartSingleConversion() or StartContinuousConversion()
         */
        void SetAbortRequest();

    private:
        ::SPI *spi;
        ::Rdyb *rdyb;

        bool abortRequest;
};

#endif

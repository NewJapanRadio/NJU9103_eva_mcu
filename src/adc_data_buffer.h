#ifndef __ADC_DATA_BUFFER_H__
#define __ADC_DATA_BUFFER_H__

#include "types.h"

/** @class ADCDataBuffer
    @brief data buffer for continuously conversion
*/
class ADCDataBuffer {
    public:
        ADCDataBuffer();
        ~ADCDataBuffer();

        /**
         * @brief allocate memory
         *
         * In fact, fixed size memory pool is allocated.
         * This function validates required size,
         * and take an alignment to 8byte.
         *
         * @param [in] size
         *      allocation memory size
         * @return success and failure
         * @retval true
         * @retval false
         *      too large memory requested
         */
        bool Alloc(uint16_t size);

        /**
         * @brief return pointer to data buffer
         */
        uint16_t * GetBuffer();

        /**
         * @brief return allocated memory size
         */
        uint16_t GetAllocatedSize();

        /**
         * @brief return actually used length for conversion data
         * @sa SetDataLength
         */
        uint16_t GetDataLength();

        /**
         * @brief set used length for conversion data
         *
         * Meaning of this function existence is
         * to save number of converted ADC data
         * to return dumped data length at StartADCDataDump.
         *
         * @param [in] length
         *      set allocated memory size
         */
        void SetDataLength(uint16_t length);

    private:
        uint16_t dataLength;
        uint16_t allocatedSize; // aligned size
        uint16_t adcDataBuffer[MAX_BUFFER_SIZE];
};

#endif

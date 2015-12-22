#ifndef __ADC_DATA_BUFFER_H__
#define __ADC_DATA_BUFFER_H__

#include "nju9103_eva.h"

class ADCDataBuffer {
    public:
        ADCDataBuffer();
        ~ADCDataBuffer();

        bool Alloc(uint16_t size);
        void Dump(void (*pfSendPacket)(Packet *));
        uint16_t * GetBuffer();
        uint16_t GetDataLength();
        void SetDataLength(uint16_t length);

        void SetAbortRequest();

    private:
        uint16_t dataLength;
        uint16_t allocatedSize; // aligned size
        uint16_t adcDataBuffer[MAX_BUFFER_SIZE];

        bool abortRequest;
};

#endif

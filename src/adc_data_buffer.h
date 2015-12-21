#ifndef __ADC_DATA_BUFFER_H__
#define __ADC_DATA_BUFFER_H__

#include "nju9103_eva.h"

class ADCDataBuffer {
    public:
        ADCDataBuffer();
        ~ADCDataBuffer();

        bool Alloc(uint16_t size);
        void Dump(void (*pftr)(Packet *));
        uint16_t * GetBuffer();

        void SetAbortRequest();

    private:
        uint16_t allocatedSize;
        uint16_t adcDataBuffer[MAX_BUFFER_SIZE];

        bool abortRequest;
};

#endif

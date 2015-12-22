#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include "adc_data_buffer.h"

class Dispatcher {
    public:
        Dispatcher();
        ~Dispatcher();

        void Dispatch(Command *command, uint8_t *rx_buffer, Packet *packet, ::ADCDataBuffer *adcDataBuffer);
        void SetAbortRequest();

    private:
        ::SPICommand *spiCommand;
};

#endif

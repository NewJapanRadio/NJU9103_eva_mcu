#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include "adc_data_buffer.h"

/** @class Dispatcher
    @brief parse received command and dispatch a command
*/
class Dispatcher {
    public:
        enum Status {
            Error = -1,
            Success = 0,
            Abort = 1,
            Fail = 2
        };

        Dispatcher();
        ~Dispatcher();

        /**
         * @brief parse and dispatch a command
         * @return result status
         * @param [in] command
         * @param [in] args
         * @param [out] packet
         * @param [out] adcDataBuffer
         * @retval Success
         * @retval Abort
         *      a conversion is aborted with SetAbortRequest()
         * @retval Error
         *      parameter error
         *
         */
        Status Dispatch(Command *command, Packet *args, Packet *packet, ::ADCDataBuffer *adcDataBuffer);

        /**
         * @brief set abort request for dispatched command
         */
        void SetAbortRequest();

    private:
        ::SPICommand *spiCommand;
};

#endif

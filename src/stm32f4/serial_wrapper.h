#ifndef __SERIAL_WRAPPER_H__
#define __SERIAL_WRAPPER_H__

#include <mbed.h>

/** @class Serial
    @brief Serial peripheral wrapper
*/
class Serial {
    public:
        enum IrqType {
            RxIrq = 0,
            TxIrq
        };

        Serial();
        ~Serial();

        /**
         * @brief read received data
         */
        uint8_t read();

        /**
         * @brief return readable
         *
         * @return readable or not
         *
         * this function may not work in some environment
         */
        int8_t  readable();

        /**
         * @brief return writeable
         *
         * @return writeable or not
         *
         * this function may not work in some environment
         */
        int8_t  writeable();

        /**
         * @brief send data
         *
         * @param [in] data
         *      data for send
         */
        void    write(const uint8_t data);

        /**
         * @brief send characters for print w/o linefeed
         *
         * @param [in] str
         *      characters for send
         */
        void    print(const char* str);

        /**
         * @brief send characters for print w/ linefeed
         *
         * @param [in] str
         *      characters for send
         */
        void    println(const char* str);

        /**
         * @brief attach Rx received event handler
         *
         * @param [in] fptr
         *      envent handler
         * @param [in] type
         *      RxIrq or TxIrq
         */
        void    attach(void (*fptr)(void), IrqType type);

    private:
        mbed::Serial *serial;
};

#endif

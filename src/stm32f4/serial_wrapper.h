#ifndef __SERIAL_WRAPPER_H__
#define __SERIAL_WRAPPER_H__

#include <mbed.h>

class Serial {
    public:
        enum IrqType {
            RxIrq = 0,
            TxIrq
        };

        Serial();
        ~Serial();

        uint8_t read();
        int8_t  readable();
        int8_t  writeable();
        void    write(const uint8_t data);
        void    print(const char* str);
        void    println(const char* str);
        void    attach(void (*fptr)(void), IrqType type);

    private:
        mbed::Serial *serial;
};

#endif

#ifndef __SERIAL_WRAPPER_H__
#define __SERIAL_WRAPPER_H__

#include <stdint.h>

class Serial {
    public:
        enum Parity {
            None = 0,
            Odd,
            Even,
            Forced1,
            Forced0
        };

        enum IrqType {
            RxIrq = 0,
            TxIrq
        };

        Serial(int32_t baud, int32_t bits, Parity parity, int32_t stop);
        ~Serial();

        uint8_t read();
        int8_t  readable();
        int8_t  writeable();
        void    write(const uint8_t data);
        void    print(const char* str);
        void    println(const char* str);
        void    attach(void (*fptr)(void), IrqType type);
};

#endif

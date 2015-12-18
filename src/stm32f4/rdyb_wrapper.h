#ifndef __RDYB_WRAPPER_H__
#define __RDYB_WRAPPER_H__

#include <mbed.h>

class Rdyb {
    public:
        Rdyb();
        ~Rdyb();

        int read();

    protected:
        int pinRdyb() { return PA_6; }
};

#endif

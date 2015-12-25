#ifndef __RDYB_WRAPPER_H__
#define __RDYB_WRAPPER_H__

#include <mbed.h>

/** @class Rdyb
    @brief GPIO wrapper for rdyb
*/
class Rdyb {
    protected:
        /**
         * @brief RDYB pin position
         */
        int pinRdyb() { return PA_6; }

    public:
        Rdyb();
        ~Rdyb();

        /**
         * @brief return RDYB/MISO pin level
         */
        int read();
};

#endif

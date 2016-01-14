#ifndef __SLEEP_WRAPPER_H__
#define __SLEEP_WRAPPER_H__

#include <mbed.h>

/** @class Sleep
    @brief Sleep peripheral wrapper
*/
class Sleep {
    public:
        Sleep();
        ~Sleep();

        /**
         * @brief per second sleep
         *
         * @param [in] t_s
         *      seconds of sleep
         */
        void sleep(uint32_t t_s);

        /**
         * @brief per milli-second sleep
         *
         * @param [in] t_ms
         *      micro-seconds of sleep
         */
        void sleepMillisecond(uint32_t t_ms);

        /**
         * @brief per micro-second sleep
         *
         * @param [in] t_us
         *      micro-seconds of sleep
         */
        void sleepMicrosecond(uint32_t t_us);
};

#endif

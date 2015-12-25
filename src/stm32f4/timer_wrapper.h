#ifndef __TIMER_WRAPPER_H__
#define __TIMER_WRAPPER_H__

#include <mbed.h>

/** @class Timer
    @brief Timer peripheral wrapper
*/
class Timer {
    public:
        Timer();
        ~Timer();

        /**
         * @brief attach timer event handler
         *
         * @param [in] fptr
         *      envent handler
         * @param [in] t_us
         *      interval [us]
         */
        void attach(void (*fptr)(void), uint64_t t_us);

        /**
         * @brief detach timer event handler
         */
        void detach();

    private:
        mbed::Ticker *timer;
};

#endif

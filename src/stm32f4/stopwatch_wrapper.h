#ifndef __STOPWATCH_WRAPPER_H__
#define __STOPWATCH_WRAPPER_H__

#include <mbed.h>

namespace NewJapanRadio {
    /** @class Stopwatch
        @brief Stopwatch peripheral wrapper
    */
    class Stopwatch {
        public:
            Stopwatch();
            ~Stopwatch();

            /**
             * @brief start stopwatch
             */
            void start();

            /**
             * @brief stop stopwatch
             */
            void stop();

            /**
             * @brief read elapsed time as second
             */
            uint32_t read();

            /**
             * @brief read elapsed time as millisecond
             */
            uint32_t readMillisecond();

            /**
             * @brief read elapsed time as microsecond
             */
            uint32_t readMicrosecond();

        private:
            mbed::Timer *timer;
    };
}

#endif

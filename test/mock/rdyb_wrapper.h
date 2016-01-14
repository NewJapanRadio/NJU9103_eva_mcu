#ifndef __RDYB_WRAPPER_H__
#define __RDYB_WRAPPER_H__

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Rdyb {
    public:
        Rdyb() { }

        MOCK_METHOD0(read,
                int());
};

#endif

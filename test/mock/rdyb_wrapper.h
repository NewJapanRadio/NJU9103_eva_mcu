#ifndef __RDYB_WRAPPER_H__
#define __RDYB_WRAPPER_H__

class Rdyb {
    public:
        Rdyb();
        ~Rdyb();

        int read();

    private:
        int pinLv;
};

#endif

#pragma once

#include "pico/stdlib.h"

#define BLINKMAX 100

class Led {
    public:
        Led(uint pin);
        void SetError(bool error);
        void LedUpdate();
    private:
        uint led_pin;
        // true means error, false means no error
        bool errorstate = false;
        uint blinkcounter = 0;
};

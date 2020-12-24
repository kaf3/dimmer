#include <Arduino.h>

class Button {
    public:
        Button(int inPin, void (*inOnClick)(), void (*inOnLongClick)());
        int pin;
        static unsigned long interval;
        int read();
        void watch();
        void setup(void (*dtct)());
        void interrupt();

    private:
        void _process();
        bool _detected;
        unsigned long _startMillis;
        void (*_onClick)();
        void (*_onLongClick)();
};
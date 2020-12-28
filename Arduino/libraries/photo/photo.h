#include <Arduino.h>

class Photo {
    public:
        Photo(int inPin, void (*inIncreaseLevel)(), void (*inDecreaseLevel)());
        int pin;
        static unsigned long interval;
        int requiredValue;
        void watch();
        void setAutoMode();
        void resetAutoMode();
        bool isAutoMode();

    private:
        unsigned long _startMillis;
        bool _autoMode;
        bool _settingAutoMode;
        int _prevOffset;
        int _read();
        void _adjustLevel(int readedFromBuffer);
        bool _needToAdjust(int offset);
        int _values[20];
        int _counter;
        void (*_increaseLevel)();
        void (*_decreaseLevel)();
        void _adjustByOffset(int offset);
        void _writeToBuffer(int value);
        int _readFromBuffer();
        void _setRequiredValue(int readedFromBuffer);
        
};



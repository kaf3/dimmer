#include <Arduino.h>
#include <ESP8266WiFi.h>

class Dimmer {
    public:
        Dimmer(int inLoadPin, int inNullDetectedPin);
        int loadPin;
        int nullDetectedPin;
        void interrupt();
        void watch();
        void setLevel(int inLevel);
        void increase();
        void decrease();
        void on();
        void off();
        void toggle();
        bool isOn();
        void setup(void (*dtct)());
        int getLevel();
        unsigned long getMicrosLevel();
        void resume();
        void pause();
        bool paused;
    
    private:
        bool _nullDetected;
        bool _state;
        unsigned long _microsLevel;
        int _level;
        unsigned long _startMicros;
        void (*_detect)();
        static unsigned long _getMicrosByLevel(int level);
        bool _needPause(); 

        
};
#include <Arduino.h>

class Button {
  public:
    Button(int inPin, void (*inOnClick)(), void (*inOnLongClick)());
    int pin;
    unsigned long startMillis;
    static unsigned long interval;
    int read();
    void watch();
    void detect();

  private:
    void _process();
    boolean _detected;
    void (*_onClick)();
    void (*_onLongClick)();
};
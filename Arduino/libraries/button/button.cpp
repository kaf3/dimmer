#include <button.h>
#include <Arduino.h>

Button::Button(int inPin, void (*inOnClick)(), void (*inOnLongClick)()) {
        pin = inPin;
        _detected = false;
        startMillis = 0;
        _onClick = *inOnClick;
        _onLongClick = *inOnLongClick;
    }

int Button::read() {
    return digitalRead(pin);
}
void Button::detect() {
    _detected = true;
}
void Button::watch() {
    if (_detected) {
    _process();
    _detected = false;
    }
}

void Button::_process() {
    const int btnState = read();
    
    if (btnState == LOW) {
    startMillis = millis();
    return;
    }
    const unsigned long prevMillis = millis();
    const boolean longClick = (prevMillis - startMillis >= interval);

    if (longClick) {
    startMillis = prevMillis;
    (*_onLongClick)();
    return;
    }
    
    (*_onClick)();
    startMillis = prevMillis;
    return;
}

unsigned long Button::interval = 5000;
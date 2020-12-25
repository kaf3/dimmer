#include <button.h>
#include <Arduino.h>

Button::Button(int inPin, void (*inOnClick)(), void (*inOnLongClick)()) {
    pin = inPin;
    _detected = false;
    _startMillis = 0;
    _onClick = *inOnClick;
    _onLongClick = *inOnLongClick;
}

void Button::setup(void (*dtct)()) {
    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin), dtct, CHANGE);
}

void Button::interrupt() {
    _detected = true;
}

int Button::read() {
    return digitalRead(pin);
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
        _startMillis = millis();
        return;
    }

    const unsigned long currMillis = millis();
    const bool longClick = (currMillis - _startMillis >= interval);

    if (longClick) {
        _startMillis = currMillis;
        (*_onLongClick)();
        return;
    }
    
    (*_onClick)();
    _startMillis = currMillis;
    return;
}

unsigned long Button::interval = 2000;
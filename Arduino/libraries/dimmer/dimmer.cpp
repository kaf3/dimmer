#include <dimmer.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

Dimmer::Dimmer(int inLoadPin, int inNullDetectedPin) {
        loadPin = inLoadPin;
        nullDetectedPin = inNullDetectedPin;
        _nullDetected = false;
        _startMicros = 0;
        setLevel(0);
        on();
}

void Dimmer::setup(void (*dtct)()) {
    pinMode(loadPin, OUTPUT);
    pinMode(nullDetectedPin, INPUT_PULLUP);
    _detect = *dtct;
    attachInterrupt(digitalPinToInterrupt(nullDetectedPin), *_detect, FALLING);
}

void Dimmer::resume() {
    attachInterrupt(digitalPinToInterrupt(nullDetectedPin), *_detect, FALLING);
}

void Dimmer::pause() {
    detachInterrupt(digitalPinToInterrupt(nullDetectedPin));
}

void Dimmer::interrupt() {
    _nullDetected = true;
}

void Dimmer::watch() {
    int loadState = HIGH;

    if (!isOn()) {
        digitalWrite(loadPin, LOW);
        _nullDetected = false;
        return;
    }

    if (_nullDetected) {
        _startMicros = micros();
        _nullDetected = false;
    }

    const unsigned long currMicros = micros();
    
    if (currMicros - _startMicros >= _microsLevel) {
        loadState = LOW;
    }

    digitalWrite(loadPin, loadState);
}

void Dimmer::increase() {
    if (_level > 95) {
        return;
    }

    setLevel(_level + 5);
}

void Dimmer::decrease() {
    if (_level < 5) {
        return;
    }

    setLevel(_level - 5);
}

void Dimmer::setLevel(int inLevel) {
    if (!isOn()) {
        return;
    }

    _level = inLevel;
    _microsLevel = _getMicrosByLevel(_level);
}

unsigned long Dimmer::_getMicrosByLevel(int level) {
    short convertedLevel = level / 5;
    short micros[] = {
        0,
        1436,
        2048, 
        2532, 
        2952, 
        3333, 
        3690, 
        4030, 
        4359, 
        4681, 
        5000, 
        5319, 
        5641, 
        5970, 
        6310, 
        6667, 
        7048, 
        7468, 
        7952, 
        8564, 
        10000};
    short result = micros[convertedLevel];
    return (unsigned long)result;
}

void Dimmer::on() {
    _state = true;
}

void Dimmer::off() {
    _state = false;
}

bool Dimmer::isOn() {
    return _state;
}

void Dimmer::toggle() {
    _state = !_state;
}

int Dimmer::getLevel() {
    return _level;
}

unsigned long Dimmer::getMicrosLevel() {
    return _microsLevel;
}

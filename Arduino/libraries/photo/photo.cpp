#include <photo.h>
#include <Arduino.h>

Photo::Photo(int inPin, void (*inIncreaseLevel)(), void (*inDecreaseLevel)()) {
    pin = inPin;
    _startMillis = 0;
    requiredValue = 0;
    _increaseLevel = *inIncreaseLevel;
    _decreaseLevel = *inDecreaseLevel;
    _prevOffset = 1023;

    for(int i = 0; i < 20; i++) {
        _values[i] = -1;
    }

    _counter = 0;
}

void Photo::watch() {
    if (!_autoMode) {
        return;
    }

    unsigned long currMillis = millis(); 

    if (currMillis - _startMillis >= interval) {
        _startMillis = currMillis;

        _writeToBuffer(_read());
        int readedFromBuffer = _readFromBuffer();

        if (_settingAutoMode) {
            _setRequiredValue(readedFromBuffer);
        } else { 
            _adjustLevel(readedFromBuffer);
        }

        _counter++;
    }
}

void Photo::_setRequiredValue(int readedFromBuffer) {
    if (readedFromBuffer == -1) {
        return;
    }

    requiredValue = readedFromBuffer;
    _settingAutoMode = false;
    Serial.println("");
    Serial.print("setting end, req val = ");
    Serial.println(requiredValue);
}

void Photo::_adjustLevel(int readedFromBuffer) {
    if (readedFromBuffer == -1) {
        return;
    }

    int currOffset = readedFromBuffer - requiredValue;

    Serial.println();
    Serial.print("required value = ");
    Serial.println(requiredValue);

    Serial.print("read = ");
    Serial.println(currOffset + requiredValue);

    Serial.print("curr offset = ");
    Serial.println(currOffset);

    Serial.print("old offset = ");
    Serial.println(_prevOffset);

    if (_needToAdjust(currOffset)) {

        _adjustByOffset(currOffset);
    }

    _prevOffset = currOffset;

}

void Photo::setAutoMode() {
    _autoMode = true;
    _settingAutoMode = true;
    requiredValue = _read();
    _prevOffset = 1023;
}

void Photo::resetAutoMode() {
    _autoMode = false;
    _settingAutoMode = false;
    requiredValue = 0;
    _prevOffset = 1023;
}

void Photo::_writeToBuffer(int value) {
    _values[(_counter + 20) % 20] = value;
}

int Photo::_readFromBuffer() {
    if (_values[19] == -1) {
        return -1;
    }
    int sum = 0;

    for(int i = 0; i < 20; i++) {
        sum += _values[i];
        _values[i] = -1;
    }

    return sum / 20;
}

int Photo::_read() {
    return map(analogRead(pin), 0, 1023, 0, 49);
}


bool Photo::_needToAdjust(int offset) {
    return (_prevOffset * offset > 0) || ((abs(_prevOffset) + 1) < abs(offset));
}

void Photo::_adjustByOffset(int offset) {
    if (offset == 0) {
        return;
    }

    if (offset > 0) {
        (*_decreaseLevel)();
    } else {
        (*_increaseLevel)();
    }
}

unsigned long Photo::interval = 250;
#include <dimmer.h>
#include <string.h>
#include <button.h>

Dimmer dimmer(16, 4);

int testPin = 5;

int prevLoad = LOW;
int currLoad = LOW;
unsigned long prevLoadMicros = 0;
unsigned long currLoadMicros = 0;

unsigned long prevPerioddMicros = 0;
unsigned long currPeriodMicros = 0;
unsigned long rangeMicros = 0;
bool testDetect = false;

int prevNull = HIGH;
int currNull = HIGH;
float Q = 0;

void onToggle() {
    dimmer.toggle();
    Serial.println(String(dimmer.isOn()));
}
void onIncrease() {
    dimmer.increase();
    Serial.println(String(dimmer.getMicrosLevel()));
}
void onDecrease() {
    dimmer.decrease();
    Serial.println(String(dimmer.getMicrosLevel()));
}

void onLongClick() {}

Button buttonOn(14, onToggle, onLongClick);
Button buttonInc(12, onIncrease, onLongClick);
Button buttonDec(13, onDecrease, onLongClick);

ICACHE_RAM_ATTR void detectsButtonOn() { buttonOn.interrupt();}
ICACHE_RAM_ATTR void detectsButtonInc() { buttonInc.interrupt();}
ICACHE_RAM_ATTR void detectsButtonDec() { buttonDec.interrupt(); }
ICACHE_RAM_ATTR void detectsNull() { dimmer.interrupt(); }

ICACHE_RAM_ATTR void testInterrupt() {
    testDetect = true;
}



void setup() {
    Serial.begin(115200);

    pinMode(testPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(testPin), testInterrupt, CHANGE);

    buttonOn.setup(detectsButtonOn);
    buttonInc.setup(detectsButtonInc);
    buttonDec.setup(detectsButtonDec);
    dimmer.setup(detectsNull);
}

unsigned long counter = 0;



void loop() {
    buttonOn.watch();
    buttonInc.watch();
    buttonDec.watch();
    dimmer.watch();

    if (testDetect) {
        testDetect = false;

        currLoad = digitalRead(testPin);

        if (currLoad == HIGH) {
            prevLoadMicros = millis();
            return;
        }

        if (currLoad == LOW) {
            currLoadMicros = millis();
            counter++;
            if (counter % 100 == 0) {
                Serial.println(currLoadMicros - prevLoadMicros);
            }
            prevLoadMicros = currLoadMicros;
        }
    }
};

#include <string.h>
#include <button.h>
#include <dimmer.h>

const int led1 = 4;  // Assign LED1 to pin GPIO2
const int led2 = 16;

void digitalToggle(int pin) {
    if (digitalRead(pin) == HIGH) {
      digitalWrite(pin, LOW);  
    } else {
      digitalWrite(pin, HIGH);
    }
}

void onClick() {
  digitalToggle(led2);
}
void onLongClick() {
  digitalToggle(led1);
}

Button button1(13, onClick, onLongClick);
Dimmer dimmer(randomPin1, randomPin2);


ICACHE_RAM_ATTR void detectsButton1() {
  Serial.println("detects Button1");
  button1.detect();
}

ICACHE_RAM_ATTR void detectsNull() {
  Serial.println("detects null");
  dimmer.nullDetect();
}

void setup() {
    // Открываем порт на скорости 9600 бод
  Serial.begin(9600);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  
  pinMode(button1.pin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(button1.pin), detectsButton1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(dimmer.nullDetectedPin), detectsNull, FALLING);
}



void loop() {
    button1.watch();
    dimmer.nullWatch();
};

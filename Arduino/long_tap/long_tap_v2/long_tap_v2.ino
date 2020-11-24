#include <string.h>

const int led1 = 4;  // Assign LED1 to pin GPIO2
const int led2 = 16;

const int button1 = 13;
const int button2 = 12;

boolean button1_finished = true;
boolean button1_changed = false;

unsigned long button1_started_millis = 0;
unsigned long now_millis = 0;
unsigned long intervalClick = 5000;

ICACHE_RAM_ATTR void detectsButton1() {
  button1_started_millis = millis();
  button1_changed = true; 
}

void setup() {
    // Открываем порт на скорости 9600 бод
  Serial.begin(9600);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  
  pinMode(button1, INPUT_PULLUP);

  
  attachInterrupt(digitalPinToInterrupt(button1), detectsButton1, CHANGE);
}

void digitalToggle(int pin) {
    if (digitalRead(pin) == HIGH) {
      digitalWrite(pin, LOW);  
    } else {
      digitalWrite(pin, HIGH);
    }
}

void watchButton1Interrupt() {
  if (button1_changed) {
      button1_finished = (digitalRead(button1) == HIGH);
      
      button1_changed = false;
  }

    now_millis = millis();
  
    const boolean longClick = !button1_finished && (now_millis - button1_started_millis >= intervalClick);
  if (longClick) {
     button1_started_millis = now_millis;
     digitalToggle(led1); 
  }
}

void loop() {
    watchButton1Interrupt();
};

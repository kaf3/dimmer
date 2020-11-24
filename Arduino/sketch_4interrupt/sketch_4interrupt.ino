#include <string.h>

const int led1 = 16;  // Assign LED1 to pin GPIO2
const int led2 = 15;
const int led3 = 4;
const int led4 = 0;
const int button1 = 14;
const int button2 = 12;
const int button3 = 13;
const int button4 = 15;
int interruptQueue[16] = {0};

int ledState = LOW;
unsigned long now = 0;
unsigned long lastTime = 0;
unsigned long interval = 1000;

ICACHE_RAM_ATTR void detectsButton1() {
  push(interruptQueue, led1);
}

ICACHE_RAM_ATTR void detectsButton2() {
  push(interruptQueue, led2);
}

ICACHE_RAM_ATTR void detectsButton3() {
  push(interruptQueue, led3);
}

ICACHE_RAM_ATTR void detectsButton4() {
  push(interruptQueue, led4);
}

void push(int *arr, int el) {
  int i = 0;
  while(arr[i] != 0 && i < 16) {
      i++;
  }
  arr[i] = el; 
}

int shift(int *arr) {
  int first = arr[0];
  for(int i = 0; i < 15; i++) {
    arr[i] = arr[i+1]; 
  }
  arr[15] = 0;
  return first;
}

void setup() {
    // Открываем порт на скорости 9600 бод
  Serial.begin(9600);

  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(button1), detectsButton1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button2), detectsButton2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button3), detectsButton3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button4), detectsButton4, CHANGE);
  
}

void digitalToggle(int pin) {
    if (digitalRead(pin) == HIGH) {
      digitalWrite(pin, LOW);  
    } else {
      digitalWrite(pin, HIGH);
    }
}

void loop() {
  int firstInterrupt = shift(interruptQueue);
  
  if (firstInterrupt != 0) {
       digitalToggle(firstInterrupt);
  }

  
};

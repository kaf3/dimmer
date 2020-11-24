const int led = 2;      // Assign LED1 to pin GPIO2
const int button = 13;

int ledState = LOW;
unsigned long now = 0;
unsigned long lastTime = 0;
unsigned long interval = 1000;

void setup() {
    // Открываем порт на скорости 9600 бод
  Serial.begin(9600);
  
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);

}

void togglePin() {
  ledState = ledState == LOW ? HIGH : LOW;
  digitalWrite(led, ledState);
}

void watchTime() {
  if (now - lastTime >= interval) {
    lastTime = now;
    togglePin();
  }  
}

void serialEvent() {
       // Считываем данные
  interval = Serial.parseInt();

    // Возвращаем данные в порт
  Serial.println(interval);
  //startBlink();
};

void startBlink() {
  now = millis();
  
}
void stopBlink() {
  if (now - lastTime >= interval) {
    lastTime = now;
  } 
}

void loop() {
  //now = millis();
  //watchTime();

  if (digitalRead(button) == LOW) {
    digitalWrite(led, LOW);
  } else {
    digitalWrite(led, HIGH);
  }

    // Ждем данные
  if (Serial.available() == 1) {
    serialEvent();
  }
  

    // Считываем данные
  //String str = Serial.readStringUntil('\n');

    // Возвращаем данные в порт
  //Serial.println(str);
};

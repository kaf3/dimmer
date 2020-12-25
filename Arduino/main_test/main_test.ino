#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <photo.h>
#include <dimmer.h>
#include <string.h>
#include <button.h>
#include <softAp.h>

Dimmer dimmer(16, 4);
ESP8266WebServer server(80);

SoftAp softAp(server);

void handleRoot() {
    softAp.handleRoot();
}

void handleCredentials() {
    softAp.handleCredentials();
}

void handleNotFound() {
    softAp.handleNotFound();
}

void onToggle() {
    dimmer.toggle();
    Serial.println(String(dimmer.isOn()));
}
void onIncrease() {
    dimmer.increase();
    Serial.println();
    Serial.print("increase to ");
    Serial.println(String(dimmer.getMicrosLevel()));
}
void onDecrease() {
    dimmer.decrease();
    Serial.println();
    Serial.print("decrease to ");
    Serial.println(String(dimmer.getMicrosLevel()));
}

Photo photo(A0, onIncrease, onDecrease);

void onIncreaseClick() {
    onIncrease();
}

void onDecreaseClick() {
    onDecrease();
    photo.resetAutoMode();
}

void onIncreaseLongClick() {
    photo.setAutoMode();
    Serial.println(" increase long click");
}

void onDecreaseLongClick() {
    softAp.setSoftAp();
    server.begin();
    Serial.println(" decrease long click");
}

void onLongClick() {}

Button buttonOn(14, onToggle, onLongClick);
Button buttonInc(12, onIncreaseClick, onIncreaseLongClick);
Button buttonDec(13, onDecreaseClick, onDecreaseLongClick);

ICACHE_RAM_ATTR void detectsButtonOn() { buttonOn.interrupt();}
ICACHE_RAM_ATTR void detectsButtonInc() { buttonInc.interrupt();}
ICACHE_RAM_ATTR void detectsButtonDec() { buttonDec.interrupt(); }
ICACHE_RAM_ATTR void detectsNull() { dimmer.interrupt(); }

void setup() {
    Serial.begin(115200);

    buttonOn.setup(detectsButtonOn);
    buttonInc.setup(detectsButtonInc);
    buttonDec.setup(detectsButtonDec);
    dimmer.setup(detectsNull);
    softAp.setup(handleRoot, handleCredentials, handleNotFound);
}

void loop() {
    buttonOn.watch();
    buttonInc.watch();
    buttonDec.watch();
    dimmer.watch();
    photo.watch();
    softAp.watch();
};

#include <Arduino.h>
#include <credentials.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>

Credentials::Credentials() {
    ssid = "";
    pwd = "";
    email = "";
    upwd = "";
}

bool Credentials::store() {
    char _email[32];
    char _upwd[32];
    email.toCharArray(_email, 32);
    upwd.toCharArray(_upwd, 32);

    for (int i = 0; i < 32; i++) {
        EEPROM.write(i, _email[i]);
        EEPROM.write(i + 32, _upwd[i]);
    }

    if (EEPROM.commit()) {
        Serial.println("EEPROM successfully committed");
        return true;
    } else {
        Serial.println("ERROR! EEPROM commit failed");
        return false;
    }
}

bool Credentials::load() {
    char _email[32];
    char _upwd[32];

    for (int i = 0; i < 32; i++) {
        _email[i] = EEPROM.read(i);
        _upwd[i] = EEPROM.read(i + 32);
    }

    email = String(_email);
    upwd = String(_upwd);
    ssid = WiFi.SSID();
    pwd = WiFi.psk();

    return true;
}
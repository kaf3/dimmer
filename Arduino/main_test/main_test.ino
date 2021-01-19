#include <EEPROM.h>
#include <FbClient.h>
#include <credentials.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <photo.h>
#include <dimmer.h>
#include <string.h>
#include <button.h>
#include <softAp.h>

#define FIREBASE_HOST "https://home-for-u.firebaseio.com"
#define API_KEY "AIzaSyDgXyRA4x6CWoNjpMY2wfxV6OX95QKWGK0"

void handleRoot();
void handleCredentials();
void handleNotFound();
void onToggle();
void onIncrease();
void onDecrease();
void onIncreaseClick();
void onDecreaseClick();
void onIncreaseLongClick();
void onDecreaseLongClick();
void onValueReceived(MultiPathStreamData data);
void onLongClick();
void detectsButtonOn();
void detectsButtonInc();
void detectsButtonDec();
void detectsNull();
void onConnected(const WiFiEventStationModeGotIP& event);
void onDisconnected(const WiFiEventStationModeDisconnected& event);
void onModeChanged(const WiFiEventModeChange& event);
void populateDeviceJson(FirebaseJson &hardware);


WiFiEventHandler staConnected, staDisconnected, modeChanged;
 
Button buttonOn(14, onToggle, onLongClick);
Button buttonInc(12, onIncreaseClick, onIncreaseLongClick);  
Button buttonDec(13, onDecreaseClick, onDecreaseLongClick);
Credentials credentials;
Dimmer dimmer(16, 4);
ESP8266WebServer server(80);
FbClient client(credentials);
SoftAp softAp(server, credentials);
Photo photo(A0, onIncrease, onDecrease);


void setup() {
    Serial.begin(115200);

    EEPROM.begin(128);
    credentials.load();

    Serial.println("email :" + credentials.email);
    Serial.println("password :" + credentials.upwd);
    
    staConnected = WiFi.onStationModeGotIP(&onConnected);
    staDisconnected = WiFi.onStationModeDisconnected(&onDisconnected);
    modeChanged = WiFi.onWiFiModeChange(&onModeChanged);

    buttonOn.setup(detectsButtonOn);
    buttonInc.setup(detectsButtonInc);
    buttonDec.setup(detectsButtonDec);

    WiFi.setAutoReconnect(false);
    client.setup(onValueReceived, populateDeviceJson, FIREBASE_HOST, API_KEY);
    softAp.setup(handleRoot, handleCredentials, handleNotFound);
    
    Serial.println("wifi setup");
    WiFi.printDiag(Serial);

    WiFi.begin();

    dimmer.setup(detectsNull);

    Serial.println("boot by uart!!!!!!!!3");
}

void loop() {
    buttonOn.watch();
    buttonInc.watch();
    buttonDec.watch();
    dimmer.watch();
    photo.watch();
    softAp.watch();
    client.watch();
};

void onConnected(const WiFiEventStationModeGotIP& event) {
    Serial.println("on connected");
    client.begin();
    dimmer.resume();
}

void onDisconnected(const WiFiEventStationModeDisconnected& event) {
    Serial.println("on disconnected, reason ");
    Serial.println(event.reason);
    dimmer.pause();

    WiFi.printDiag(Serial);

    if(!client.reconnect()) {
       softAp.begin(); 
    }
}

void onModeChanged(const WiFiEventModeChange& event) {    
    if (softAp.isSoftAp()) {
        Serial.println("on ap mode changed!!!");
        dimmer.resume();
    }
}


void handleRoot() {
    softAp.handleRoot();
}

void handleCredentials() {
    softAp.handleCredentials();

    dimmer.pause();
    credentials.store();
    dimmer.resume();

    Serial.print("ssid from crdentials ");
    Serial.println(credentials.ssid);
}

void handleNotFound() {
    softAp.handleNotFound();
}

void onToggle() {
    dimmer.toggle();
    client.postBool("/switcher/value", dimmer.isOn());
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



void onIncreaseClick() {
    if (photo.isAutoMode()) {
        photo.resetAutoMode();
        client.postBool("/photoDetector/value", photo.isAutoMode());
    } else {
        onIncrease();
        client.postInt("/brightness/value", dimmer.getLevel());
    }
}

void onDecreaseClick() {
    if (photo.isAutoMode()) {
        photo.resetAutoMode(); 
        client.postBool("/photoDetector/value", photo.isAutoMode());
    } else {
        onDecrease();
        client.postInt("/brightness/value", dimmer.getLevel());
    }
}

void onIncreaseLongClick() {
    photo.setAutoMode();
    client.postBool("/photoDetector/value", photo.isAutoMode());
    Serial.println(" increase long click");
}

void onDecreaseLongClick() {
    Serial.println(" decrease long click");
    dimmer.pause();
    
    if (softAp.isSoftAp()) {
        softAp.end();
        WiFi.begin(credentials.ssid, credentials.pwd);
    } else {
        softAp.begin(); 
    }
    
    //dimmer.resume();
}

void populateDeviceJson(FirebaseJson &hardware) {
        FirebaseJson switcher;
        switcher.add("group", "device");
        switcher.add("name", "Выключатель");
        switcher.add("status", true);
        switcher.add("type", "switcher");
        switcher.add("value", dimmer.isOn());

        FirebaseJson photoDetector;
        photoDetector.add("group", "device");
        photoDetector.add("name", "Авто-режим");
        photoDetector.add("status", true);
        photoDetector.add("type", "photoDetector");
        photoDetector.add("value", photo.isAutoMode());

        FirebaseJson brightness;
        brightness.add("group", "device");
        brightness.add("name", "Регулятор яркости");
        brightness.add("status", true);
        brightness.add("type", "brightness");
        brightness.add("value", dimmer.getLevel());
        
        hardware.add("mac", WiFi.macAddress());
        hardware.add("name", "Ваш новый диммер");
        hardware.add("type", "dimmer");
        hardware.add("numberOfEquip", 3);
        hardware.add("equipmentCollection/switcher", switcher);
        hardware.add("equipmentCollection/photoDetector", photoDetector);
        hardware.add("equipmentCollection/brightness", brightness);
}

void onValueReceived(MultiPathStreamData stream) {
        Serial.println("new value received");
        FirebaseJson json;
        FirebaseJsonData data;

    if (stream.get("/switcher")) {
        json.setJsonData(stream.value);
        json.get(data, "value");
        if (data.type == "bool") {
            if (data.boolValue) {
                dimmer.on();
            } else {
                dimmer.off();
            }
        }
    }
    if (stream.get("/photoDetector")) {
        json.setJsonData(stream.value);
        json.get(data, "value");
        if (data.type == "bool") {
            if (data.boolValue) {
                photo.setAutoMode();
            } else {
                photo.resetAutoMode();
            }
        }
    } 

    if (stream.get("/brightness")) {
        json.setJsonData(stream.value);
        json.get(data, "value");

        if (data.type == "int" && data.intValue != dimmer.getLevel()) {
            dimmer.setLevel(data.intValue);
        }  
    }
}

void onLongClick() {}



ICACHE_RAM_ATTR void detectsButtonOn() { buttonOn.interrupt();}
ICACHE_RAM_ATTR void detectsButtonInc() { buttonInc.interrupt();}
ICACHE_RAM_ATTR void detectsButtonDec() { buttonDec.interrupt(); }
ICACHE_RAM_ATTR void detectsNull() { dimmer.interrupt(); }

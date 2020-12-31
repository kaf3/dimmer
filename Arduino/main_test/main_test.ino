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
void onValueReceived(String value);
void onLongClick();
void detectsButtonOn();
void detectsButtonInc();
void detectsButtonDec();
void detectsNull();
void onConnected(const WiFiEventStationModeGotIP& event);
void onDisconnected(const WiFiEventStationModeDisconnected& event);
void onModeChanged(const WiFiEventModeChange& event);
void onDisconnectedFromAP(const WiFiEventSoftAPModeStationDisconnected&);


WiFiEventHandler staConnected, staDisconnected, modeChanged, apDisconnected;
 
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

    staConnected = WiFi.onStationModeGotIP(&onConnected);
    staDisconnected = WiFi.onStationModeDisconnected(&onDisconnected);
    modeChanged = WiFi.onWiFiModeChange(&onModeChanged);
    apDisconnected = WiFi.onSoftAPModeStationDisconnected(&onDisconnectedFromAP);

    buttonOn.setup(detectsButtonOn);
    buttonInc.setup(detectsButtonInc);
    buttonDec.setup(detectsButtonDec);

    WiFi.setAutoReconnect(true);
    
    client.setup(onValueReceived, FIREBASE_HOST, API_KEY);
    softAp.setup(handleRoot, handleCredentials, handleNotFound);

    
    if(!client.begin()) {
        softAp.begin();
    }

    dimmer.setup(detectsNull);
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
    dimmer.resume();
}

void onDisconnected(const WiFiEventStationModeDisconnected& event) {
    Serial.println("on disconnected");
    dimmer.pause();
}

void onModeChanged(const WiFiEventModeChange& event) {
    if (event.newMode == WIFI_AP) {
        Serial.println("on ap mode changed!!!");
    }
    /*if (event.newMode == WIFI_OFF) {
        dimmer.resume();
        Serial.println("wifi off dimmer resume");
    } else {
        dimmer.pause();
    }
    */
}

void onDisconnectedFromAP(const WiFiEventSoftAPModeStationDisconnected&) {
    //dimmer.pause();
}


void handleRoot() {
    softAp.handleRoot();
}

void handleCredentials() {
    softAp.handleCredentials();

    Serial.print("ssid from crdentials ");
    Serial.println(credentials.ssid);
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



void onIncreaseClick() {
    if (softAp.isSoftAp()) {
        dimmer.pause();
        softAp.end();
        client.begin();
        dimmer.resume();
    } else {
        onIncrease();
    }
}

void onDecreaseClick() {
    if (photo.isAutoMode()) {
        photo.resetAutoMode(); 
    } else {
        onDecrease();
    }
}

void onIncreaseLongClick() {
    photo.setAutoMode();
    Serial.println(" increase long click");
}

void onDecreaseLongClick() {
    Serial.println(" decrease long click");
    dimmer.pause();
    softAp.begin();
    dimmer.resume();
}

void onValueReceived(String value) {
    Serial.print("on value received callback, value = ");
    Serial.println(value);
}

void onLongClick() {}



ICACHE_RAM_ATTR void detectsButtonOn() { buttonOn.interrupt();}
ICACHE_RAM_ATTR void detectsButtonInc() { buttonInc.interrupt();}
ICACHE_RAM_ATTR void detectsButtonDec() { buttonDec.interrupt(); }
ICACHE_RAM_ATTR void detectsNull() { dimmer.interrupt(); }

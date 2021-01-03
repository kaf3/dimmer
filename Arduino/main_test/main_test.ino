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
void onValueReceived(StreamData data);
void onLongClick();
void detectsButtonOn();
void detectsButtonInc();
void detectsButtonDec();
void detectsNull();
void onConnected(const WiFiEventStationModeGotIP& event);
void onDisconnected(const WiFiEventStationModeDisconnected& event);
void onModeChanged(const WiFiEventModeChange& event);


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

    //credentials.ssid = "iPhone (Никита)";
    //credentials.pwd = "qwerty1234";
    //credentials.email = "nekit.97@bk.ru";
    //credentials.upwd = "123456";

    staConnected = WiFi.onStationModeGotIP(&onConnected);
    staDisconnected = WiFi.onStationModeDisconnected(&onDisconnected);
    modeChanged = WiFi.onWiFiModeChange(&onModeChanged);

    buttonOn.setup(detectsButtonOn);
    buttonInc.setup(detectsButtonInc);
    buttonDec.setup(detectsButtonDec);

    WiFi.setAutoReconnect(false);

    dimmer.setup(detectsNull);
    dimmer.pause();
    client.setup(onValueReceived, FIREBASE_HOST, API_KEY);
    softAp.setup(handleRoot, handleCredentials, handleNotFound);

    WiFi.begin();
    delay(5000);
    if (WiFi.status() != WL_CONNECTED) {
        softAp.begin();
    } 
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
    if (photo.isAutoMode()) {
        photo.resetAutoMode();
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
    
    if (softAp.isSoftAp()) {
        softAp.end();
        WiFi.begin(credentials.ssid, credentials.pwd);
    } else {
        softAp.begin(); 
    }
    
    //dimmer.resume();
}

void onValueReceived(StreamData data) {
  Serial.println("Stream Data1 available...");
  Serial.println("STREAM PATH: " + data.streamPath());
  Serial.println("EVENT PATH: " + data.dataPath());
  Serial.println("DATA TYPE: " + data.dataType());
  Serial.println("EVENT TYPE: " + data.eventType());
  Serial.println("VALUE: " + data.intData());
  Serial.println();
}

void onLongClick() {}



ICACHE_RAM_ATTR void detectsButtonOn() { buttonOn.interrupt();}
ICACHE_RAM_ATTR void detectsButtonInc() { buttonInc.interrupt();}
ICACHE_RAM_ATTR void detectsButtonDec() { buttonDec.interrupt(); }
ICACHE_RAM_ATTR void detectsNull() { dimmer.interrupt(); }

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

/*
 * Exception (9):
epc1=0x4010571c epc2=0x00000000 epc3=0x00000000 excvaddr=0x00000003 depc=0x00000000

>>>stack>>>

ctx: sys
sp: 3fffecd0 end: 3fffffb0 offset: 0190
3fffee60:  3ffe89dc 00000000 73730020 3ffef280  
3fffee70:  4010594d 00128a80 3fff0a88 00000000  
3fffee80:  3ffef100 3fff0a88 3ffe924c 4021b05d  
3fffee90:  00000000 000dc5cc 4021cf98 000003e8  
3fffeea0:  3fff0a88 3fff068c 3ffef280 4021b264  
3fffeeb0:  3fff068c 3fffeef0 3fff0684 402046d4  
3fffeec0:  00000000 3fffeef0 3fff0994 4020327a  
3fffeed0:  3fff068c 3fff4e2c 3fff17ec 402287a0  
3fffeee0:  00000000 3fff4e2c 3fff17ec 40204d5c  
3fffeef0:  3fff182c 0015001f 00ff2144 00000000  
3fffef00:  002c0000 000000c9 00000000 00000000  
3fffef10:  801056c9 000de13b 3ffef100 40206891  
3fffef20:  40269b88 3fff17ec 3fff4e0c 4020525b  
3fffef30:  40269bae 3fffdab0 00000000 3ffef280  
3fffef40:  3fffdc10 00000000 3fff17ec 4026db58  
3fffef50:  40000f49 3fffdab0 3fffdab0 40000f49  
3fffef60:  40000e19 00000005 00086b90 00000000  
3fffef70:  3fffefc0 aa55aa55 000000e1 40105069  
3fffef80:  4010506f 00086b90 00000000 b9f4e10b  
3fffef90:  4010000d 7f0e7b61 f110afe0 568b1486  
3fffefa0:  40261774 3fffef3c 4026172d 3fffff48  
3fffefb0:  3fffffc0 00000000 00000000 feefeffe  
3fffefc0:  feefeffe feefeffe feefeffe feefeffe  
3fffefd0:  feefeffe feefeffe feefeffe feefeffe  
3fffefe0:  feefeffe feefeffe feefeffe feefeffe  
3fffeff0:  feefeffe feefeffe feefeffe feefeffe  
3ffff000:  feefeffe feefeffe feefeffe feefeffe  
3ffff010:  feefeffe feefeffe feefeffe feefeffe  
3ffff020:  feefeffe feefeffe feefeffe feefeffe  
3ffff030:  feefeffe feefeffe feefeffe feefeffe  
3ffff040:  feefeffe feefeffe feefeffe feefeffe  
3ffff050:  feefeffe feefeffe feefeffe feefeffe  
3ffff060:  feefeffe feefeffe feefeffe feefeffe  
3ffff070:  feefeffe feefeffe feefeffe feefeffe  
3ffff080:  feefeffe feefeffe feefeffe feefeffe  
3ffff090:  feefeffe feefeffe feefeffe feefeffe  
3ffff0a0:  feefeffe feefeffe feefeffe feefeffe  
3ffff0b0:  feefeffe feefeffe feefeffe feefeffe  
3ffff0c0:  feefeffe feefeffe feefeffe feefeffe  
3ffff0d0:  feefeffe feefeffe feefeffe feefeffe  
3ffff0e0:  feefeffe feefeffe feefeffe feefeffe  
3ffff0f0:  feefeffe feefeffe feefeffe feefeffe  
3ffff100:  feefeffe feefeffe feefeffe feefeffe  
3ffff110:  feefeffe feefeffe feefeffe feefeffe  
3ffff120:  feefeffe feefeffe feefeffe feefeffe  
3ffff130:  feefeffe feefeffe feefeffe feefeffe  
3ffff140:  feefeffe feefeffe feefeffe feefeffe  
3ffff150:  feefeffe feefeffe feefeffe feefeffe  
3ffff160:  feefeffe feefeffe feefeffe feefeffe  
3ffff170:  feefeffe feefeffe feefeffe feefeffe  
3ffff180:  feefeffe feefeffe feefeffe feefeffe  
3ffff190:  feefeffe feefeffe feefeffe feefeffe  
3ffff1a0:  feefeffe feefeffe feefeffe feefeffe  
3ffff1b0:  feefeffe feefeffe feefeffe feefeffe  
3ffff1c0:  feefeffe feefeffe feefeffe feefeffe  
3ffff1d0:  feefeffe feefeffe feefeffe feefeffe  
3ffff1e0:  feefeffe feefeffe feefeffe feefeffe  
3ffff1f0:  feefeffe feefeffe feefeffe feefeffe  
3ffff200:  feefeffe feefeffe feefeffe feefeffe  
3ffff210:  feefeffe feefeffe feefeffe feefeffe  
3ffff220:  feefeffe feefeffe feefeffe feefeffe  
3ffff230:  feefeffe feefeffe feefeffe feefeffe  
3ffff240:  feefeffe feefeffe feefeffe feefeffe  
3ffff250:  feefeffe feefeffe feefeffe feefeffe  
3ffff260:  feefeffe feefeffe feefeffe feefeffe  
3ffff270:  feefeffe feefeffe feefeffe feefeffe  
3ffff280:  feefeffe feefeffe feefeffe feefeffe  
3ffff290:  feefeffe feefeffe feefeffe feefeffe  
3ffff2a0:  feefeffe feefeffe feefeffe feefeffe  
3ffff2b0:  feefeffe feefeffe feefeffe feefeffe  
3ffff2c0:  feefeffe feefeffe feefeffe feefeffe  
3ffff2d0:  feefeffe feefeffe feefeffe feefeffe  
3ffff2e0:  feefeffe feefeffe feefeffe feefeffe  
3ffff2f0:  feefeffe feefeffe feefeffe feefeffe  
3ffff300:  feefeffe feefeffe feefeffe feefeffe  
3ffff310:  feefeffe feefeffe feefeffe feefeffe  
3ffff320:  feefeffe feefeffe feefeffe feefeffe  
3ffff330:  feefeffe feefeffe feefeffe feefeffe  
3ffff340:  feefeffe feefeffe feefeffe feefeffe  
3ffff350:  feefeffe feefeffe feefeffe feefeffe  
3ffff360:  feefeffe feefeffe feefeffe feefeffe  
3ffff370:  feefeffe feefeffe feefeffe feefeffe  
3ffff380:  feefeffe feefeffe feefeffe feefeffe  
3ffff390:  feefeffe feefeffe feefeffe feefeffe  
3ffff3a0:  feefeffe feefeffe feefeffe feefeffe  
3ffff3b0:  feefeffe feefeffe feefeffe feefeffe  
3ffff3c0:  feefeffe feefeffe feefeffe feefeffe  
3ffff3d0:  feefeffe feefeffe feefeffe feefeffe  
3ffff3e0:  feefeffe feefeffe feefeffe feefeffe  
3ffff3f0:  feefeffe feefeffe feefeffe feefeffe  
3ffff400:  feefeffe feefeffe feefeffe feefeffe  
3ffff410:  feefeffe feefeffe feefeffe feefeffe  
3ffff420:  feefeffe feefeffe feefeffe feefeffe  
3ffff430:  feefeffe feefeffe feefeffe feefeffe  
3ffff440:  feefeffe feefeffe feefeffe feefeffe  
3ffff450:  feefeffe feefeffe feefeffe feefeffe  
3ffff460:  feefeffe feefeffe feefeffe feefeffe  
3ffff470:  feefeffe feefeffe feefeffe feefeffe  
3ffff480:  feefeffe feefeffe feefeffe feefeffe  
3ffff490:  feefeffe feefeffe feefeffe feefeffe  
3ffff4a0:  feefeffe feefeffe feefeffe feefeffe  
3ffff4b0:  feefeffe feefeffe feefeffe feefeffe  
3ffff4c0:  feefeffe feefeffe feefeffe feefeffe  
3ffff4d0:  feefeffe feefeffe feefeffe feefeffe  
3ffff4e0:  feefeffe feefeffe feefeffe feefeffe  
3ffff4f0:  feefeffe feefeffe feefeffe feefeffe  
3ffff500:  feefeffe feefeffe feefeffe feefeffe  
3ffff510:  feefeffe feefeffe feefeffe feefeffe  
3ffff520:  feefeffe feefeffe feefeffe feefeffe  
3ffff530:  feefeffe feefeffe feefeffe feefeffe  
3ffff540:  feefeffe feefeffe feefeffe feefeffe  
3ffff550:  feefeffe feefeffe feefeffe feefeffe  
3ffff560:  feefeffe feefeffe feefeffe feefeffe  
3ffff570:  feefeffe feefeffe feefeffe feefeffe  
3ffff580:  feefeffe feefeffe feefeffe feefeffe  
3ffff590:  feefeffe feefeffe feefeffe feefeffe  
3ffff5a0:  feefeffe feefeffe feefeffe feefeffe  
3ffff5b0:  feefeffe feefeffe feefeffe feefeffe  
3ffff5c0:  feefeffe feefeffe feefeffe feefeffe  
3ffff5d0:  feefeffe feefeffe feefeffe feefeffe  
3ffff5e0:  feefeffe feefeffe feefeffe feefeffe  
3ffff5f0:  feefeffe feefeffe feefeffe feefeffe  
3ffff600:  feefeffe feefeffe feefeffe feefeffe  
3ffff610:  feefeffe feefeffe feefeffe feefeffe  
3ffff620:  feefeffe feefeffe feefeffe feefeffe  
3ffff630:  feefeffe feefeffe feefeffe feefeffe  
3ffff640:  feefeffe feefeffe feefeffe feefeffe  
3ffff650:  feefeffe feefeffe feefeffe feefeffe  
3ffff660:  feefeffe feefeffe feefeffe feefeffe  
3ffff670:  feefeffe feefeffe feefeffe feefeffe  
3ffff680:  feefeffe feefeffe feefeffe feefeffe  
3ffff690:  feefeffe feefeffe feefeffe feefeffe  
3ffff6a0:  feefeffe feefeffe feefeffe feefeffe  
3ffff6b0:  feefeffe feefeffe feefeffe feefeffe  
3ffff6c0:  feefeffe feefeffe feefeffe feefeffe  
3ffff6d0:  feefeffe feefeffe feefeffe feefeffe  
3ffff6e0:  feefeffe feefeffe feefeffe feefeffe  
3ffff6f0:  feefeffe feefeffe feefeffe feefeffe  
3ffff700:  feefeffe feefeffe feefeffe feefeffe  
3ffff710:  feefeffe feefeffe feefeffe feefeffe  
3ffff720:  feefeffe feefeffe feefeffe feefeffe  
3ffff730:  feefeffe feefeffe feefeffe feefeffe  
3ffff740:  feefeffe feefeffe feefeffe feefeffe  
3ffff750:  feefeffe feefeffe feefeffe feefeffe  
3ffff760:  feefeffe feefeffe feefeffe feefeffe  
3ffff770:  feefeffe feefeffe feefeffe feefeffe  
3ffff780:  feefeffe feefeffe feefeffe feefeffe  
3ffff790:  feefeffe feefeffe feefeffe feefeffe  
3ffff7a0:  feefeffe feefeffe feefeffe feefeffe  
3ffff7b0:  feefeffe feefeffe feefeffe feefeffe  
3ffff7c0:  feefeffe feefeffe feefeffe feefeffe  
3ffff7d0:  feefeffe feefeffe feefeffe feefeffe  
3ffff7e0:  feefeffe feefeffe feefeffe feefeffe  
3ffff7f0:  feefeffe feefeffe feefeffe feefeffe  
3ffff800:  feefeffe feefeffe feefeffe feefeffe  
3ffff810:  feefeffe feefeffe feefeffe feefeffe  
3ffff820:  feefeffe feefeffe feefeffe feefeffe  
3ffff830:  feefeffe feefeffe feefeffe feefeffe  
3ffff840:  feefeffe feefeffe feefeffe feefeffe  
3ffff850:  feefeffe feefeffe feefeffe feefeffe  
3ffff860:  feefeffe feefeffe feefeffe feefeffe  
3ffff870:  feefeffe feefeffe feefeffe feefeffe  
3ffff880:  feefeffe feefeffe feefeffe feefeffe  
3ffff890:  feefeffe feefeffe feefeffe feefeffe  
3ffff8a0:  feefeffe feefeffe feefeffe feefeffe  
3ffff8b0:  feefeffe feefeffe feefeffe feefeffe  
3ffff8c0:  feefeffe feefeffe feefeffe feefeffe  
3ffff8d0:  feefeffe feefeffe feefeffe feefeffe  
3ffff8e0:  feefeffe feefeffe feefeffe feefeffe  
3ffff8f0:  feefeffe feefeffe feefeffe feefeffe  
3ffff900:  feefeffe feefeffe feefeffe feefeffe  
3ffff910:  feefeffe feefeffe feefeffe feefeffe  
3ffff920:  feefeffe feefeffe feefeffe feefeffe  
3ffff930:  feefeffe feefeffe feefeffe feefeffe  
3ffff940:  feefeffe feefeffe feefeffe feefeffe  
3ffff950:  feefeffe feefeffe feefeffe feefeffe  
3ffff960:  feefeffe feefeffe feefeffe feefeffe  
3ffff970:  feefeffe feefeffe feefeffe feefeffe  
3ffff980:  feefeffe feefeffe feefeffe feefeffe  
3ffff990:  feefeffe feefeffe feefeffe feefeffe  
3ffff9a0:  feefeffe feefeffe feefeffe feefeffe  
3ffff9b0:  feefeffe feefeffe feefeffe feefeffe  
3ffff9c0:  feefeffe feefeffe feefeffe feefeffe  
3ffff9d0:  feefeffe feefeffe feefeffe feefeffe  
3ffff9e0:  feefeffe feefeffe feefeffe feefeffe  
3ffff9f0:  feefeffe feefeffe feefeffe feefeffe  
3ffffa00:  feefeffe feefeffe feefeffe feefeffe  
3ffffa10:  feefeffe feefeffe feefeffe feefeffe  
3ffffa20:  feefeffe feefeffe feefeffe feefeffe  
3ffffa30:  feefeffe feefeffe feefeffe feefeffe  
3ffffa40:  feefeffe feefeffe feefeffe feefeffe  
3ffffa50:  feefeffe feefeffe feefeffe feefeffe  
3ffffa60:  feefeffe feefeffe feefeffe feefeffe  
3ffffa70:  feefeffe feefeffe feefeffe feefeffe  
3ffffa80:  feefeffe feefeffe feefeffe feefeffe  
3ffffa90:  feefeffe feefeffe feefeffe feefeffe  
3ffffaa0:  feefeffe feefeffe feefeffe feefeffe  
3ffffab0:  feefeffe feefeffe feefeffe feefeffe  
3ffffac0:  feefeffe feefeffe feefeffe feefeffe  
3ffffad0:  feefeffe feefeffe feefeffe feefeffe  
3ffffae0:  feefeffe feefeffe feefeffe feefeffe  
3ffffaf0:  feefeffe feefeffe feefeffe feefeffe  
3ffffb00:  feefeffe feefeffe feefeffe feefeffe  
3ffffb10:  feefeffe feefeffe feefeffe feefeffe  
3ffffb20:  feefeffe feefeffe feefeffe feefeffe  
3ffffb30:  feefeffe feefeffe feefeffe feefeffe  
3ffffb40:  feefeffe feefeffe feefeffe feefeffe  
3ffffb50:  feefeffe feefeffe feefeffe feefeffe  
3ffffb60:  feefeffe feefeffe feefeffe feefeffe  
3ffffb70:  feefeffe feefeffe feefeffe feefeffe  
3ffffb80:  feefeffe feefeffe feefeffe feefeffe  
3ffffb90:  feefeffe feefeffe feefeffe feefeffe  
3ffffba0:  feefeffe feefeffe feefeffe feefeffe  
3ffffbb0:  feefeffe feefeffe feefeffe feefeffe  
3ffffbc0:  feefeffe feefeffe feefeffe feefeffe  
3ffffbd0:  00000000 00000000 00000020 40100320  
3ffffbe0:  feefeffe feefeffe 00000000 40101cb8  
3ffffbf0:  3ffe9df0 40104f4b 3fff17ec feefeffe  
3ffffc00:  00000001 40104126 3ffedf70 feefeffe  
3ffffc10:  40104597 feefeffe feefeffe feefeffe  
3ffffc20:  feefeffe feefeffe feefeffe feefeffe  
3ffffc30:  4010405f 0000007f 00000000 00040000  
3ffffc40:  00000000 feefeffe feefeffe feefeffe  
3ffffc50:  40102f28 00080000 feefeffe feefeffe  
3ffffc60:  00000005 00000000 00000020 40100320  
3ffffc70:  feefeffe 2c9f0300 00000005 40101cb8  
3ffffc80:  3ffe9df5 40104f63 3ffed880 00000000  
3ffffc90:  4010284f 3ffed880 0000000a 00000000  
3ffffca0:  fffffff2 00011d0a 3ffee250 40102a2c  
3ffffcb0:  3ffea670 00000000 00000000 0000001c  
3ffffcc0:  fffffff2 00011d0a 40102eee 00000100  
3ffffcd0:  3ffea670 7fffffff 00000000 00000001  
3ffffce0:  00000001 00000080 feefeffe feefeffe  
3ffffcf0:  3ffea670 feefeffe 4022cdd5 00011d0a  
3ffffd00:  3ffea67c 2c9f0300 4000050c 3fffc278  
3ffffd10:  40102c10 3fffc200 00000022 00000000  
3ffffd20:  4010106f 00000030 00000013 ffffffff  
3ffffd30:  4010106c 3fff5004 3fff11c0 40100cfe  
3ffffd40:  3fff0bc8 00000004 000007da 40100e09  
3ffffd50:  00003ed0 3fff0bc8 00000000 00000020  
3ffffd60:  00000010 00000000 0000000a 40001db4  
3ffffd70:  40001f46 00000008 00000020 4010106c  
3ffffd80:  00000000 3ffe8ada 3ffffe30 40100d4d  
3ffffd90:  00000000 00000004 000007c8 40100e09  
3ffffda0:  00003e40 000007c8 000007c8 40100cfe  
3ffffdb0:  feefeffe 0000000c 000007eb 40100e09  
3ffffdc0:  00003f58 3fff0bc8 00000020 4010101f  
3ffffdd0:  40105ae5 0000000c 3fff500c 00000000  
3ffffde0:  00000000 3fff50bc 40263a03 00000001  
3ffffdf0:  ffffffff 00000000 3ffe9f41 00000008  
3ffffe00:  40263a52 3fff50bc 3fff50bc 00000000  
3ffffe10:  fffffffe 00000000 40101df3 3fff50bc  
3ffffe20:  000000c0 3fff0bc8 00000000 3fff5124  
3ffffe30:  3ffe9df0 40262b07 3fff50bc 3ffee1cc  
3ffffe40:  00000000 40256abf 3ffee1cc 3fff50bc  
3ffffe50:  3ffe9914 0000000c 3fff5080 4021bac4  
3ffffe60:  3fff5163 3fff135c 00000015 3ffee910  
3ffffe70:  3ffe9914 3ffe9914 3fff1767 40258907  
3ffffe80:  3ffee8c4 402584d8 4025ab3c 3ffee8c4  
3ffffe90:  4010594d 0000c967 3ffee9f0 00000000  
3ffffea0:  3ffef100 3ffee9f0 00000001 00000001  
3ffffeb0:  00000000 000036eb 4025a4d3 3ffee9b8  
3ffffec0:  3ffee9f0 4025ab3c 3ffee8c4 00000000  
3ffffed0:  3ffee250 4025a448 3ffee1cc 00000012  
3ffffee0:  402584c8 4025849c 3ffee250 00000012  
3ffffef0:  40258467 3ffee1cc 00000012 4021b904  
3fffff00:  40258a18 3ffee1cc 00000000 4025ab3c  
3fffff10:  40258445 3ffee348 00000002 00000000  
3fffff20:  4025a8f5 0000010f 3ffef770 3fffdad0  
3fffff30:  4010594d 00180e84 3fff0a88 00000000  
3fffff40:  3ffef100 3fff0a88 00001388 3fff0a88  
3fffff50:  3fffdad0 3fff0a2c 4021c990 3fffefa0  
3fffff60:  3fff0a88 3fffdad0 3fff0a2c 4021cfa6  
3fffff70:  3fffdad0 3ffef770 3fff0684 40203f4c  
3fffff80:  3fff4f00 0027002f 802287d0 402287bc  
3fffff90:  3fff4f00 0021002f 80efeffe feefeffe  
3fffffa0:  feefeffe 00000000 3fff09ec 4021caac  
<<<stack<<<
 */

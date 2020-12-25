#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

class SoftAp {
    public:
        SoftAp(ESP8266WebServer &inServer);
        ESP8266WebServer *server;
        String ssid;
        String pwd;
        String mac;
        void setSoftAp();
        void resetSoftAp();
        bool isSoftAp();
        void watch();
        void setup(void (*inHandleRoot)(), void (*inHandleCredentials)(), void (*inHandleNotFound)());
        bool needCredentials();
        void handleCredentials();
        void handleRoot();
        void handleNotFound();

    private:
        unsigned long _startMillis;
        String _makePage(String body);
        bool _state;

};



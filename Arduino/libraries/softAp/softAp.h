#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <credentials.h>

class SoftAp {
    public:
        SoftAp(ESP8266WebServer &inServer, Credentials &credentials);
        ESP8266WebServer *server;
        Credentials *credentials;
        String mac;
        void begin();
        void end();
        bool isSoftAp();
        void watch();
        void setup(void (*inHandleRoot)(), void (*inHandleCredentials)(), void (*inHandleNotFound)());
        void handleCredentials();
        void handleRoot();
        void handleNotFound();

    private:
        unsigned long _startMillis;
        String _makePage(String body);
        bool _state;

};



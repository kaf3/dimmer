#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <credentials.h>

class FbClient {
    public:
        FbClient(Credentials &credentials);
        Credentials *credentials;
        FirebaseData data;
        FirebaseAuth auth;
        FirebaseConfig config;
        String path;
        void setup(void (*inValueReceived)(String value), String host, String apiKey);
        void begin();
        void watch();
        bool isClient();

    private:
        unsigned long _startMillis;
        unsigned int _counter;
        void (*_onValueReceived)(String value);

};
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
        void setup(void (*inValueReceived)(StreamData data), String host, String apiKey);
        void begin();
        void watch();
        bool isClient();
        bool reconnect();

    private:
        unsigned long _startMillis;
        void (*_onValueReceived)(StreamData data);
        void _createDataObject();
        void _errorDataHandler();
        void _createPath();
        void _createDevice();
        unsigned int _connectTryCounter;
        bool _initializing;
        void _beginStream();
};
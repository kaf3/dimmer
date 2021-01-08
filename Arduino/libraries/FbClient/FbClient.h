#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <credentials.h>

class FbClient {
    public:
        FbClient(Credentials &credentials);
        Credentials *credentials;
        FirebaseData data;
        FirebaseData postData;
        FirebaseAuth auth;
        FirebaseConfig config;
        String path;
        void setup(void (*inValueReceived)(MultiPathStreamData data), void (*inDeviceJson)(FirebaseJson &json), String host, String apiKey);
        void begin();
        void watch();
        bool isClient();
        bool reconnect();
        void postBool(String childPath, bool value);
        void postInt(String childPath, int value);

    private:
        unsigned long _startMillis;
        void (*_onValueReceived)(MultiPathStreamData data);
        void (*_populateDeviceJson)(FirebaseJson &json);
        void _createDataObject();
        void _errorDataHandler();
        void _createPath();
        void _createDevice();
        unsigned int _connectTryCounter;
        bool _initializing;
        void _beginStream();
};
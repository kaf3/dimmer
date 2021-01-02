#include <FbClient.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <credentials.h>

FbClient::FbClient(Credentials &inCredentials) {
    credentials = &inCredentials;
    _startMillis = 0;
    _counter = 0;
    _doesDataObjectExist = false;
    path = "/UsersData2/";
}

void FbClient::watch() {
    if (!isClient()) {
        return;
    }

    unsigned long currentMillis = millis();

    if (currentMillis - _startMillis >= 300) {
        _startMillis = currentMillis;
        _counter++;



        if ( _counter < 10) {
            return;
        }

        _counter = 0;

        Serial.print("http connected status of data ");
        Serial.println(data.httpConnected());

        if (Firebase.getJSON(data, path)) {

            Serial.println(data.dataType());
            Serial.println(path);

            if(data.dataType() == "json") {
                Serial.println(data.jsonString());
                (*_onValueReceived)(data.jsonString());
            }
        } else {
            Serial.println(data.errorReason());
            _errorDataHandler();
        }
        
    }
}

void FbClient::_errorDataHandler() {
    if (data.errorReason() == "Firebase authentication was not initialized" || data.errorReason() == "token is not ready") {
        auth.user.email = std::string(credentials->email.c_str());
        auth.user.password = std::string(credentials->upwd.c_str());
        Firebase.begin(&config, &auth);
    }
}

void FbClient::setup(void (*inValueReceived)(String value), String host, String apiKey) {
    _onValueReceived = *inValueReceived;
    config.host = std::string(host.c_str());
    config.api_key = std::string(apiKey.c_str());

    data.setResponseSize(4096);
}

bool FbClient::begin() {
    //WiFi.mode(WIFI_STA);
    WiFi.begin(credentials->ssid, credentials->pwd);

    int tries = 0;

    delay(1000);

/*     while(WiFi.status() != WL_CONNECTED && tries < 10) {
            Serial.print("wifi statuses = ");
            Serial.println(WiFi.status());
            delay(1000);
            tries++;
    } */

    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

    auth.user.email = std::string(credentials->email.c_str());
    auth.user.password = std::string(credentials->upwd.c_str());

    //Firebase.setMaxRetry(data, 5);
    //Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth);
    path = String("/UsersData2/") + auth.token.uid.c_str();

    return WiFi.status() == WL_CONNECTED;
}

bool FbClient::isClient() {
    return WiFi.getMode() == WIFI_STA;
}
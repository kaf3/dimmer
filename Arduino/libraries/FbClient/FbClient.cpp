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

        if (WiFi.status() != WL_CONNECTED || _counter < 10) {
            return;
        }

        _counter = 0;

        if (Firebase.getJSON(data, path)) {

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
}

bool FbClient::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(credentials->ssid, credentials->pwd);

    int tries = 0;

    while(WiFi.status() != WL_CONNECTED && tries < 20) {
            Serial.print("wifi statuses = ");
            Serial.println(WiFi.status());
            delay(300);
            tries++;
    }

    delay(300);

    if (WiFi.status() != WL_CONNECTED) {
        Serial.print("wifi connect timeout ");
        return false;
    }

    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

    auth.user.email = std::string(credentials->email.c_str());
    auth.user.password = std::string(credentials->upwd.c_str());

    Firebase.reconnectWiFi(true);
    data.setResponseSize(4096);
    Firebase.begin(&config, &auth);
    path = path + auth.token.uid.c_str();

    return true;
}

bool FbClient::isClient() {
    return WiFi.getMode() == WIFI_STA;
}
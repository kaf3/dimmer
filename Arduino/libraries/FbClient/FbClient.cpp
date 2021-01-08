#include <FbClient.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <credentials.h>

FbClient::FbClient(Credentials &inCredentials) {
    credentials = &inCredentials;
    _startMillis = 0;
    _connectTryCounter = 0;
    path = "";
    _initializing = false;
}

void FbClient::watch() {
    if (!isClient()) {
        return;
    }

    unsigned long currentMillis = millis();

    if (currentMillis - _startMillis >= 3000) {
        _startMillis = currentMillis;


        if (auth.token.uid.length() == 0) {
            Firebase.getShallowData(data, "/UsersData2/");
            return;
        } 

        if (_initializing) {
            _createPath();
            _createDevice();
            _beginStream();
            _initializing = false;
            return;
        }

        if (path.length() == 0) {
            Serial.println("Empty path");
            return;
        }
    }
}

void FbClient::_beginStream() {
    String childPath[3] = {"/switcher","/photoDetector", "/brightness"};
    size_t length = 3;
    if (!Firebase.beginMultiPathStream(data, path + "/equipmentCollection", childPath, length)) {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + data.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Firebase.setMultiPathStreamCallback(data, _onValueReceived);
}

void FbClient::_createPath() {
    path = "";
    String _path = String("/users/") + auth.token.uid.c_str() + String("/rooms");

    if (Firebase.getShallowData(data, _path)) {
        if(data.dataType() == "json") {
            FirebaseJson &json = data.jsonObject();
            String key, value = "";
            int type = 0;
            size_t index = 0;
            json.iteratorBegin();
            json.iteratorGet(index, type, key, value);
            json.iteratorEnd();
            path = _path + String('/') + key + String("/hardwareCollection/") + WiFi.macAddress();
        }
    } else {
            
            _errorDataHandler();
        } 
}

void FbClient::_createDevice() {
    if (Firebase.getShallowData(data, path + String("/mac"))) {
        if (data.dataType() == "null") {

            FirebaseJson hardware;
            (*_populateDeviceJson)(hardware);

            Serial.print("created json ");
            String jsonString;
            hardware.toString(jsonString, true);
            Serial.println(jsonString);

            if (!Firebase.updateNode(data, path, hardware)) {
                Serial.println(data.errorReason());
            }
        }
    }
}

bool FbClient::reconnect() {
    if (_connectTryCounter < 10) {
        _connectTryCounter++;
        Serial.println("try reconnect after 1s");
        delay(1000);
        WiFi.reconnect();
        return true;
    }
    _connectTryCounter = 0;

    return WiFi.status() == WL_CONNECTED;
}

void FbClient::postBool(String childPath, bool value) {
    if (!Firebase.setBool(postData, path + "/equipmentCollection/"+ childPath, value)) {
        _errorDataHandler();
    }
}

void FbClient::postInt(String childPath, int value) {
    if (!Firebase.setInt(postData, path + "/equipmentCollection/"+ childPath, value)) {
        _errorDataHandler();
    }
}

void FbClient::_errorDataHandler() {
    Serial.println(data.errorReason());
    if (data.errorReason() == "Firebase authentication was not initialized" || data.errorReason() == "token is not ready") {
        auth.user.email = std::string(credentials->email.c_str());
        auth.user.password = std::string(credentials->upwd.c_str());
        Firebase.begin(&config, &auth);
    }
    Serial.println(postData.errorReason());
}

void FbClient::setup(void (*inValueReceived)(MultiPathStreamData _data), void (*inDeviceJson)(FirebaseJson &json), String host, String apiKey) {
    _onValueReceived = *inValueReceived;
    _populateDeviceJson = *inDeviceJson;
    config.host = std::string(host.c_str());
    config.api_key = std::string(apiKey.c_str());

    //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
    data.setBSSLBufferSize(1024, 1024);
    data.setResponseSize(4096);

    postData.setBSSLBufferSize(1024, 1024);
    postData.setResponseSize(4096);
}

void FbClient::begin() {
    auth.user.email = std::string(credentials->email.c_str());
    auth.user.password = std::string(credentials->upwd.c_str());

    Firebase.begin(&config, &auth);
    _initializing = true;
}

bool FbClient::isClient() {
    return WiFi.getMode() == WIFI_STA;
}
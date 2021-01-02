#include <softAp.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <credentials.h>

SoftAp::SoftAp(ESP8266WebServer &inServer, Credentials &inCredentials) {
    server = &inServer;
    credentials = &inCredentials;
    _state = false;
    _startMillis = 0;

}

void SoftAp::watch() {
    if (!isSoftAp()) {
        return;
    }

    unsigned long currMillis = millis();

    if (currMillis - _startMillis >= 1000) {
        server->handleClient();
        _startMillis = currMillis;
    }


}

void SoftAp::begin() {
    WiFi.mode(WIFI_AP);

    WiFi.softAP("Dimmer Soft Ap");

    Serial.print("Setting AP (Access Point)…");

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
    Serial.println("");

    Serial.println(WiFi.localIP());

    mac = WiFi.softAPmacAddress();

    server->begin();
    Serial.println("HTTP server started");
};

void SoftAp::end() {
    WiFi.mode(WIFI_STA);
    Serial.print("reset ap and is soft ap");
    Serial.print(isSoftAp());
};

bool SoftAp::isSoftAp() {
    return WiFi.getMode() == WIFI_AP;
};

void SoftAp::setup(void (*inHandleRoot)(), void (*inHandleCredentials)(), void (*inHandleNotFound)()) {
    server->on("/", inHandleRoot);
    server->on("/credentials", inHandleCredentials);
    server->onNotFound(inHandleNotFound);
}

void SoftAp::handleRoot() {
    String body = "<div class=\"bar dark\">\
        Добро пожаловать!\
    </div>\
    <div class=\"bar light\">\
        mac-адрес: ";
    body += mac;
    body += "</div>\
    <div class=\"bar light\">\
        Настройте Wi-fi для устройства\
    </div>\
    <form action=\"/credentials\" method=\"post\">\
        <div class=\"field\">\
            <label for=\"ssid\">Имя сети</label>\
            <input type=\"text\" value=\"iPhone (Никита)\" name=\"ssid\">\
        </div>\
        <div class=\"field\">\
            <label for=\"pwd\">Пароль сети</label>\
            <input type=\"password\" value=\"qwerty1234\" name=\"pwd\">\
        </div>\
        <div class=\"field\">\
            <label for=\"email\">Имя пользователя Smart Home</label>\
            <input type=\"text\" value=\"nekit.97@bk.ru\" name=\"email\">\
        </div>\
        <div class=\"field\">\
            <label for=\"upwd\">Пароль пользователя Smart Home</label>\
            <input type=\"password\" value=\"123456\" name=\"upwd\">\
        </div>\
        <button>Сохранить</button>\
    </form>";

    server->send(200, "text/html", _makePage(body));
}

void SoftAp::handleCredentials() {
    String ssid = server->arg("ssid");
    String pwd = server->arg("pwd");
    String email = server->arg("email");
    String upwd = server->arg("upwd");
  
    if (ssid.length() * pwd.length() * email.length() * upwd.length() == 0) {
        return;
    }

    credentials->ssid = ssid;
    credentials->pwd = pwd;
    credentials->email = email;
    credentials->upwd = upwd;

    String body = "<div class=\"bar light\">\
        <span>Данные сохранены! </span>\
        <a href=\"/\"> Назад</a>\
    </div>";

    Serial.print("имя сети ");
    Serial.println(ssid);
    Serial.print("пароль сети ");
    Serial.println(pwd);

    Serial.print("имя пользователя ");
    Serial.println(email);
    Serial.print("пароль пароль пользователя ");
    Serial.println(upwd);

    server->send(200, "text/html", _makePage(body));
}

void SoftAp::handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server->uri();
    message += "\nMethod: ";
    message += (server->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server->args();
    message += "\n";

    for (uint8_t i = 0; i < server->args(); i++) {
        message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
    }

    server->send(404, "text/plain", message);
}

String SoftAp::_makePage(String body) {
        String style = "body {\
        background-color: #fafafa;\
        color: rgba(0,0,0,0.87);\
        padding: 0;\
        margin: 0;\
    }\
    form {\
        display: block;\
        width: 400px;\
        margin: auto;\
        font-weight: 400;\
        line-height: 1.125;\
        font-family: Roboto, \"Helvetica Neue\", sans-serif;\
        letter-spacing: normal;\
    }\
    .field {\
        border-radius: 4px 4px 0 0;\
        padding: .75em .75em 0 .75em;\
        margin: 1.5em 0;\
        background-color: rgba(0,0,0,0.04);\
        box-sizing: border-box;\
        width: 100%;\
        border-bottom: #3f51b5 2px solid;\
    }\
    input {\
        padding: .25em 0 .75em 0;\
        display: block;\
        width: 100%;\
        font: inherit;\
        background: transparent;\
        caret-color: #3f51b5;\
        border: none;\
        outline: none;\
    }\
    label {\
        color: #3f51b5;\
        display: block;\
        font-size: 12px;\
    }\
    .bar {\
        height: 64px;\
        width: 100%;\
        box-sizing: border-box;\
        padding: 16px;\
        font: 500 20px/32px Roboto, \"Helvetica Neue\", sans-serif;\
    }\
    .light {\
        background: #f5f5f5;\
        color: rgba(0,0,0,0.87);\
    }\
    .dark {\
        background: #3f51b5;\
        color: #fff;\
    }\
    button {\
        outline: none;\
        cursor: pointer;\
        border: none;\
        padding: 0 16px;\
        border-radius: 4px;\
        text-decoration: none;\
        background-color: #3f51b5;\
        color: #fff;\
        font: inherit;\
        height: 36px;\
        margin-bottom: 32px;\
    }";

    String page = "<!DOCTYPE html>\
        <html lang=\"ru\">\
        <head>\
        <meta charset=\"UTF-8\">\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
        <title>Dimmer</title>\
        </head>\
    <style>";
    page += style;
    page += "</style>";
    page += body;
    page += "</body></html>";
    
    return page;
}

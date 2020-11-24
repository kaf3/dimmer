#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ap_ssid = "SOFT_AP_ESP8266";
String ssid = "";
String pwd = "";
uint8_t macAddr[6];

ESP8266WebServer server(80);

void handleRoot() {
  char temp[2000];

  String body = "<div class=\"bar dark\">\
        Добро пожаловать!\
    </div>\
    <div class=\"bar light\">\
        Настройте Wi-fi для устройства\
    </div>\
    <form action=\"/credentials\" method=\"post\">\
        <div class=\"field\">\
            <label for=\"ssid\">Имя сети</label>\
            <input type=\"text\" name=\"ssid\">\
        </div>\
        <div class=\"field\">\
            <label for=\"pwd\">Пароль сети</label>\
            <input type=\"text\" name=\"pwd\">\
        </div>\
        <button>Сохранить</button>\
    </form>";

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
    }";
  
  snprintf(temp, 2000,

           "<html>\
  <head>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from Dimmer!</h1>\
    <h4>MAC address = %02x:%02x:%02x:%02x:%02x:%02x</h4>\
    <h4>Units = \%</h4>\
    <form method=\"post\" action=\"/credentials\">\
    <label for=\"ssid\"> ssid </label>\
    <input name=\"ssid\" type=\"text\" id=\"ssid\" >\
    <label for=\"pwd\">password</label>\
    <input name=\"pwd\" type=\"text\" id=\"pwd\" >\
    <button>send</button>\
    </form>\
  </body>\
</html>",
          macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]
          );
  server.send(200, "text/html", makePage(body, style));
}

void handleCredentials() {
  String ssidArg = server.arg("ssid");
  String pwdArg = server.arg("pwd");
  
  if (ssidArg.length() * pwdArg.length() != 0) {
    ssid = ssidArg;
    pwd = pwdArg;
  }
  
String temp = "<html>\
  <head>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Success!</h1>\
    <a href=\"/\">home</a>\
  </body>\
</html>";

  Serial.println(ssid);
  Serial.println(pwd);
  server.send(200, "text/html", temp);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

String makePage(String body, String style) {
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

void setup(void) {
  Serial.begin(115200);
  
  WiFi.softAP(ap_ssid);

  Serial.print("Setting AP (Access Point)…");

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  Serial.println("");

  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  WiFi.softAPmacAddress(macAddr);

  server.on("/", handleRoot);
  server.on("/credentials", handleCredentials);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}

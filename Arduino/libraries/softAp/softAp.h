#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <credentials.h>

class SoftAp {
    public:
        SoftAp(ESP8266WebServer &inServer, Credentials &credentials);
        ESP8266WebServer *server;       //ссылка на экземпляр класса сервера
        Credentials *credentials;       //ссылка на экземпляр класса учетных данных
        String mac;                     //mac-адресс устройства
        void begin();                   //запуск точки доступа
        void end();                     //выключение точки доступа
        bool isSoftAp();                //проверка включена ли точка доступа
        void watch();                   //обработка запросов к запущенному серверу
        void setup(void (*inHandleRoot)(), void (*inHandleCredentials)(), void (*inHandleNotFound)());
                                        //конфигурация точки доступа
        void handleCredentials();       //обработка запроса введенных учетных данных на сервер
        void handleRoot();              //обработка корневого запроса на сервер
        void handleNotFound();          //обработка неннайденной страницы

    private:
        unsigned long _startMillis;     //время последней обработки запросов клиента
        String _makePage(String body);  //создание web-страницы из каркаса
};

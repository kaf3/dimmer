#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <credentials.h>

class FbClient {
    public:
        FbClient(Credentials &credentials);
        Credentials *credentials;                   //ссылка на экземпляр класса учетных данных
        FirebaseData data;                          //объект полученных данных
        FirebaseData postData;                      //объект отправленных данных
        FirebaseAuth auth;                          //объект авторизации Firebase
        FirebaseConfig config;                      //объект конфигурации Firebase
        String path;                                //путь в бд до диммера
                                                    //кофигурация клиента
        void setup(void (*inValueReceived)(MultiPathStreamData data), void (*inDeviceJson)(FirebaseJson &json), String host, String apiKey);
        void begin();                               //старт клиента
        void watch();                               //отслеживание авторизации и инициализации
        bool isClient();                            //проверка, что включен режим станции (клиента)
        bool reconnect();                           //переподключение к точке доступа Wi-Fi
        void postBool(String childPath, bool value);//записать булевы данные в бд
        void postInt(String childPath, int value);  //записать целочисленные данные в бд

    private:
        unsigned long _startMillis;                 //время с поледней проверки состояния авторизации
        void (*_onValueReceived)(MultiPathStreamData data);//функция обратного вызова, когда данные в бд изменилась
        void (*_populateDeviceJson)(FirebaseJson &json);//функция обратного вызова на заполнение json
        void _errorDataHandler();                   //обработка ошибок при отправке или получении данных
        void _createPath();                         //определение пути до диммера
        void _createDevice();                       //создание объекта диммера в бд, если его нет
        unsigned int _connectTryCounter;            //счетчик попыток переподключения
        bool _initializing;                         //флаг инициализации
        void _beginStream();                        //включение потока передачи данных от бд в диммер
};
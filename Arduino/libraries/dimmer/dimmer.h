#include <Arduino.h>
#include <ESP8266WiFi.h>

class Dimmer {
    public:
        Dimmer(int inLoadPin, int inNullDetectedPin);
        int loadPin;                    //пин выхода на нагрузку
        int nullDetectedPin;            //пин входа с детектора перехода через ноль  
        void interrupt();               //установка флага перехода через ноль 
        void watch();                   //управление сигналом на выходе
        void setLevel(int inLevel);     //установка уровня диммирования
        void increase();                //увелечинение уровня диммирования
        void decrease();                //уменьшение уровня диммирования
        void on();                      //включение диммера
        void off();                     //выключение диммера
        void toggle();                  //переключение диммера
        bool isOn();                    //проверка включен ли диммер
        void setup(void (*dtct)());     //конфиграция диммера 
        int getLevel();                 //получение уровня диммера
        unsigned long getMicrosLevel(); //получение времени обрезания синусоиды
        void resume();                  //возобновить детекцию нуля
        void pause();                   //приостановить детекцию нуля
    
    private:
        bool _nullDetected;             //флаг перехода через ноль
        bool _state;                    //состояние диммера (вкл/выкл)
        unsigned long _microsLevel;     //время обрезания синусоиды
        int _level;                     //уровень диммирования 0 - 100
        unsigned long _startMicros;     //время перехода через ноль
        void (*_detect)();              //прерывание при переходе через ноль
        static unsigned long _getMicrosByLevel(int level); //вычисление времени 
                                                            //обрезания синусоиды
                                                            //по уровню диммирования
};                                                           

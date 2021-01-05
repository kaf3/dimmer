#include <Arduino.h>

class Button {
    public:
        Button(int inPin, void (*inOnClick)(), void (*inOnLongClick)());
        int pin;                        //пин входа кнопки
        static unsigned long interval;  //интервал времени для
                                        //отслеживания долгого нажатия
        int read();                     //считать уровень сигнала с пина
        void watch();                   //отслеживание прерывания
        void setup(void (*dtct)());     //конфигурирование пина для кнопки
        void interrupt();               //выставление флага прерывания

    private:
        void _process();                //обработка нажатий
        bool _detected;                 //флаг прерывания
        unsigned long _startMillis;     //время старта нажатия
        void (*_onClick)();             //функция обратного вызова
                                        //для короткого нажатия
        void (*_onLongClick)();         //функция обратного вызова
                                        //для длительного нажатия
};
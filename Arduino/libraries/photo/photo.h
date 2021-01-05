#include <Arduino.h>

class Photo {
    public:
        Photo(int inPin, void (*inIncreaseLevel)(), void (*inDecreaseLevel)());
        int pin;                        //пин входа с фотодетектора
        static unsigned long interval;  //интервал считывания 
        int requiredValue;              //требуемый уровень освещенности
        void watch();                   //накполение значений и 
                                        //корректировка уровня при необходимости
        void setAutoMode();             //включение авто-коррекции
        void resetAutoMode();           //выключение авто-коррекции
        bool isAutoMode();              //включена ли авто-коррекции

    private:
        unsigned long _startMillis;         //время считывания
        bool _autoMode;                     //состояние авто-коррекции
        bool _settingAutoMode;              //флаг процесса включения авто-коррекции
        int _prevOffset;                    //отклонение при предыдущем считывании
        int _read();                        //чтение с фотодетектора 
        void _adjustLevel(int readedFromBuffer);//слежение за показаниями для коррекции
        bool _needToAdjust(int offset);     //нужна ли коррекция уровня
        int _values[20];                    //буффер накопленных значений
        int _counter;                       //счетчик для буффера накопленных значений
        void (*_increaseLevel)();           //функция обратного вызова 
                                            //при необходимости увеличить освещенность
        void (*_decreaseLevel)();           //функция обратного вызова 
                                            //при необходимости уменьшить освещенность
        void _adjustByOffset(int offset);   //коррекция уровня
        void _writeToBuffer(int value);     //запись в буффер данных
        int _readFromBuffer();              //чтение из буфера среднего по всем значениям
        void _setRequiredValue(int readedFromBuffer);//установка требуемого уровня освещенности
        
};



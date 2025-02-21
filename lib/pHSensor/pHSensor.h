#ifndef PHSENSOR_H
#define PHSENSOR_H

#include <Arduino.h>

class PHSensor {
public:
    PHSensor(uint8_t pin, float slope = -0.23f, float offset = 0);
    void begin();
    float readPH();

private:
    uint8_t _pin;
    float _slope;
    float _offset;
};

#endif

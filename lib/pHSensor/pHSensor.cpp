#include "pHSensor.h"

PHSensor::PHSensor(uint8_t pin, float slope, float offset) {
    _pin = pin;
    _slope = slope;
    _offset = offset;
}

void PHSensor::begin() {
    pinMode(_pin, INPUT);
}

float PHSensor::readPH() {
    int analogValue = analogRead(_pin);
    float voltage = analogValue * (3.3 / 4095.0);
    float pHValue = 7 + ((voltage - 1.65) / _slope); 
    pHValue += _offset;
    return pHValue;
}

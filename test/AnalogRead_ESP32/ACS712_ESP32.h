#ifndef ACS712_ESP32_H
#define ACS712_ESP32_H

#include <Arduino.h>

class ACS712 {
public:

    ACS712(int pin, float vref = 3.3, int adcMax = 4095, float sensitivity = 0.066);

   
    void calibrateOffset(int numSamples = 100);


    float readCurrent();

private:
    int _pin;             
    float _vref;          
    int _adcMax;         
    float _sensitivity;  
    int _offsetValue;     

    int readAverageADC(int numSamples = 100);
};

#endif

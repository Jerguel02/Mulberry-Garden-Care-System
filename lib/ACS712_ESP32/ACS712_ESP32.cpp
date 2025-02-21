#include "ACS712_ESP32.h"

ACS712::ACS712(int pin, float vref, int adcMax, float sensitivity) 
    : _pin(pin), _vref(vref), _adcMax(adcMax), _sensitivity(sensitivity), _offsetValue(0) {}

void ACS712::calibrateOffset(int numSamples) {
    _offsetValue = readAverageADC(numSamples);
    Serial.print("Calibrated Offset Value: ");
    Serial.println(_offsetValue);
}

int ACS712::readAverageADC(int numSamples) {
    long sum = 0;
    for (int i = 0; i < numSamples; i++) {
        sum += analogRead(_pin);
        delay(1);
    }
    return sum / numSamples;
}

float ACS712::readCurrent() {
    int adcValue = readAverageADC(10); 
    Serial.print("ACS ADC: ");
    Serial.println(adcValue);
    int adjustedValue = adcValue - _offsetValue;
    float voltage = (adjustedValue * _vref) / _adcMax;
    return (abs(voltage / _sensitivity) >= 0.25) ? abs(voltage / _sensitivity) : 0;
}

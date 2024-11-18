#ifndef ACS712_ESP32_H
#define ACS712_ESP32_H

#include <Arduino.h>

class ACS712 {
public:
    // Constructor
    ACS712(int pin, float vref = 3.3, int adcMax = 4095, float sensitivity = 0.066);

    // Hàm hiệu chuẩn offset
    void calibrateOffset(int numSamples = 100);

    // Hàm đọc dòng điện
    float readCurrent();

private:
    int _pin;             // Chân ADC của cảm biến
    float _vref;          // Điện áp tham chiếu (ESP32 thường là 3.3V)
    int _adcMax;          // Độ phân giải ADC (ESP32 là 4095)
    float _sensitivity;   // Độ nhạy của ACS712 (30A = 66mV/A)
    int _offsetValue;     // Giá trị offset sau khi hiệu chuẩn

    // Hàm đọc giá trị ADC trung bình
    int readAverageADC(int numSamples = 100);
};

#endif // ACS712_H

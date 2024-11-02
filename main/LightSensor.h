#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>

class LightSensor {
  private:
    int analogPin;  
    int digitalPin; 
    int threshold;  
    
  public:

    LightSensor(int aPin, int dPin, int thres);

    void begin();

    int readAnalog();


    bool isLightDetected();
};

#endif

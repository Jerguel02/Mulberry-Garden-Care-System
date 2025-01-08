#ifndef WATER_SENSOR_H
#define WATER_SENSOR_H

#include <Arduino.h>

class WaterSensor {
  private:
    int analogPin;       
    float maxHeightCm;   
    
  public:

    WaterSensor(int aPin, float maxHeight);


    int readAnalog();


    float getWaterLevelCm();
};

#endif

#include "WaterSensor.h"


WaterSensor::WaterSensor(int aPin, float maxHeight) {
  analogPin = aPin;
  maxHeightCm = maxHeight;
  pinMode(analogPin, INPUT);
}


int WaterSensor::readAnalog() {
  return analogRead(analogPin);
}


float WaterSensor::getWaterLevelCm() {
  int analogValue = readAnalog();
  return (analogValue / 4095.0) * maxHeightCm;
}

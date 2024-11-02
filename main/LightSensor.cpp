#include "LightSensor.h"


LightSensor::LightSensor(int aPin, int dPin, int thres) {
  analogPin = aPin;
  digitalPin = dPin;
  threshold = thres;

}

void ::LightSensor::begin()
{
  pinMode(analogPin, INPUT);
  pinMode(digitalPin, INPUT);
}
int LightSensor::readAnalog() {
  return analogRead(analogPin);
}


bool LightSensor::isLightDetected() {
  return digitalRead(digitalPin) == LOW;
}

#include "ACS712_ESP32.h"

#define ACS_PIN1  32 
#define ACS_PIN2  34
#define ACS_PIN3  33
#define PIN1      16
#define PIN2      23
#define PIN3      17
ACS712 acsSensor1(ACS_PIN1, 5.0, 1024, 0.066); // Khởi tạo đối tượng ACS712 (30A)
ACS712 acsSensor2(ACS_PIN2, 5.0, 1024, 0.066); // Khởi tạo đối tượng ACS712 (30A)
ACS712 acsSensor3(ACS_PIN3, 5.0, 1024, 0.066); // Khởi tạo đối tượng ACS712 (30A)
void setup() {
  Serial.begin(115200);
  delay(2000); // Chờ ổn định hệ thống

  // Hiệu chuẩn cảm biến
  Serial.println("Calibrating...");
  acsSensor1.calibrateOffset();
  acsSensor2.calibrateOffset();
  acsSensor3.calibrateOffset();
  delay(1000);
  pinMode(PIN1, OUTPUT);
  pinMode(PIN2, OUTPUT);
  pinMode(PIN3, OUTPUT);
}

void loop() {
  // Đọc dòng điện
  float current1 = acsSensor1.readCurrent();
  float current2 = acsSensor2.readCurrent();
  float current3 = acsSensor3.readCurrent();
  Serial.print("Current1 (A): ");
  Serial.println(current1);
  Serial.print("Current2 (A): ");
  Serial.println(current2);
  Serial.print("Current3 (A): ");
  Serial.println(current3);
  Serial.println();
  delay(1000); // Đọc mỗi giây
}

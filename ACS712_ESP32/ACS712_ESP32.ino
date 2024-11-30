#include "ACS712_ESP32.h"

#define ACS_PIN1  A0 
#define ACS_PIN2  A1
#define ACS_PIN3  A2 

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

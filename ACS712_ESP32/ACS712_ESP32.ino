#include <ACS712_ESP32.h>

const int ACS_PIN1 = 34; 
const int ACS_PIN2 = 35; 
ACS712 acsSensor1(ACS_PIN1, 3.3, 4095, 0.066); // Khởi tạo đối tượng ACS712 (30A)
ACS712 acsSensor2(ACS_PIN2, 3.3, 4095, 0.066); // Khởi tạo đối tượng ACS712 (30A)
void setup() {
  Serial.begin(115200);
  delay(2000); // Chờ ổn định hệ thống

  // Hiệu chuẩn cảm biến
  Serial.println("Calibrating...");
  acsSensor1.calibrateOffset();
  acsSensor2.calibrateOffset();
}

void loop() {
  // Đọc dòng điện
  float current1 = acsSensor1.readCurrent();
  float current2 = acsSensor2.readCurrent();
  Serial.print("Current1 (A): ");
  Serial.println(current1);
  Serial.print("Current2 (A): ");
  Serial.println(current2);
  Serial.println();
  delay(1000); // Đọc mỗi giây
}

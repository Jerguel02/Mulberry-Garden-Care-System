/*****************************************************************************************************************************
****************************************************MỘT SỐ LƯU Ý**************************************************************
*******Sơ đồ chân:                                                                                                          **
**TFT Pin         CTP_INT    CTP_SDA   CTP_RTS   CTP_SCL   SDO(MISO)   LED   SCK   SDI(MOSI)   LCD_RS    LCD_RST   LCD_CS   **
**GPIO               32        33        25        12          12       X     14       13        2         27        15     **
**Sensor: Light Sensor: Analog Pin:  26  |pH Sensor:     34  | DHT11: 4                                                     **
**                      Digital Pin: 5   |Water Sensor:  35  |                                                              **
********Mục lục mã nguồn:                                                                                                   **
**Phần tiền xử lý                                                                                                           ** 
**Phần khai báo module                                                                                                      **
**Phần khai báo biến                                                                                                        **
**Hàm phụ toàn cục                                                                                                          **
**Hàm thiết lập                                                                                                             **
**Phần khởi tạo task                                                                                                        **
**Hàm xử lý toàn cục                                                                                                        **
******************************************************************************************************************************
*****************************************************************************************************************************/





////////////////////////////////////////////////////////////////////////////////
//                Phần tiền xử lý/Preprocessing Section                       //  
////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <TFT_eSPI.h>                     
//#include <FirebaseESP32.h> 
#include <Firebase.h>          
#include <DHT.h>                
#include "touch.h"
#include "LightSensor.h"
#include "WaterSensor.h"
#include "my_images.h"
#include "pHSensor.h"
#include "WiFi.h"
#include <esp_task_wdt.h>
// Firebase config
#define DATABASE_URL "smartgarden-86bab-default-rtdb.firebaseio.com"
#define DATABASE_SECRET "BOj3P8OdeNNEPdyPZ8g4sTu0A4N9QUWjgnDKX6FR"
#define API_KEY "AIzaSyCcmsIiBMejWivNKMthf5kWbzLKeTKJ2sw"
#define WIFI_SSID "PhamNghia"
#define WIFI_PASSWORD "244466666"

#define DHTPIN                  4
#define DHTTYPE               DHT11
#define PH_SENSOR_PIN           34
#define WATER_LEVEL_SENSOR_PIN  35
#define WATER_MAXIMUM_HEIGHT    4.0
#define PUMP_PIN                21
#define LIGHT_ANALOG_PIN        26
#define LIGHT_DIGITAL_PIN        5
#define LIGHT_THRES             500
#define LIGHT_PIN               19
#define TIMER_FREQ              2000
#define TFT_BACKGROUND          0x0f1b
#define WHITE                   0xFFFF
////////////////////////////////////////////////////////////////////////////////
//                Phần khai báo module/Module Declaration Section             //    
////////////////////////////////////////////////////////////////////////////////

PHSensor pHSensor(PH_SENSOR_PIN); 
LightSensor lightSensor(LIGHT_ANALOG_PIN, LIGHT_DIGITAL_PIN, LIGHT_THRES);
WaterSensor waterSensor(WATER_LEVEL_SENSOR_PIN, WATER_MAXIMUM_HEIGHT);
DHT dht(DHTPIN, DHTTYPE);
////////////////////////////////////////////////////////////////////////////////
//                Phần khai báo biến/Var Declaration Section                  //    
////////////////////////////////////////////////////////////////////////////////
/*esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 10000,  
    .trigger_panic = true
};*/
int x, y = 0;
TFT_eSPI tft = TFT_eSPI();
int screenWidth = 320; 
int screenHeight = 480;
int debounceDelay = 500;
int lastTouchTime = 0;
bool touchProcessed = false;
float prevTemperature = -1, prevHumidity = -1, prevWaterLevel = -1;
float prevPHValue = -1;
float temperature = 0;
float humidity = 0;
float waterLevel = 0;
float pHValue = 0;
bool light_detect = false;
bool prevLightDetect = false;
bool isCollectingData = false;
uint8_t processState = 0;
uint8_t processFirebase = 0;
bool isCollectingDataFromFB = false;
volatile unsigned long lastTouchInterruptTime = 0;
volatile bool interruptFlag = false;
bool isControlling = false;
uint8_t cor_x;
uint8_t cor_y;
TaskHandle_t xfetchStateTaskHandle;
TaskHandle_t xfetchSensorTaskHandle;
// Flags for sensor reading intervals
unsigned long lastTouchCheck = 0;
unsigned long lastFirebaseControlCheck = 0;
unsigned long lastTemperatureCheck = 0;
unsigned long lastHumidityCheck = 0;
unsigned long lastWaterLevelCheck = 0;
unsigned long lastPHCheck = 0;
unsigned long lastLightDetectCheck = 0;
String path = "/";
hw_timer_t* Firebase_timer;
Firebase Firebase(DATABASE_URL);
bool pumpOn = false;
bool lightOn = false;
volatile SemaphoreHandle_t timerSemaphore;

////////////////////////////////////////////////////////////////////////////////
//           Task Functions Replaced with Timer-Based Functions               //
////////////////////////////////////////////////////////////////////////////////



/*void controlDevicesFromTouch() {
  ts.read();
  if (ts.isTouched && !touchProcessed) 
  {
    unsigned long currentTime = millis();
    if (ts.isTouched && (currentTime - lastTouchTime > debounceDelay))
    {
      lastTouchTime = currentTime;
 
      Serial.print("Touch: ");
      Serial.print("  x: ");Serial.print(x);
      Serial.print("  y: ");Serial.print(y);
      Serial.println(' ');
      if (cor_x < screenWidth / 2 && cor_y < screenHeight / 2) {
        pumpOn = !pumpOn;
        digitalWrite(PUMP_PIN, pumpOn ? HIGH : LOW);
        Firebase.setBool("/controls/pump", pumpOn);
        Serial.println("Pump Control");
      } else if (cor_x >= screenWidth / 2 && cor_y < screenHeight / 2) {
        lightOn = !lightOn;
        digitalWrite(LIGHT_PIN, lightOn ? HIGH : LOW);
        Firebase.setBool("/controls/light", lightOn);
        Serial.println("LED Control");
      } else {
        Serial.println("Invalid touch point!");
      }
    }
  }
  if (!ts.isTouched) 
  {
    touchProcessed = false; // Cho phép xử lý chạm tiếp theo
  }
  Serial.println("Devices Controlled from Touch");
}*/

void fetchPumpState() {
    bool pumpState = Firebase.getBool("/controls/pump");
    if (pumpOn != pumpState) {
        pumpOn = pumpState;
        digitalWrite(PUMP_PIN, pumpOn ? HIGH : LOW);
        Serial.printf("Pump state updated: %s\n", pumpOn ? "ON" : "OFF");
    }
}

void fetchLightState() {
    bool lightState = Firebase.getBool("/controls/light");
    if (lightOn != lightState) {
        lightOn = lightState;
        digitalWrite(LIGHT_PIN, lightOn ? HIGH : LOW);
        Serial.printf("Light state updated: %s\n", lightOn ? "ON" : "OFF");
    }
}

void readTemperature() {
    temperature = dht.readTemperature();
    if (!isnan(temperature)) {
        updateSensorValue("/sensors/temperature", temperature, prevTemperature, "Temperature: ", 54, 15);
        Serial.printf("Temperature: %.2f\n", temperature);
    }
}

void readHumidity() {
    humidity = dht.readHumidity();
    if (!isnan(humidity)) {
        updateSensorValue("/sensors/humidity", humidity, prevHumidity, "Humidity: ", 54, 15 + 52);
        Serial.printf("Humidity: %.2f\n", humidity);
    }
}

void readWaterLevel() {
    // Replace with actual water level sensor logic
    waterLevel = waterSensor.getWaterLevelCm(); // Placeholder
    updateSensorValue("/sensors/waterLevel", waterLevel, prevWaterLevel, "Water Level: ", 54, 15 + 52 * 2);
    Serial.printf("Water Level: %.2f\n", waterLevel);
}

void readPHValue() {
    // Replace with actual pH sensor logic
    pHValue = pHSensor.readPH(); // Placeholder
    updateSensorValue("/sensors/phValue", pHValue, prevPHValue, "pH: ", 54, 15 + 52 * 3);
    Serial.printf("pH Value: %.2f\n", pHValue);
}

void readLightDetect() {
    light_detect = lightSensor.isLightDetected(); // Placeholder
    updateBoolSensorValue("/sensors/lightDetect", light_detect, prevLightDetect, "Light Detect: ", 54, 15 + 52 * 4);
    Serial.printf("Light Detect: %s\n", light_detect ? "True" : "False");
}
void updateSensorValue(const char* path, float newValue, float& prevValue, const char* label, int x, int y) {
    if (!isnan(newValue) && prevValue != newValue) {
      //if (Firebase.setFloat(fbdo, path, newValue)) 
      //if (Firebase.setFloat(path, newValue)) 
      //{
      tft.fillRect(x, y, 225, 20, TFT_BACKGROUND);
      tft.setCursor(x, y);
      tft.print(label);
      tft.println(newValue);
      prevValue = newValue;
      //}
    }
}

void updateBoolSensorValue(const char* path, bool newValue, bool& prevValue, const char* label, int x, int y) {
    if (prevValue != newValue) {
      //if (Firebase.setBool(path, newValue)) 
      //{
      tft.fillRect(x, y, 225, 20, TFT_BACKGROUND);
      tft.setCursor(x, y);
      tft.print(label);
      tft.println(newValue ? "True" : "False");
      prevValue = newValue;
      //} 
    } 
    else {
      tft.fillRect(x, y, 225, 20, TFT_BACKGROUND);
      tft.setCursor(x, y);
      tft.print(label);
      tft.println(prevValue ? "True" : "False");
    }
}
void fetchSensor_Task(void *pvParameters)
{
  while (true)
  {
      Serial.print("Task fetchSensor_Task is running on core: ");
      Serial.println(xPortGetCoreID());

      //Serial.println("ControlDevicesFromTouch...");
      //controlDevicesFromTouch();
      Serial.println("readTemperature...");
      readTemperature();
      Serial.println("readHumidity...");
      readHumidity();
      Serial.println("readWaterLevel...");
      readWaterLevel();
      Serial.println("readPHValue...");
      readPHValue();
      Serial.println("readLightDetect...");
      readLightDetect();
      vTaskDelay(3000 / portTICK_PERIOD_MS); 
  }
}
void  fetchFirebase_Task(void *pvParameters)
{
  while (1)
  {
    Serial.print("Task fetchFirebase_Task is running on core: ");
    Serial.println(xPortGetCoreID());
    Serial.println("fetchPumpState...");
    fetchPumpState();
    Serial.println("fetchLightState...");
    fetchLightState();
    switch (processFirebase)
    {
      case 0:
        Serial.println("readTemperature in Task...");
        Firebase.setFloat("/sensors/temperature", prevTemperature);
        processFirebase++;
        break;
      case 1:
        Serial.println("readHumidity in Task...");
        Firebase.setFloat("/sensors/humidity", prevHumidity);
        processFirebase++;
        break;
      case 2:
        Serial.println("readWaterLevel in Task...");
        Firebase.setFloat("/sensors/waterLevel", prevWaterLevel);
        processFirebase++;
        break;
      case 3:
        Serial.println("readPHValue in Task...");
        Firebase.setFloat("/sensors/phValue", prevPHValue);
        processFirebase++;
        break;
      case 4:
        Serial.println("readLightDetect in Task...");
        Firebase.setFloat("/sensors/lightDetect", prevLightDetect);
        processFirebase = 0;
        break;
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

/*void fetchState_Task(void *pvParameters)
{
  esp_task_wdt_delete(xTaskGetCurrentTaskHandle());
  while (true)
  {

    Serial.println("fetchPumpState...");
    fetchPumpState();
    Serial.print("Stack 2 high water mark: ");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));  // In ra số words còn lại trong stack
    //esp_task_wdt_reset();
    Serial.println("fetchLightState...");
    fetchLightState();
    Serial.print("Stack 2 high water mark: ");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));  // In ra số words còn lại trong stack
    //esp_task_wdt_reset();
    //lastFirebaseControlCheck = millis();
    //yield();
    delay(1000);
  }
}*/

////////////////////////////////////////////////////////////////////////////////
//                Hàm thiết lập/Setup Section                                 //  
////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);

  //pH
  pHSensor.begin(); 
  //Light Sensor
  lightSensor.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  uint8_t TryCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    TryCount++;
    //WiFi.disconnect();
    //WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(1000);
    /*if ( TryCount == 10 )
    {
      ESP.restart();
    }*/
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  /*Firebase_timer = timerBegin(1000000);
  timerAttachInterrupt(Firebase_timer, &fetchSensor_Task);
  timerAlarm(Firebase_timer, 3000000, true, 0);*/
  dht.begin();

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
   // TFT
  pinMode(TOUCH_FT6336_INT, INPUT);

  tft.init();
  tft.setRotation(3);

  touch_init(tft.width(), tft.height(), tft.getRotation());
  tft.fillScreen(TFT_BACKGROUND);
  tft.setTextColor(0xc0d5);
  tft.setTextSize(2); // Đặt kích thước chữ

  tft.setCursor(54, 15);
  tft.print("Temp: --.--");
  tft.setCursor(54, 15 + 52);
  tft.print("Humidity: --.--"); 
  tft.setCursor(54, 15 + 52*2);
  tft.print("Water Level: -.--");
  tft.setCursor(54, 15 + 52*3);
  tft.print("pH: --.--");
  tft.setCursor(54, 15 + 52*4);
  tft.print("Light Detect: -----");
  delay(500);
  fetchPumpState();
  fetchLightState();
  readTemperature();
  readHumidity();
  readWaterLevel();
  readPHValue();
  readLightDetect();
  //timerSemaphore = xSemaphoreCreateBinary();
  //esp_task_wdt_init(&wdt_config);
  BaseType_t result1 = xTaskCreate(
    fetchSensor_Task,        // Hàm thực hiện task
    "fetchSensorState",           // Tên task
    40000,                    // Kích thước stack (words)
    NULL,                     // Tham số truyền vào
    1,                        // Độ ưu tiên của task
    &xfetchSensorTaskHandle      // Task handle
    //0                        // Chạy trên core 1
  );
  if (result1 == pdPASS) {
    Serial.println("Task 1 created successfully.");
  } else {
    Serial.println("Task 1 creation failed.");
  }
  delay(1000);
  BaseType_t result2 = xTaskCreate(
    fetchFirebase_Task,        // Hàm thực hiện task
    "fetchState",           // Tên task
    40000,                    // Kích thước stack (words)
    NULL,                     // Tham số truyền vào
    1,                        // Độ ưu tiên của task
    &xfetchStateTaskHandle      // Task handle
    //0                       // Chạy trên core 0
  );
  if (result2 == pdPASS) {
    Serial.println("Task 2 created successfully.");
  } else {
    Serial.println("Task 2 creation failed.");
  }
  delay(1000);

}

  //Firebase.setReadTimeout(fbdo, 2000);
  
  //attachInterrupt(digitalPinToInterrupt(TOUCH_FT6336_INT), touchInterrupt, FALLING);


/////////////////////////////////////////////////////////////////////////////////
//         Hàm ngắt I2C/I2C Interrupt function Section                         //  
/////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//                Hàm xử lý toàn cục/Processing Section                       //  
////////////////////////////////////////////////////////////////////////////////
void drawIcon(int x, int y, int height, int width, const uint16_t *icon) {
  for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
          if (icon[i * width + j] != WHITE) { 
              tft.drawPixel(x + j, y + i, icon[i * width + j]); 
          }
      }
  }
}



void loop() {
  vTaskDelete(NULL);
  /*if (!isCollectingData)
  {
    isCollectingData = true;
    Serial.println("fetchPumpState...");
    fetchPumpState();
    Serial.println("fetchLightState...");
    fetchLightState();
    isCollectingData = false;
  }
  if (!isCollectingData)
  {
    switch (processState)
    {
      case 0:
        Serial.println("readTemperature...");
        readTemperature();
        processState = 1;
        break;
      case 1:
        Serial.println("readHumidity...");
        readHumidity();
        processState = 2;
        break;
      case 2:
        Serial.println("readWaterLevel...");
        readWaterLevel();
        processState = 3;
        break;
      case 3:
        Serial.println("readPHValue...");
        readPHValue();
        processState = 4;
        break;
      case 4:
        Serial.println("readLightDetect...");
        readLightDetect();
        processState = 0;
        break;
    }
  }*/
}

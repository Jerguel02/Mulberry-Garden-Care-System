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
//#include <esp_task_wdt.h>
// Firebase config
#define DATABASE_URL "smartgarden-86bab-default-rtdb.firebaseio.com"
#define DATABASE_SECRET "BOj3P8OdeNNEPdyPZ8g4sTu0A4N9QUWjgnDKX6FR"
#define API_KEY "AIzaSyCcmsIiBMejWivNKMthf5kWbzLKeTKJ2sw"


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
#define TFT_BACKGROUND          0x0F1B
#define WHITE                   0xFFFF
#define DEFAULT_TXT_COLOR       0xC0D5
#define DETAIL_TXT_COLOR        0xFA24
#define DEBOUNCE_TIME             200
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
String WIFI_SSID = "PhamNghia";
String WIFI_PASSWORD = "244466666";
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
int cor_x = 0;
int cor_y = 0;
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
//hw_timer_t* Firebase_timer;
Firebase Firebase(DATABASE_URL);
bool pumpOn = false;
bool lightOn = false;
bool lightStateChanging = false;
bool pumpStateChanging = false;
volatile bool touchDetected = false;
bool autoOn = false;
bool prev_wifiState = false;
unsigned long lastCheckWifi = 0;
unsigned long checkWifiInterval = 5000;
unsigned long lastWifiCheck = 0;
bool showSSID = true;
////////////////////////////////////////////////////////////////////////////////
//           Task Functions Replaced with Timer-Based Functions               //
////////////////////////////////////////////////////////////////////////////////


/*void IRAM_ATTR onTouchInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  //Serial.println("ISR Activated!");
  touchDetected = true;
  portEXIT_CRITICAL_ISR(&mux);
}*/

bool isIconTouched(int touchX, int touchY, int iconX, int iconY, int iconW, int iconH) {
  return (touchX >= iconX && touchX <= (iconX + iconW) &&
          touchY >= iconY && touchY <= (iconY + iconH));
}

bool isCircleTouched(int x_touch, int y_touch, int x_center, int y_center, int radius) {
  int dx = x_touch - x_center;
  int dy = y_touch - y_center;
  return (dx * dx + dy * dy <= radius * radius);  // Kiểm tra điều kiện
}
void controlLedFromTouch()
{
  lightStateChanging = true;
  lightOn = !lightOn;
  digitalWrite(LIGHT_PIN, lightOn ? HIGH : LOW);
  touchDetected = false;
  
}
void controlPumpFromTouch()
{
  pumpStateChanging = true;
  autoOn = !autoOn;
  Serial.print(("AUTO MODE DONE! Auto: "));
  Serial.println(autoOn ? "ON" : "OFF");
  Serial.println(("AUTO ON DONE!"));
  //readAutoPump();
  touchDetected = false;
}
void controlDevicesFromTouch() {
  while (touchDetected)
  {
    if (isIconTouched(cor_x, cor_y, 0, 52 * 4 + 1, 52, 52))
    {
      lightStateChanging = true;
      lightOn = !lightOn;
      digitalWrite(LIGHT_PIN, lightOn ? HIGH : LOW);
      touchDetected = false;
      break;
    }
    else if (isIconTouched(cor_x, cor_y, 0, 52 * 3 + 1, 52, 52))
    {
      autoOn = !autoOn;
      if (autoOn)
      {
        Serial.println(("AUTO ON!"));
        touchDetected = false;
        Serial.println(("AUTO ON DONE!"));
        break;
      }
      else
      {
        Serial.println(("AUTO OFF!"));
        touchDetected = false;
        Serial.println(("AUTO OFF DONE!"));
        break;
      }
    }
  }
}
void drawIcon(int x, int y, int height, int width, const uint16_t *icon) {
  for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
          if (icon[i * width + j] != WHITE) { 
              tft.drawPixel(x + j, y + i, icon[i * width + j]); 
          }
      }
  }
}

void clearIcon(int x, int y, int height, int width) {
  for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        tft.drawPixel(x + j, y + i, TFT_BACKGROUND); 
      }
  }
}
void fetchPumpState() {
  if ((prevWaterLevel <= 1.0) && (autoOn))
  {
    pumpOn = true;
    digitalWrite(PUMP_PIN, pumpOn ? HIGH : LOW);
    Serial.printf("Pump state updated: %s\n", pumpOn ? "ON" : "OFF");
    Firebase.setBool("/controls/pump", pumpOn);
  }
  else if ((prevWaterLevel > 2.5) && (autoOn))
  {
    pumpOn = false;
    digitalWrite(PUMP_PIN, pumpOn ? HIGH : LOW);
    Serial.printf("Pump state updated: %s\n", pumpOn ? "ON" : "OFF");
    Firebase.setBool("/controls/pump", pumpOn);
  }
  else if (!autoOn)
  {
    bool pumpState = Firebase.getBool("/controls/pump");
    if (pumpOn != pumpState) {
        pumpOn = pumpState;
        digitalWrite(PUMP_PIN, pumpOn ? HIGH : LOW);
        Serial.printf("Pump state updated: %s\n", pumpOn ? "ON" : "OFF");
    }
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

void readAutoPump()
{
  tft.fillRect(54, 15 + 52 * 5, 225, 20, TFT_BACKGROUND);
  tft.setTextColor(DEFAULT_TXT_COLOR);
  tft.setCursor(54, 15 + 52 * 5);
  tft.print("Auto PUMP: ");
  tft.setTextColor(DETAIL_TXT_COLOR);
  tft.println(autoOn ? "ON" : "OFF");
}
void updateSensorValue(const char* path, float newValue, float& prevValue, const char* label, int x, int y) {
    //if (!isnan(newValue) && prevValue != newValue) {
      //if (Firebase.setFloat(fbdo, path, newValue)) 
      //if (Firebase.setFloat(path, newValue)) 
      //{
      tft.fillRect(x, y, 225, 20, TFT_BACKGROUND);
      tft.setTextColor(DEFAULT_TXT_COLOR);
      tft.setCursor(x, y);
      tft.print(label);
      tft.setTextColor(DETAIL_TXT_COLOR);
      tft.println(newValue);
      prevValue = newValue;
      //}
    //}
}

void updateBoolSensorValue(const char* path, bool newValue, bool& prevValue, const char* label, int x, int y) {
    //if (prevValue != newValue) {
      //if (Firebase.setBool(path, newValue)) 
      //{
      tft.setTextColor(DEFAULT_TXT_COLOR);
      tft.fillRect(x, y, 225, 20, TFT_BACKGROUND);
      tft.setCursor(x, y);
      tft.print(label);
      tft.setTextColor(DETAIL_TXT_COLOR);
      tft.println(newValue ? "True" : "False");
      prevValue = newValue;
      //} 
    /*} 
    else {
      tft.setTextColor(DEFAULT_TXT_COLOR);
      tft.fillRect(x, y, 225, 20, TFT_BACKGROUND);
      tft.setCursor(x, y);
      tft.print(label);
      tft.setTextColor(DETAIL_TXT_COLOR);
      tft.println(prevValue ? "True" : "False");
    }*/
}
void fetchSensor_Task(void *pvParameters)
{
  while (true)
  {
    //if (touchDetected)
    //{
      //Serial.println("Touch detected, performing control tasks...");
      //controlDevicesFromTouch();;
    //}
    if (!touchDetected)
    {
      Serial.print("Task fetchSensor_Task is running on core: ");
      Serial.println(xPortGetCoreID());
      Serial.println("readAutoPUMP...");
      readAutoPump();
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

    }
    vTaskDelay(3000 / portTICK_PERIOD_MS); 
  }
}
void drawWifiIcon()
{
  bool wifiState = (WiFi.status() == WL_CONNECTED);
  if (wifiState != prev_wifiState)
  {
    tft.fillRect(479 - 26, 0, 26, 26, TFT_BACKGROUND);
    if (wifiState)
    {
      
      drawIcon(479 - 26, 0, 26, 26, wifi_icon);
    }
    else
    {
      drawIcon(479 - 26, 0, 26, 26, nowifi_icon);
    }
  }
  prev_wifiState = wifiState;
}

void  fetchFirebase_Task(void *pvParameters)
{
  while (1)
  { 
    //Serial.print("Task fetchFirebase_Task is running on core: ");
    //Serial.println(xPortGetCoreID());
    if (millis() - lastCheckWifi > checkWifiInterval) {
      drawWifiIcon();
      lastCheckWifi = millis();
    }
    /*Serial.print("Wifi State: ");
    Serial.println(prev_wifiState);
    Serial.print("Touch State: ");
    Serial.println(touchDetected);   */
    /*if (touchDetected)
    {
      controlDevicesFromTouch();
      touchDetected = false;
    } */
    if (!touchDetected && prev_wifiState)
    {  

      if (!pumpStateChanging)
      {
        Serial.println("fetchPumpState...");
        fetchPumpState();
      }
      else
      {
        Serial.println("Push PUMP State...");
        //readAutoPump();
        Firebase.setBool("/controls/auto_PUMP", autoOn);
        fetchPumpState();
        delay(10);
        pumpStateChanging = false;
      }

      if (!lightStateChanging)
      {
        Serial.println("fetchLightState...");
        fetchLightState();
      }
      else
      {
        Serial.println("Push LightState...");
        Firebase.setBool("/controls/light", lightOn);
        delay(10);
        lightStateChanging = false;
      }
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
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
void checkTouch_Task(void *pvParameters) {
  while (true) {
    uint8_t maxChars = 10;
    ts.read();  

    if (ts.isTouched && !touchProcessed) {
      unsigned long currentTime = millis();
      if (currentTime - lastTouchTime > DEBOUNCE_TIME) {
        lastTouchTime = currentTime;
        touchDetected = true;
        touchProcessed = true;
        Serial.println("Touched");
        cor_x = map(ts.points[0].x, min_x, 479, 0, width - 1);
        cor_y = map(ts.points[0].y, min_y, 319, 0, height - 1);
        Serial.print("Touched: (");
        Serial.print(cor_x);
        Serial.print(", ");
        Serial.print(cor_y);
        Serial.println(")");
        if (isIconTouched(cor_x, cor_y, 0, 52 * 4 + 1, 52, 52))
        {
          Serial.println("Entering controlLedFromTouch");
          //lightStateChanging = true;
          controlLedFromTouch();
        }
        else if (isIconTouched(cor_x, cor_y, 0, 52*5+1, 52, 52))
        {
          Serial.println("Entering controlPumpFromTouch");
          //pumpStateChanging = true;
          controlPumpFromTouch();
        }
        else
        {
          Serial.println("Invalid Position!!!");
        }

        touchDetected = false;
      }
    }
    if (!ts.isTouched)
    {
      touchProcessed = false;
    }
    /*if (lightStateChanging)
      {
        Serial.println("Push LightState...");
        Firebase.setBool("/controls/light", lightOn);
        delay(100);
        lightStateChanging = false;
      }*/
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
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  drawIcon((480-67)/2, (320-86)/2, 86, 67, logo_icon);
  delay(1500);
  tft.fillScreen(TFT_BLACK);
  delay(300);
  tft.setTextSize(1);
  //pH
  tft.setTextColor(TFT_WHITE);
  tft.print("Initializing Touch...");
  touch_init(tft.width(), tft.height(), tft.getRotation());
  delay(100);
  tft.println("Done!");
  tft.println();
  tft.print("Initializing pHSensor...");
  pHSensor.begin(); 
  delay(100);
  tft.println("Done!");
  tft.println();
  //Light Sensor
  tft.print("Initializing lightSensor...");
  lightSensor.begin();
  delay(100);
  tft.println("Done!");
  tft.println();
  tft.print("Initializing DHT...");
  dht.begin();
  delay(100);
  tft.println("Done!");
  tft.println();
  tft.print("Initializing WIFI...");
  WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
  Serial.print("Connecting to WiFi...");
  uint8_t TryCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    TryCount++;
    delay(1000);
    tft.print(".");
    if ( TryCount == 15 )
    {
      tft.println();
      tft.println();
      tft.print("Failed to Connect to WIFI: ");
      tft.println(WIFI_SSID);
      tft.println();
      tft.println("Please check your WIFI connection! ");
      tft.println();
      tft.println("Tap the right half of the screen to skip, the left half to restart the ESP32");
      Serial.print(".");
      while(1)
      {
        ts.read();
        if (ts.isTouched)
        {
          cor_x = map(ts.points[0].x, min_x, 479, 0, width - 1);
          cor_y = map(ts.points[0].x, min_y, 319, 0, height - 1);
          if (cor_x < 480/2)
          {
            ESP.restart();
          }
          else
          {
            tft.println();
            tft.println("Ignoring... ");
            delay(200);
            break;
          }
        }
      }
      break;
    }

  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    tft.println();
    tft.println();
    tft.print("Connected with IP: ");
    tft.println(WiFi.localIP());
  }
  tft.println();
  tft.print("Setting Up IO...");
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  delay(100);
  tft.println("Done!");
  tft.println();
  delay(100);
  tft.print("Initializing the Graphic...");
  delay(2000);
  /*fetchPumpState();
  fetchLightState();
  readTemperature();
  readHumidity();
  readWaterLevel();
  readPHValue();
  readLightDetect();
  readAutoPump();*/
  tft.fillScreen(TFT_BACKGROUND);
  delay(1000);
  tft.setTextColor(DEFAULT_TXT_COLOR);
  tft.setTextSize(1); // Đặt kích thước chữ
  drawIcon(0, 0, 52, 52, Temperature_icon);
  tft.setCursor(54, 15);
  tft.print("Temp: --.--");
  drawIcon(0, 53, 52, 52, Humid_icon);
  tft.setCursor(54, 15 + 52);
  tft.print("Humidity: --.--"); 
  drawIcon(0, 52*2 + 1, 52, 52, Water_level_icon);
  tft.setCursor(54, 15 + 52*2);
  tft.print("Water Level: -.--");
  drawIcon(0, 52 * 3 + 1, 52, 52, pH_icon);
  tft.setCursor(54, 15 + 52*3);
  tft.print("pH: --.--");
  drawIcon(0, 52 * 4 + 1, 52, 52, light_on_icon);
  tft.setCursor(54, 15 + 52*4);
  tft.print("Light Detect: -----");
  drawIcon(0, 52 * 5 + 1, 52, 52, pump_icon);
  tft.setCursor(54, 15 + 52*5);
  tft.print("Auto PUMP: -----");
  drawIcon(479 - 26, 0, 26, 26, nowifi_icon);
  delay(2000);
  BaseType_t result1 = xTaskCreatePinnedToCore(
    fetchSensor_Task,        // Hàm thực hiện task
    "fetchSensorState",           // Tên task
    40000,                    // Kích thước stack (words)
    NULL,                     // Tham số truyền vào
    1,                        // Độ ưu tiên của task
    &xfetchSensorTaskHandle,      // Task handle
    0                        // Chạy trên core 1
  );
  if (result1 == pdPASS) {
    Serial.println("Task 1 created successfully.");
  } else {
    Serial.println("Task 1 creation failed.");
  }
  delay(10);
  BaseType_t result2 = xTaskCreatePinnedToCore(
    fetchFirebase_Task,        // Hàm thực hiện task
    "fetchState",           // Tên task
    40000,                    // Kích thước stack (words)
    NULL,                     // Tham số truyền vào
    1,                        // Độ ưu tiên của task
    &xfetchStateTaskHandle,      // Task handle
    1                      // Chạy trên core 0
  );
  if (result2 == pdPASS) {
    Serial.println("Task 2 created successfully.");
  } else {
    Serial.println("Task 2 creation failed.");
  }
  BaseType_t result3 = xTaskCreate(checkTouch_Task, "checkTouchTask", 5120, NULL, 1, NULL);
  if (result3 == pdPASS) {
    Serial.println("Task 3 created successfully.");
  } else {
    Serial.println("Task 3 creation failed.");
  }
  delay(10);
  
}

  //Firebase.setReadTimeout(fbdo, 2000);
  
  //attachInterrupt(digitalPinToInterrupt(TOUCH_FT6336_INT), touchInterrupt, FALLING);


/////////////////////////////////////////////////////////////////////////////////
//         Hàm ngắt I2C/I2C Interrupt function Section                         //  
/////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//                Hàm xử lý toàn cục/Processing Section                       //  
////////////////////////////////////////////////////////////////////////////////




void loop() {
  vTaskDelete(NULL);
}

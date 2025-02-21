/*******************************************************************************************************************************
****************************************************MỘT SỐ LƯU Ý****************************************************************
*******Sơ đồ chân:                                                                                                            **
**TFT Pin         CTP_INT    CTP_SDA   CTP_RTS   CTP_SCL   SDO(MISO)   LED   SCK   SDI(MOSI)   LCD_RS/DC    LCD_RST   LCD_CS  **
**GPIO               X        22        19        0          12       X     14       13           2         27        15      **
**Sensor: Light Sensor: Analog Pin:  26  |pH Sensor:  39 (VN)  | DHT11: 4      | ACS1: 32      |ACS3: 33      |LIGHT_PIN: 16  **
**                      Digital Pin: 5   |Water Sensor:  35  | PUMP_PIN: 17  | ACS2: 34      |MIST_PIN: 23                    **
********Mục lục mã nguồn:                                                                                                     **
**Phần tiền xử lý                                                                                                             ** 
**Phần khai báo module                                                                                                        **
**Phần khai báo biến                                                                                                          **
**Hàm phụ toàn cục                                                                                                            **
**Hàm thiết lập                                                                                                               **
**Phần khởi tạo task                                                                                                          **
**Hàm xử lý toàn cục                                                                                                          **
********************************************************************************************************************************
*******************************************************************************************************************************/





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
#include "ACS712_ESP32.h"
#include "ArduinoJson.h"
#include "esp_task_wdt.h"


#define WDT_TIMEOUT 300000  


esp_task_wdt_config_t twdt_config = {
    .timeout_ms = WDT_TIMEOUT,  
    .trigger_panic = true,  
};
// Firebase config
#define DATABASE_URL "smartgarden-86bab-default-rtdb.firebaseio.com"
#define DATABASE_SECRET "BOj3P8OdeNNEPdyPZ8g4sTu0A4N9QUWjgnDKX6FR"
#define API_KEY "AIzaSyCcmsIiBMejWivNKMthf5kWbzLKeTKJ2sw"


#define DHTPIN                  4
#define DHTTYPE               DHT11
#define PH_SENSOR_PIN           39
#define WATER_LEVEL_SENSOR_PIN  35
#define WATER_MAXIMUM_HEIGHT    4.0
#define PUMP_PIN                17
#define MIST_PIN                23
#define ACS_PUMP                32
#define ACS_MIST                34
#define ACS_REGULAR             33
#define LIGHT_ANALOG_PIN        26
#define LIGHT_DIGITAL_PIN        5
#define LIGHT_THRES             500
#define LIGHT_PIN               16
#define TIMER_FREQ              2000
#define TFT_BACKGROUND          0x0F1B
#define WHITE                   0xFFFF
#define DEFAULT_TXT_COLOR       0xC0D5
#define DETAIL_TXT_COLOR        0xFA24
#define DEBOUNCE_TIME             2000
////////////////////////////////////////////////////////////////////////////////
//                Phần khai báo module/Module Declaration Section             //    
////////////////////////////////////////////////////////////////////////////////

PHSensor pHSensor(PH_SENSOR_PIN); 
LightSensor lightSensor(LIGHT_ANALOG_PIN, LIGHT_DIGITAL_PIN, LIGHT_THRES);
WaterSensor waterSensor(WATER_LEVEL_SENSOR_PIN, WATER_MAXIMUM_HEIGHT);
DHT dht(DHTPIN, DHTTYPE);
/*CurrentSensor currentPUMP(ACS_PUMP, 1000.0, 2000.0, 3.3, 0, 0.066);
CurrentSensor currentMIST(ACS_MIST, 1000.0, 2000.0, 3.3, 0, 0.066);
CurrentSensor currentREGULAR(ACS_REGULAR, 1000.0, 2000.0, 3.3, 0, 0.066);*/
ACS712 currentPUMP(ACS_PUMP, 3.3, 4095, 0.066);
ACS712 currentMIST(ACS_MIST, 3.3, 4095, 0.066);
ACS712 currentREGULAR(ACS_REGULAR, 3.3, 4095, 0.066);
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
bool lightOnChange = false;
bool mistSprayOnChange = false;
bool pumpOnChange = false;
bool prevLightDetect = false;
bool isCollectingData = false;
uint8_t processState = 0;
uint8_t processFirebase = 0;
bool isCollectingDataFromFB = false;
volatile unsigned long lastTouchInterruptTime = 0;
volatile bool interruptFlag = false;
bool isControlling = false;
bool touchOK = false;
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
bool mistSprayOn = false;
bool autoLightStateChange = false;
bool autoPumpStateChange = false;
bool autoMistStateChange = false;
bool prevLightOn = false, prevMistSprayOn = false, prevPumpOn = false;
volatile bool touchDetected = false;
bool prevAutoPumpOn = false;
bool prevAutoMistSprayOn = false;
bool prevAutoLightOn = false;
bool autoPumpOn = false;
bool autoMistSprayOn = false;
bool autoLightOn = false;
bool prev_wifiState = false;
bool isTouching = false;
unsigned long lastCheckWifi = 0;
unsigned long checkWifiInterval = 5000;
unsigned long lastWifiCheck = 0;
bool showSSID = true;
bool manualPumpControlHandling = false;
bool manualMistSprayControlHandling = false;
bool manualLightControlHandling = false;
float currentPumpValue = 0.0;
float currentMistValue = 0.0;
float currentLedValue = 0.0;
float prevCurrentPumpValue = 0.0;
float prevCurrentMistValue = 0.0;
float prevcurrentLedValue = 0.0;
////////////////////////////////////////////////////////////////////////////////
//           Task Functions Replaced with Timer-Based Functions               //
////////////////////////////////////////////////////////////////////////////////


bool isIconTouched(int touchX, int touchY, int iconX, int iconY, int iconW, int iconH) {
  //vTaskDelay(50 / portTICK_PERIOD_MS);
  return (touchX >= iconX && touchX <= (iconX + iconW) &&
          touchY >= iconY && touchY <= (iconY + iconH));
}

/*bool isCircleTouched(int x_touch, int y_touch, int x_center, int y_center, int radius) {
  int dx = x_touch - x_center;
  int dy = y_touch - y_center;
  return (dx * dx + dy * dy <= radius * radius);  
}*/

void controlLedFromTouch()
{
  autoLightStateChange = true;
  autoLightOn = !autoLightOn;
  prevAutoLightOn = autoLightOn;
  //Firebase.setBool("/controls/auto_LIGHT", autoLightOn);
  readAutoLight();
  touchDetected = false;
  
}
void controlPumpFromTouch()
{
  autoPumpStateChange = true;
  autoPumpOn = !autoPumpOn;
  prevAutoPumpOn = autoPumpOn;
  readAutoPump();
  touchDetected = false;
}

void controlMistSprayFromTouch()
{
  autoMistStateChange = true;
  autoMistSprayOn = !autoMistSprayOn;
  prevAutoMistSprayOn = autoMistSprayOn;
  readAutoMist();
  touchDetected = false;
}

void drawIcon(int x, int y, int height, int width, const uint16_t *icon) {
  for (int i = 0; i < height; i++) {
    const uint16_t *row = icon + i * width;
    for (int j = 0; j < width; j++) {
      if (row[j] != WHITE) { 
        tft.drawPixel(x + j, y + i, row[j]);
      }
      
    }
    //vTaskDelay(1 / portTICK_PERIOD_MS);
  }
  

}


void clearIcon(int x, int y, int height, int width) {
  for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        tft.drawPixel(x + j, y + i, TFT_BACKGROUND); 
      }
      //vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}



void pumpStateInterface()
{
  drawIcon(90, 180, 45, 45, pumpOn ? switch_on_icon : switch_off_icon);
  delay(5);
  drawIcon(90, 130, 45, 45, autoPumpOn ? switch_on_icon : switch_off_icon);
}

void manualPumpControlHandlingFunct()
{
  Serial.println("Manual Pump");
  autoPumpOn = false;
  manualPumpControlHandling = true;
  pumpOn = !pumpOn;
  prevPumpOn = pumpOn;
  digitalWrite(PUMP_PIN, pumpOn ? LOW : HIGH);
  pumpStateInterface();
}

void mistStateInterface()
{
  drawIcon(240, 180, 45, 45, mistSprayOn ? switch_on_icon : switch_off_icon);
  delay(5);
  drawIcon(240, 130, 45, 45, autoMistSprayOn ? switch_on_icon : switch_off_icon);
  
  
}
void manualMistSprayControlHandlingFunct()
{
  autoMistSprayOn = false;
  manualMistSprayControlHandling = true;
  mistSprayOn = !mistSprayOn;
  mistSprayOnChange = true;
  prevMistSprayOn = mistSprayOn;
  digitalWrite(MIST_PIN, mistSprayOn ? LOW : HIGH);
  mistStateInterface();
}

void lightStateInterface()
{
  drawIcon(390, 180, 45, 45, lightOn ? switch_on_icon : switch_off_icon);
  delay(5);
  drawIcon(390, 130, 45, 45, autoLightOn ? switch_on_icon : switch_off_icon);

  
}

void readACS()
{
  //currentPumpValue = currentPUMP.readCurrent();
  currentPumpValue = (pumpOn ? currentPUMP.readCurrent() : 0);
  //currentPumpValue = analogRead(ACS_PUMP);
  updateSensorValue(currentPumpValue, prevCurrentPumpValue, "May bom: ", 250, 270);
  //currentLedValue = currentREGULAR.readCurrent();
  currentLedValue = (lightOn ? currentREGULAR.readCurrent() : 0);
  //currentLedValue = analogRead(ACS_MIST);
  updateSensorValue(currentLedValue, prevcurrentLedValue, "LED: ", 250, 290);
  //currentMistValue = currentMIST.readCurrent();
  currentMistValue = (mistSprayOn ? currentMIST.readCurrent() : 0);
  //currentMistValue = analogRead(ACS_MIST);
  updateSensorValue(currentMistValue, prevCurrentMistValue, "Phun suong: ", 250, 310);

}

void manualLightControlHandlingFunct()
{
  
  //Firebase.setBool("/controls/auto_LIGHT", autoLightOn);
  manualLightControlHandling = true;
  lightOn = !lightOn;
  lightOnChange = true;
  prevLightOn = lightOn;
  digitalWrite(LIGHT_PIN, lightOn ? LOW : HIGH);
  lightStateInterface();
}


void readTemperature() {
    temperature = dht.readTemperature();
    if (!isnan(temperature)) {
        updateSensorValue(temperature, prevTemperature, "Nhiet do (Cel): ", 10, 10);
    }
}

void readHumidity() {
    humidity = dht.readHumidity();
    if (!isnan(humidity)) {
        updateSensorValue(humidity, prevHumidity, "Do am: ", 10, 30);
    }
    if (autoMistSprayOn)
    {
      if ((humidity <= 60))
      {
        mistSprayOn = true;
        digitalWrite(MIST_PIN, LOW);
        mistStateInterface();
      }
      else if ((humidity >= 80))
      {
        mistSprayOn = false;
        digitalWrite(MIST_PIN, HIGH);
        mistStateInterface();
      }
      prevMistSprayOn = mistSprayOn;
      readACS();
      esp_task_wdt_reset();
    }
}

void readWaterLevel() {
    waterLevel = waterSensor.getWaterLevelCm(); 
    updateSensorValue(waterLevel, prevWaterLevel, "Muc nuoc (cm): ", 160, 10);
    if (autoPumpOn)
    {
      if ((prevWaterLevel <= 1.0))
      {
        Serial.println("Water Level under 1cm");
        pumpOn = true;
        digitalWrite(PUMP_PIN, LOW);
        pumpStateInterface();
      }
      else if ((prevWaterLevel >= 2.3))
      {
        Serial.println("Water Level above 2.3cm");
        pumpOn = false;
        digitalWrite(PUMP_PIN, HIGH);
        pumpStateInterface(); 
        esp_task_wdt_reset();
      }
      prevPumpOn = pumpOn;
      readACS();
    }

}

void readPHValue() {
  pHValue = pHSensor.readPH(); 
  updateSensorValue(pHValue, prevPHValue, "pH: ", 160, 30);
}

void readLightDetect() {
    light_detect = lightSensor.isLightDetected();
    updateBoolSensorValue(light_detect, prevLightDetect, "Anh sang: ", 310, 10);
    if (autoLightOn)
    {
      lightOn = light_detect ? false : true;
      lightOnChange = true;
      prevLightOn = lightOn;
      digitalWrite(LIGHT_PIN, lightOn ? LOW : HIGH);
      lightStateInterface();
      esp_task_wdt_reset();
      //readACS();
    }
}

void readAutoPump()
{
  delay(5);
  drawIcon(90, 130, 45, 45, autoPumpOn ? switch_on_icon : switch_off_icon);
}

void readAutoMist()
{
  delay(5);
  drawIcon(240, 130, 45, 45, autoMistSprayOn ? switch_on_icon : switch_off_icon);
}

void readAutoLight()
{
  delay(5);
  drawIcon(390, 130, 45, 45, autoLightOn ? switch_on_icon : switch_off_icon);
}
String createControlJson() {
  String jsonData = "{";
  jsonData += "\"auto_LIGHT\":" + String(autoLightOn ? "true" : "false") + ",";
  jsonData += "\"auto_MIST\":" + String(autoMistSprayOn ? "true" : "false") + ",";
  jsonData += "\"auto_PUMP\":" + String(autoPumpOn ? "true" : "false") + ",";
  jsonData += "\"light\":" + String(lightOn ? "true" : "false") + ",";
  jsonData += "\"mistSpray\":" + String(mistSprayOn ? "true" : "false") + ",";
  jsonData += "\"pump\":" + String(pumpOn ? "true" : "false");
  jsonData += "}";
  return jsonData;
}
String createSensorJson() {
  String jsonData = "{";
  jsonData += "\"humidity\":" + String(prevHumidity) + ",";
  jsonData += "\"ledConsumption\":" + String(currentLedValue) + ",";
  jsonData += "\"lightDetect\":" + String(light_detect) + ",";
  jsonData += "\"mistConsumption\":" + String(currentMistValue) + ",";
  jsonData += "\"pumpConsumption\":" + String(currentPumpValue) + ",";
  jsonData += "\"temperature\":" + String(prevTemperature) + ",";
  jsonData += "\"phValue\":" + String(pHValue) + ",";
  jsonData += "\"waterLevel\":" + String(prevWaterLevel);
  jsonData += "}";
  return jsonData;
}

void updateSensorValue(float newValue, float& prevValue, const char* label, int x, int y) {
  tft.fillRect(x, y, 150, 20, TFT_BACKGROUND);
  tft.setTextColor(DEFAULT_TXT_COLOR);
  tft.setCursor(x, y);
  tft.print(label);
  tft.setTextColor(DETAIL_TXT_COLOR);
  tft.println(newValue);
  prevValue = newValue;
}

void updateBoolSensorValue(bool newValue, bool& prevValue, const char* label, int x, int y) {
    tft.setTextColor(DEFAULT_TXT_COLOR);
    tft.fillRect(x, y, 100, 20, TFT_BACKGROUND);
    tft.setCursor(x, y);
    tft.print(label);
    tft.setTextColor(DETAIL_TXT_COLOR);
    tft.println(newValue ? "True" : "False");
    prevValue = newValue;
}

void updateStringValue(String newValue, String label, int x, int y) {
  tft.fillRect(x, y, 180, 20, TFT_BACKGROUND);
  tft.setTextColor(DEFAULT_TXT_COLOR);
  tft.setCursor(x, y);
  tft.print(label);
  tft.setTextColor(DETAIL_TXT_COLOR);
  tft.println(newValue);
}

void fetchSensor_Task(void *pvParameters)
{
  delay(1000);
  
  while (true)
  {
    if (!touchDetected)
    {
      readACS();
      esp_task_wdt_reset();
      //vTaskDelay(50);
      readWaterLevel();
      readLightDetect();
      //vTaskDelay(50 / portTICK_PERIOD_MS);
      //WiFi.disconnect(true);
      readTemperature();
      
      readHumidity();
      //vTaskDelay(50 / portTICK_PERIOD_MS);
      readPHValue();
      esp_task_wdt_reset();
      //WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS); 
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

void fetchFirebase_Task(void *pvParameters) {
  delay(2000);            
  while (1) {
    esp_task_wdt_reset();
    if (millis() - lastCheckWifi > checkWifiInterval) {
      drawWifiIcon();
      lastCheckWifi = millis();
    }

    if (prev_wifiState) {
      if (touchOK) {
        String jsonData = createControlJson();
        int result = Firebase.setJson("/controls", jsonData);
        esp_task_wdt_reset();
        if (result == 200) {
          Serial.println("Control states updated successfully.");
        } else {
          Serial.print("Failed to update control states. Error code: ");
          Serial.println(result);
        }
        touchOK = false; 
      } else {
       
        String json = Firebase.getJson("/controls");
        esp_task_wdt_reset(); 

        if (json.length() > 0) {
          const size_t capacity = JSON_OBJECT_SIZE(10) + 200; 
          DynamicJsonDocument doc(capacity);
          DeserializationError error = deserializeJson(doc, json);
          
          if (!error) {
          
            autoPumpOn = doc["auto_PUMP"];
            autoLightOn = doc["auto_LIGHT"];
            autoMistSprayOn = doc["auto_MIST"];
            lightOn = doc["light"];
            mistSprayOn = doc["mistSpray"];
            pumpOn = doc["pump"];

       
            if (lightOn != prevLightOn) {
              digitalWrite(LIGHT_PIN, lightOn ? LOW : HIGH);
              Serial.printf("Light state updated: %s\n", lightOn ? "ON" : "OFF");
              drawIcon(390, 180, 45, 45, lightOn ? switch_on_icon : switch_off_icon);
              esp_task_wdt_reset();
              drawIcon(390, 130, 45, 45, autoLightOn ? switch_on_icon : switch_off_icon);
              esp_task_wdt_reset();
              prevLightOn = lightOn;
              readACS();
              
            }
            if (touchOK)
            {
              continue;
            }
            if (mistSprayOn != prevMistSprayOn) {
              digitalWrite(MIST_PIN, mistSprayOn ? LOW : HIGH);
              Serial.printf("Mist Spray state updated: %s\n", mistSprayOn ? "ON" : "OFF");
              drawIcon(240, 180, 45, 45, mistSprayOn ? switch_on_icon : switch_off_icon);
              esp_task_wdt_reset();
              drawIcon(240, 130, 45, 45, autoMistSprayOn ? switch_on_icon : switch_off_icon);
              esp_task_wdt_reset();
              prevMistSprayOn = mistSprayOn;
              readACS();
            }
            if (touchOK)
            {
              continue;
            }
            if (pumpOn != prevPumpOn) {
              digitalWrite(PUMP_PIN, pumpOn ? LOW : HIGH);
              Serial.printf("Pump state updated: %s\n", pumpOn ? "ON" : "OFF");
              drawIcon(90, 180, 45, 45, pumpOn ? switch_on_icon : switch_off_icon);
              esp_task_wdt_reset();
              drawIcon(90, 130, 45, 45, autoPumpOn ? switch_on_icon : switch_off_icon);
              esp_task_wdt_reset();
              prevPumpOn = pumpOn;
              readACS();
            }

            if (touchOK)
            {
              continue;
            }
            if (autoPumpOn != prevAutoPumpOn) 
            {
              readAutoPump();
              prevAutoPumpOn = autoPumpOn;
            }
            if (touchOK)
            {
              continue;
            }
            if (autoLightOn != prevAutoLightOn) 
            {
              readAutoLight();
              prevAutoLightOn = autoLightOn;
            }
            if (touchOK)
            {
              continue;
            }
            if (autoMistSprayOn != prevAutoMistSprayOn) 
            {
              readAutoMist();
              prevAutoMistSprayOn = autoMistSprayOn;
            }
            if (touchOK)
            {
              continue;
            }
          } else {
            Serial.print(F("JSON deserialization failed: "));
            Serial.println(error.c_str());
          }
        } else {
          Serial.println("Failed to get JSON from Firebase.");
        }
      }

      String jsonSensorData = createSensorJson();
      int sensor_result = Firebase.setJson("/sensors", jsonSensorData);
      esp_task_wdt_reset();
      if (sensor_result == 200) {
        Serial.println("Sensors states updated successfully.");
      } else {
        Serial.print("Failed to update sensors states. Error code: ");
        Serial.println(sensor_result);
      }
      String forecast_json = Firebase.getJson("/forecast");
      esp_task_wdt_reset(); 
      if (forecast_json.length() > 0) {
        const size_t forecast_capacity = JSON_OBJECT_SIZE(10) + 200; 
        DynamicJsonDocument forecast_doc(forecast_capacity);
        DeserializationError error = deserializeJson(forecast_doc, forecast_json);
        
        if (!error) {
        
        updateStringValue(forecast_doc["time"], "Thoi gian: ", 10, 270);
        updateStringValue(forecast_doc["temp_humid"], "Nhiet do/Do Am: ", 10, 290);
        updateStringValue(forecast_doc["weather"], "Thoi tiet: ", 10, 310);
        }
      }

    }

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void checkTouch_Task(void *pvParameters) {
  delay(1000);
  while (true) {
    ts.read();  

    if (ts.isTouched && !touchProcessed) {
      unsigned long currentTime = millis();
      if (currentTime - lastTouchTime > DEBOUNCE_TIME) {
        //vTaskSuspend(xfetchStateTaskHandle);
        lastTouchTime = currentTime;
        isTouching = true;
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
        if (isIconTouched(cor_x, cor_y, 90, 130, 45, 45))
        {          
          touchOK = true;
          //readAutoPump();
          Serial.println("Entering auto control pump");
          //autoPumpStateChange = true;
          controlPumpFromTouch();
          
        }
        else if (isIconTouched(cor_x, cor_y, 90, 180, 45, 45))
        {
          touchOK = true;
          //pumpOnChange = true;
          autoPumpOn = false;
          Serial.println(("Control pump!"));
          manualPumpControlHandlingFunct();
          //autoPumpStateChange = true;
        }
        else if (isIconTouched(cor_x, cor_y, 240, 130, 45, 45))
        {
          touchOK = true;
          Serial.println("Manual mist spray");
          controlMistSprayFromTouch();
        }
        else if (isIconTouched(cor_x, cor_y, 240, 180, 45, 45))
        {
          touchOK = true;
          //mistSprayOnChange = true;
          autoMistSprayOn = false;
          Serial.println("Control mist spray");
          manualMistSprayControlHandlingFunct();
        }
        else if (isIconTouched(cor_x, cor_y, 390, 130, 45, 45))
        {
          touchOK = true;
          Serial.println("Control Light");
          controlLedFromTouch();
        }
        else if (isIconTouched(cor_x, cor_y, 390, 180, 45, 45))
        {
          touchOK = true;
          //lightOnChange = true;
          autoLightOn = false;
          Serial.println("Manual light control");
          manualLightControlHandlingFunct();
        }
        else
        {
          Serial.println("Invalid Position!!!");
        }
        isTouching = false;
        touchProcessed = false;
        //readACS();
        //vTaskResume(xfetchStateTaskHandle);
      }
    }
    if (!ts.isTouched)
    {
      touchDetected = false;
      
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);  
  }
}

void drawHeader() {
  // temp and humid
  tft.setCursor(10, 10);
  tft.print("Nhiet do: 25 C");
  tft.setCursor(10, 30);
  tft.print("Do am: 60%");

  //Water Level
  tft.setCursor(160, 10);
  tft.print("Muc nuoc: 80%");

  // Light
  tft.setCursor(160, 30);
  tft.print("Anh sang: 400 Lux");

  // pH
  tft.setCursor(310, 10);
  tft.print("pH: 6.5");

  tft.drawLine(0, 70, 480, 70, TFT_WHITE);
}

void drawControls() {
  // pump
  drawIcon(5, 90, 30, 30, faucet);
  tft.setCursor(45, 100);
  tft.print("May Bom:");
  tft.setCursor(10, 150);
  tft.print("Auto: ");
  drawIcon(90, 130, 45, 45, switch_off_icon);
  tft.setCursor(10, 200);
  tft.print("Trang thai: ");
  drawIcon(90, 180, 45, 45, switch_off_icon);
  // mist
  drawIcon(155, 90, 30, 30, mistSpray);
  tft.setCursor(195, 100);
  tft.print("Phun Suong:");
  tft.setCursor(160, 150);
  tft.print("Auto: ");
  drawIcon(240, 130, 45, 45, switch_off_icon);
  tft.setCursor(160, 200);
  tft.print("Trang thai: ");
  drawIcon(240, 180, 45, 45, switch_off_icon);
  // light
  drawIcon(305, 90, 30, 30, light);
  tft.setCursor(355, 100);
  tft.print("Den:");
  tft.setCursor(310, 150);
  tft.print("Auto: ");
  drawIcon(390, 130, 45, 45, switch_off_icon);
  tft.setCursor(310, 200);
  tft.print("Trang thai: ");
  drawIcon(390, 180, 45, 45, switch_off_icon);
  tft.drawLine(0, 240, 480, 240, TFT_WHITE);
}

void drawFooter() {
  // weather forecast
  tft.setCursor(10, 250);
  tft.print("Thoi tiet ngay mai:");
  drawIcon(200, 250, 35, 35, weather);
  tft.drawLine(240, 240, 240, 320, TFT_WHITE);
  tft.setCursor(250, 250);
  tft.print("Dong dien: ");
  tft.setCursor(250, 270);
  tft.print("May bom: ");
  tft.setCursor(250, 290);
  tft.print("LED: ");
  tft.setCursor(250, 310);
  tft.print("Phun suong: ");
  drawIcon(400, 270, 45, 45, current);
}
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
    if ( TryCount == 5 )
    {
      tft.println();
      tft.println();
      tft.print("Failed to Connect to WIFI: ");
      tft.println(WIFI_SSID);
      tft.println();
      tft.println("Please check your WIFI connection! ");
      tft.println();
      /*tft.println("Tap the right half of the screen to skip, the left half to restart the ESP32");
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
      }*/
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
  tft.print("Getting data from Firebase...");
  String time ="";
  String temp_humid = "";
  String weather = "";
  if (WiFi.status() == WL_CONNECTED)
  { 
    String json = Firebase.getJson("/controls");
    //vTaskDelay(50 / portTICK_PERIOD_MS);
    //Serial.println(json)
    if (json.length() > 0) {
      const size_t capacity = JSON_OBJECT_SIZE(10) + 200; 
      DynamicJsonDocument doc(capacity);
      DeserializationError error = deserializeJson(doc, json);
      if (error) {
        Serial.print(F("JSON deserialization failed: "));
        Serial.println(error.c_str());
      } else {
        autoPumpOn = doc["auto_PUMP"];
        prevAutoPumpOn = autoPumpOn;
        autoLightOn = doc["auto_LIGHT"];
        prevAutoLightOn = autoLightOn;
        autoMistSprayOn = doc["auto_MIST"];
        prevAutoMistSprayOn = autoMistSprayOn;
        lightOn = doc["light"];
        prevLightOn = lightOn;
        mistSprayOn = doc["mistSpray"];
        prevMistSprayOn = mistSprayOn;
        pumpOn = doc["pump"];
        prevPumpOn = pumpOn;
        String forecast_json = Firebase.getJson("/forecast");
          if (forecast_json.length() > 0) {
          const size_t forecast_capacity = JSON_OBJECT_SIZE(10) + 200; 
          DynamicJsonDocument forecast_doc(forecast_capacity);
          DeserializationError error = deserializeJson(forecast_doc, forecast_json);
          time = forecast_doc["time"].as<String>();
          temp_humid = forecast_doc["temp_humid"].as<String>();
          weather = forecast_doc["weather"].as<String>();
        }
      }
    }
  }
  tft.println("Done!");
  tft.println();
  tft.print("Calibrating Pump motor...");
  currentPUMP.calibrateOffset();
  tft.println("Done!");
  tft.println();
  tft.print("Calibrating Mist motor...");
  currentMIST.calibrateOffset();
  tft.println("Done!");
  tft.println();
  tft.print("Calibrating LED...");
  currentREGULAR.calibrateOffset();
  tft.println("Done!");
  tft.println();
  tft.println();
  delay(100);
  tft.print("Setting Up IO...");
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, HIGH);
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, HIGH);
  pinMode(MIST_PIN, OUTPUT);
  digitalWrite(MIST_PIN, HIGH);
  delay(100);
  tft.println("Done!");
  tft.println();
  tft.print("Initializing the Graphic...");
  delay(1000);
  tft.fillScreen(TFT_BACKGROUND);
  delay(500);
  tft.setTextColor(DEFAULT_TXT_COLOR);
  drawHeader();
  drawControls();
  drawFooter();
  drawIcon(479 - 26, 0, 26, 26, nowifi_icon);
  delay(1000);
  mistStateInterface();
  readAutoPump();
  readAutoLight();
  readAutoMist();
  lightStateInterface();
  pumpStateInterface();
  updateStringValue(time, "Thoi gian: ", 10, 270);
  updateStringValue(temp_humid, "Nhiet do/Do Am: ", 10, 290);
  updateStringValue(weather, "Thoi tiet: ", 10, 310);
  drawWifiIcon();
  digitalWrite(PUMP_PIN, pumpOn ? LOW : HIGH);
  digitalWrite(LIGHT_PIN, lightOn ? LOW : HIGH);
  digitalWrite(MIST_PIN, mistSprayOn ? LOW : HIGH);
  delay(500);
  readACS();
  esp_task_wdt_deinit();
  esp_task_wdt_init(&twdt_config); 
  delay(100);
  BaseType_t result1 = xTaskCreatePinnedToCore(
    fetchSensor_Task,       
    "fetchSensorState",          
    5000,                    
    NULL,                    
    1,                       
    &xfetchSensorTaskHandle,     
    1                       
  );
  esp_task_wdt_add(xfetchSensorTaskHandle);
  if (result1 == pdPASS) {
    Serial.println("Task 1 created successfully.");
  } else {
    Serial.println("Task 1 creation failed.");
  }
  delay(10);
  BaseType_t result2 = xTaskCreatePinnedToCore(
    fetchFirebase_Task,        
    "fetchState",          
    60000,                   
    NULL,                   
    2,                        
    &xfetchStateTaskHandle,      
    0                      
  );
  esp_task_wdt_add(xfetchStateTaskHandle);
  if (result2 == pdPASS) {
    Serial.println("Task 2 created successfully.");
  } else {
    Serial.println("Task 2 creation failed.");
  }
  BaseType_t result3 = xTaskCreate(checkTouch_Task, "checkTouchTask", 20000, NULL, 1, NULL);
  if (result3 == pdPASS) {
    Serial.println("Task 3 created successfully.");
  } else {
    Serial.println("Task 3 creation failed.");
  }
  delay(10);
}





void loop() {
  vTaskDelete(NULL);
}

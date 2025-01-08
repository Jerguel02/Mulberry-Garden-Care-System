# Mulberry Garden Care System

## Table of Contents
1. [Introduction](#introduction)
2. [System Requirements](#system-requirements)
3. [Project Structure](#project-structure)
4. [Installation and Code Execution Guide](#installation-and-code-execution-guide)
5. [Configuration](#configuration)

---

## 1. Introduction

### Project Description
The Mulberry Garden Care System is designed to automate the care of mulberry gardens, minimizing traditional manual processes. This system provides:
- Remote monitoring of garden conditions.
- Automatic execution of basic care functions.
- Continuous data exchange via MCU and Webserver Cloud.
- Automatic and manual control of sensors and modules through a touchscreen interface.

### Technologies Used
- **Programming Languages**: C/C++
- **Hardware**: ESP32, Motor, DHT11, PH-4602C, LDR, LED, 4" TFT, ACS712
- **Programming Methods**: OOP, RTOS
- **Cloud Technology**: Google Firebase

### Authors
- Phạm Hữu Nghĩa (Lead Author)

**Contact Information:**
- Email: huunghia250102@gmail.com
- Phone: 0817819589

---

## 2. System Requirements

### Hardware Requirements
- ESP32
- Motor
- DHT11
- PH-4602C
- LDR
- LED
- 4" TFT
- ACS712

### Software Requirements
- **Required Software**:
  - Arduino IDE
  - EspTool
  - Sublime Text
- **Required Libraries**:
  - TFT_eSPI.h
  - Firebase.h
  - DHT.h
  - WiFi.h
  - LightSensor.h*
  - WaterSensor.h*
  - my_images.h*
  - pHSensor.h*
  - ACS712_ESP32.h*
  - ArduinoJSON.h
  - esp_task_wdt.h

*Note: Libraries marked with * are custom and included in the project.

---

## 3. Project Structure

```
Mulberry-Garden-Care-System/
├── doc/
│   ├── TaiLieu.docx
│   └── TaiLieu.pdf
├── gui/
│   ├── Icon/
│   │   └── *.png
│   └── Icon_Hex/
│       └── *.txt
├── lib/
│   ├── ACS712_ESP32/
│   │   ├── ACS712_ESP32.h
│   │   └── ACS712_ESP32.c
│   ├── LightSensor/
│   │   ├── LightSensor.h
│   │   └── LightSensor.c
│   ├── my_images/
│   │   └── my_images.h
│   ├── pHSensor/
│   │   ├── pHSensor.h
│   │   └── pHSensor.c
│   ├── touch/
│   │   └── touch.h
│   └── Water_Sensor/
│       ├── Water_Sensor.h
│       └── Water_Sensor.c
├── src/
│   ├── main/
│   │   └── main.ino
│   └── web/
│       ├── img/
│       │   └── *.png
│       ├── firebase.js
│       ├── index.html
│       ├── script.js
│       ├── styles.css
│       └── Web_React.code-workspace
├── test/
├── tool/
│   └── img2hex/
│       ├── build/
│       ├── dist/
│       ├── Test/
│       ├── img2hex.exe
│       ├── img2hex.py
│       └── img2hex.spec
└── LICENSE
```

### Key Directories

- **src/** - Main source code
  - `main/`: Contains `main.ino` which implements core logic, including sensor initialization, data handling, and control.
- **lib/** - Custom libraries
  - `ACS712_ESP32/`: Current sensor library
  - `LightSensor/`: Light sensor library
  - `pHSensor/`: pH sensor library
  - `Water_Sensor/`: Water level sensor library
- **doc/** - Documentation files
- **gui/** - Graphical resources for the TFT display
- **tool/** - Utility tools for development
- **LICENSE** - Project license (MIT License)

---

## 4. Installation and Code Execution Guide

### Steps to Set Up and Run
1. Clone the repository:
   ```bash
   git clone https://github.com/Jerguel02/Mulberry-Garden-Care-System.git
   ```
2. Extract the libraries from `Mulberry-Garden-Care-System/lib` and place them in the Arduino IDE's library directory or alongside `main.ino`.
3. Install remaining libraries via Arduino IDE or download them manually from GitHub.
4. Upload the code to ESP32 and start the system.

---

## 5. Configuration

### Hardware Configuration
Connect the hardware as follows:

| Component             | ESP32 Pin        |
|-----------------------|------------------|
| CTP_SDA              | G22              |
| CTP_RTS              | G19              |
| CTP_SCL              | G0               |
| MISO                 | G12              |
| MOSI                 | G13              |
| SCK                  | G14              |
| LCD SCL              | G22              |
| LCD_RS               | G2               |
| LCD_RST              | G27              |
| LCD_CS               | G15              |
| LDR DIGITAL PIN      | G5               |
| PH SENSOR            | G39 (VN)         |
| WATER SENSOR         | G35              |
| DHT11                | G4               |
| PUMP PIN             | G17              |
| MIST SPRAY PIN       | G23              |
| LED PIN              | G16              |
| ACS PUMP             | G32              |
| ACS MIST SPRAY       | G34              |
| ACS LED              | G33              |

### Software Configuration
Modify WiFi and Google Firebase settings in `main.ino`:

```cpp
#define WIFI_SSID "YourSSID"
#define WIFI_PASSWORD "YourPassword"
#define DATABASE_URL "YourDatabaseURL"
```




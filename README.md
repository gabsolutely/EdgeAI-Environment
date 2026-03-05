# EAE - Edge AI Environment — Overview & Usage // ######################
----------------------------------
An ESP32-based smart environment monitoring and automation system with:
 - AI-driven decision making
 - Mathematical + statistical fallbacks
 - Cloud logging (Railway / Node.js)
 - Local web dashboard (STA / AP)
 - CSV data storage (SPIFFS)
> This project is a next-generation evolution of ESPSmart-Environment, expanding into cloud, AI inference, anomaly detection, activity recognizer, and prediction.

## Table Of Contents // ############################################
 - Hardware Overview
 - System Architecture
 - Features
 - Project Structure
 - Firmware Setup
 - Cloud Setup (Railway)
 - AI Model Workflow
 - Web Dashboard Endpoints
 - Automation Logic Summary
 - System Status
 - Usage
 - Troubleshooting
 - Possible Upgrades
 - Notes & Additional Information

## Hardware Overview // ############################################
 ### Sensors:
 - DHT11 — Temperature & Humidity
 - LDR (ADC) — Light intensity (**Inverted Sensor**)
 - Ultrasonic Sensor — Motion / presence detection
 ### Outputs:
 - LED1 — Light automation
 - LED2 — Motion indicator
 - Buzzer / LED3 — Temperature Alerts
 - RGB LED:
  (RED) Temperature
  (GREEN) Motion
  (BLUE) Light

## System Architecture // ##########################################
### Decision Flow:
```
 Sensors
   v
 Averaging + Filtering
   v
 AIEngine (PRIMARY decision path)
   v
 IOManager (LEDs / Buzzer / RGB)
```

### Fallback Path (activated on AI failure / disable):
 - Predictor (trend forecasting)
 - AnomalyDetector (statistical deviation)
 - ActivityRecognizer (motion smoothing & noise rejection)
> AI is the primary decision maker. If AI fails, is disabled, or outputs invalid data -> fallback logic takes over.  Manual override always has highest priority.

## Features // #####################################################
 ### Monitoring:
 - Live temperature, humidity, light, motion
 - Timestamps on each reading
 - Rolling averages
 - Anomaly detection
 - Future value prediction

 ### Automation:
 - AI based action selection
 - Math/statistics based fallback
 - LED visualization for actions
 - RGB LED indicates anomalies and predicted thresholds (can be changed to reflect system state)
 - Manual override via HTTP

 ### Logging:
 - CSV logging to SPIFFS (/data.csv)
 - Automatic recovery on corruption
 - Downloadable CSV from dashboard

 ### Cloud:
 - HTTPS POST events
 - Non-blocking FreeRTOS task
 - Heartbeat + sensor payloads
 - Railway compatible Node.js backend

 ### Dashboard:
 - Real-time sensor updates (readings and averages)
 - Action status
 - Manual overrides
 - Toggle AI/Math modes
 - CSV download
 - Chart.js data visualization
 - Recent events board
 - Runs fully on ESP32 (SPIFFS)

## Project Structure // ############################################
```bash
EAE - Edge AI Environment/        # Some are IDE/library artifacts
├── AI_model/
│   ├── CreateData.py
│   ├── TrainModel.ipynb
│   ├── constraints_patch.py
│   ├── converter.py
│   ├── model.pkl
│   ├── README_AI.md
│   └── Model_Explanation.md
├── cloud/
│   ├── README_cloud.md
│   ├── docs_cloud.md
│   └── api_test.rest
├── data/
│   ├── dashboard/
│   │   ├── dashboard.html
│   │   ├── dashboard.js
│   │   └── dashboard.css
│   ├── data.csv
│   ├── events.json
│   └── training_data.csv     # Place in AI_model if training
├── EAEAPI/              # Railway Node.js backend
│   ├── server.js
│   ├── package.json
│   └── package-lock.json
├── include/
│   ├── AIEngine.h
│   ├── Automation.h
│   ├── CloudManager.h
│   ├── IOManager.h
│   ├── Logger.h
│   ├── Predictor.h
│   ├── SensorManager.h
│   ├── AnomalyDetector.h
│   ├── ActivityRecognizer.h
│   └── config.h
├── node_modules/
├── src/
│   ├── main.cpp
│   ├── AIEngine.cpp
│   ├── Automation.cpp
│   ├── CloudManager.cpp
│   ├── IOManager.cpp
│   ├── Logger.cpp
│   ├── SensorManager.cpp
│   ├── Predictor.cpp
│   ├── AnomalyDetector.cpp
│   ├── ActivityRecognizer.cpp
│   ├── model.c
│   └── model.h
├── platformio.ini
└── README.md
```

## Firmware Setup // #############################################
### Requirements:
 - PlatformIO (recommended) or Arduino IDE (might take more setup tweaks)
 - ESP32 board package (Sensors, Outputs, etc)
### Libraries:
 - DHT sensor
 - SPIFFS
 - WiFi / WebServer
 - HTTPClient
### Filesystem:
 - Upload Filesystem Image - from PlatformIO
### Configuration:
 ```
 #define WIFI_SSID        "your_wifi"
 #define WIFI_PASSWORD    "your_password"

 #define CLOUD_ENDPOINT   "https://your-cloud-provider/api/event"
 ```
> **AI retraining is required for configuration**
 **Fine-tune config.h for your actual environment/usage.**

## Cloud Setup (Railway) // ######################################
 #### Please refer to `cloud/README_cloud.md`
 #### Quick Steps:
 - Sign up at https://railway.app
 - Create Node.js project
 #### Install CLI:
 - npm install -g railway
 #### Deploy:
 - `cd EAEAPI`
 - `railway login`
 - `railway link`
 - `railway up`
> Copy your API endpoint into config.h.

## AI Model Workflow // ###########################################
 #### Please refer to `AI_model/README_AI.md`
 #### Only required if generating a NEW model
 #### Generate data:
 - python CreateData.py
 #### Train model:
 - Open TrainModel.ipynb (VSCode / Colab)
 - Save model as .pkl
 #### Convert to C:
 - python converter.py
> model.c and model.h are automatically included in /src.

## Web Dashboard Endpoints // ####################################
```
`/`                          - Dashboard
`/data`	                     - Live JSON data
`/csv`	                     - Download CSV
`/override?value=X&dur=MS`   - Manual override
`/ai?enable=0/1`	         - Enable / disable AI
```
> Runs locally from SPIFFS, no internet required.

## Automation Logic Summary // ###################################
### AI Actions:
```
 0 - No action
 1 - LED1 (Light)
 2 - LED2 (Motion)
 3 - LED3/Buzzer (Temp)
```
### Fallback Rules:
```
 Low light            -> LED1
 Motion               -> LED2
 High temp -> LED3/Buzzer
```
> Anomalies & predictions reinforce decisions

## System Status // ##############################################
 - Fully functional without AI
 - AI inference-first architecture
 - Cloud + local operation
 - TFLite Micro intentionally avoided (compatibility issues)
 - Operationally stable under extended runtime testing (30+ minutes)

## Usage // ######################################################
 - The system collects data from various sensors and triggers actions based on predefined thresholds or AI model inference.
 - The system will automatically log sensor readings and cloud events.
 - A web dashboard is provided for real-time visualization, CSV data download, manual override, and toggle AI/Math.

## Troubleshooting // ############################################
 - Memory Issues: Double-check AI models as they're the most likely to crash the ESP32.
 - SPIFFS Issues: If the ESP32 fails to mount SPIFFS or create files, ensure that the file system is properly formatted.
 - Wi-Fi Connection: Double-check your Wi-Fi credentials in config.h.
 - CSV Logging: If data is not being logged correctly, check for permissions in SPIFFS and ensure there's enough storage available.
 - Dashboard/data Issues: Try reuploading file system images (likely a SPIFFS issue).
> **Contact information is available on GitHub, my DMs and comments are open! (TIKTOK, YOUTUBE, GITHUB)**

## Possible Upgrades // ##########################################
 - More advanced AI models
 - Cloud-side inference
 - More refined cloud (MQTT, WEBSOCKET, SECURITY, Dashboard Integration etc)
 - OTA updates
 - Multi-device aggregation
 - More advanced sensors/outputs (motion sensor is noisy)
 - Physical buttons (not implemented due to hardware constraints)

## Notes & Additional Information // #############################
 - Due to sensor noise and multi-source data conflicts, LED3/Buzzer control is currently driven only by temperature, rather than a combined temperature–humidity rule.
 - The RGB LED operates exclusively under the fallback system and does not reflect AI decisions.
 - The ActivityRecognizer introduces a slight detection delay compared to the AI-based activity logic, caused by its sliding-window design and conservative noise filtering.
 - Earlier versions experienced memory constraint issues with AI models leading to occasional crashes on the ESP32. These have been fixed with a custom patch.

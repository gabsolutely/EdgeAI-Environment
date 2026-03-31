// config.h

#pragma once

// WIFI STTINGS -------------------
#define WIFI_SSID        "yourWifiName"           // your WiFi SSID
#define WIFI_PASSWORD    "yourPassword"  // your WiFi password
#define FALLBACK_AP_SSID "EAE-Setup"         // fallback AP SSID
#define FALLBACK_AP_PASS "eaepass"           // fallback AP password

// CLOUD / API SETTINGS -----------
#define CLOUD_ENDPOINT   "https://nodejs-production-41f4.up.railway.app/api/event" // e.g. ".../api/event"
// NOTE* for reference, "https://nodejs-production-41f4.up.railway.app/api/event" is this projects deployed backend

#define FRAME_INTERVAL   500     // main loop interval in ms
#define CLOUD_INTERVAL   1000    // send heartbeat every second

// SENSORS SETTINGS ---------------
// DHT sensor
#define DHT_PIN          4       // replace with actual pin number
#define DHT_TYPE         11      // 11/12, etc.

// Light sensor (LDR via ADC)
#define LIGHT_PIN        34      // ADC pin

// Ultrasonic Distance Sensor
#define OBS_PIN          35      // obstacle sensor pin

// OUTPUT DEVICE PINS -------------
#define LED_PIN          15      // main LED
#define LED2_PIN         2       // second LED
#define BUZZ_PIN         19      // LED3/buzzer
#define RGB_R_PIN        21      // temp
#define RGB_G_PIN        22      // humidity
#define RGB_B_PIN        23      // light

// AUTOMATION RULES SETTINGS -------
// High light threshold for LED1 auto turn-on
#define LIGHT_THRESHOLD  3100    // e.g. 3000

// Temp/Humidity threshold for LED3/BUZZ
#define TEMP_THRESHOLD   35      // e.g. 33.0
#define HUM_THRESHOLD    81      // e.g. 80.0

// AI/FALLBACK ENGINE SETTINGS --------------
// Use AI by default
extern bool USE_AI;              // configure default AIEngine.cpp

// Manual override
#define OverrideDuration  10000  // 10 seconds

// anomaly threshold multiplier (z-score/statistical)
#define ANOMALY_Z_LIMIT    3.0f

// Prediction horizon (samples)
#define TEMP_PREDICT_AHEAD  3
#define HUM_PREDICT_AHEAD   3
#define LIGHT_PREDICT_AHEAD 2    // how far to forecast/steps ahead

// Prediction limits (absolute)
#define TEMP_PREDICT_LIMIT 35.0
#define HUM_PREDICT_LIMIT  81.0
#define LIGHT_PREDICT_MIN  3100

// CSV LOGGING SETTINGS -------------
#define CSV_PATH         "/data.csv"    // path to CSV file
#define CLEAR_CSV_ON_BOOT  true         // clear CSV file on boot
#define CSV_FILE_LIMIT     500000       // max CSV file size to clear
#define EPOCH_OFFSET    1766934881UL    // Unix time for 2025-12-28 15:00:00 (as an example), just incase RTC fails
// NOTE* (optional) adjust EPOCH_OFFSET to your current time before first use
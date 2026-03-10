// main.cpp

#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

#include "SensorManager.h"
#include "Automation.h"
#include "AIEngine.h"
#include "Logger.h"
#include "CloudManager.h"
#include "IOManager.h"
#include "AnomalyDetector.h"
#include "Predictor.h"
#include "config.h" 
#include <Arduino.h>

unsigned long lastFrame = 0;
unsigned long lastCloud = 0;
unsigned long lastWiFiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 5000; // check every 5 seconds

WebServer server(80);

float temp = 0, hum = 0, avgTemp = 0, avgHum = 0;
int light = 0, avgLight = 0, motion = 0, action = 0;
bool apModeActive = false;

// dashboard handlers
void handleRoot() {
  if (!SPIFFS.exists("/dashboard/dashboard.html")) {
    server.send(404, "text/plain", "dashboard.html not found");
    return;
  }
  File f = SPIFFS.open("/dashboard/dashboard.html", "r");
  server.streamFile(f, "text/html");
  f.close();
}

void handleDataJS() {
  if (!SPIFFS.exists("/dashboard/dashboard.js")) {
    server.send(404, "text/plain", "dashboard.js not found");
    return;
  }
  File f = SPIFFS.open("/dashboard/dashboard.js", "r");
  server.streamFile(f, "application/javascript");
  f.close();
}

void handleDataJSON() {
    char payload[512];
    Logger::buildPayload(temp, avgTemp, hum, avgHum, light, avgLight, motion, action, payload, sizeof(payload));
    server.send(200, "application/json", payload);
}

void handleCSS() {
  if (!SPIFFS.exists("/dashboard/dashboard.css")) {
    server.send(404, "text/plain", "dashboard.css not found");
    return;
  }
  File f = SPIFFS.open("/dashboard/dashboard.css", "r");
  server.streamFile(f, "text/css");
  f.close();
}

void handleCSV() {
  if(!SPIFFS.exists(CSV_PATH)) {
    server.send(404, "text/plain", "no csv");
    return;
  }
  File f = SPIFFS.open(CSV_PATH, "r");
  server.streamFile(f, "text/csv");
  f.close();
}

void handleOverride() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing value arg");
    return;
  }
  int v = server.arg("value").toInt();
  int val = v;
  switch (v) {
    case 1: val = ACTION_LED1; break;
    case 2: val = ACTION_LED2; break;
    case 3: val = ACTION_BUZZ; break;
  }

  unsigned long durationMs = OverrideDuration;
  if (server.hasArg("dur")) durationMs = (unsigned long)server.arg("dur").toInt();
  AIEngine::setManualOverride(val, durationMs);
  server.send(200, "text/plain", "OK");
}

void handleSetAI() {
  if (!server.hasArg("enable")) {
    server.send(400, "text/plain", "Missing enable arg");
    return;
  }
  int e = server.arg("enable").toInt();
  USE_AI = (e == 1);
  server.send(200, "text/plain", USE_AI ? "AI enabled" : "AI disabled");
}

void handleNotFound() {
  String msg = "Not found\n";
  msg += "URI: " + server.uri();
  server.send(404, "text/plain", msg);
}

// NTP time synchronization
void handleTime() {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    time_t now = 0;
    unsigned long start = millis();
    const unsigned long timeout = 10000;
    while (now < 1700000000 && (millis() - start) < timeout) {
        delay(200);
        time(&now);
    }
    if (now >= 1700000000) {
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);

        char timeString[30];
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);

        Serial.print("NTP time synchronized: ");
        Serial.println(timeString);
    } else {
        Serial.println("NTP sync timed out.");
    }
}

// non-blocking WiFi watchdog
void checkWiFi() {
  unsigned long now = millis();
  if (now - lastWiFiCheck < WIFI_CHECK_INTERVAL) return;
  lastWiFiCheck = now;

  if (WiFi.status() != WL_CONNECTED && !apModeActive) {
    Serial.println("WiFi lost, switching to AP mode...");
    WiFi.softAP(FALLBACK_AP_SSID, FALLBACK_AP_PASS);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    apModeActive = true;
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("STARTING..");

  /* SPIFFS FORMATTING (uncomment to use, optional)
  Serial.println("Formatting SPIFFS to clear corruption...");
  if (SPIFFS.format()) {
      Serial.println("SPIFFS formatted successfully.");
  } else {
      Serial.println("SPIFFS format failed!");
  }
  */

  IOManager::init();
  CloudManager::init();
  Automation::init();
  AIEngine::init();
  AnomalyDetector::init(30);
  Predictor::init(20);
  Logger::init();

  Serial.println("EAE initialized");

  // connect to WiFi
  //Serial.printf("Connecting to WiFi %s ...", WIFI_SSID);
  Serial.print("Connecting to WiFi ");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000UL) {
    delay(250);
    Serial.print('.');
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
    handleTime();
  } else {
    Serial.println("\nWiFi failed, starting AP mode...");
    WiFi.softAP(FALLBACK_AP_SSID, FALLBACK_AP_PASS);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    apModeActive = true;
  }

  // webservers
  server.on("/", handleRoot);
  server.on("/dashboard.html", handleRoot);
  server.on("/dashboard.js", handleDataJS);
  server.on("/data", handleDataJSON);  
  server.on("/dashboard.css", handleCSS);
  server.on("/csv", handleCSV);
  server.on("/override", handleOverride);
  server.on("/ai", handleSetAI);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Web server started");

  // print network/cloud info
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Cloud endpoint: ");
  Serial.println(CLOUD_ENDPOINT);
}

void loop() {
  server.handleClient();
  unsigned long now = millis();
  checkWiFi();

  // system loop
  if (now - lastFrame >= FRAME_INTERVAL) {
      lastFrame = now;

      temp = SensorManager::readTemp();
      hum = SensorManager::readHumidity();
      light = SensorManager::readLight();
      motion = SensorManager::readMotion()? 1 : 0;

      SensorManager::averageReadings(temp, hum, light);
      avgTemp = SensorManager::readAverageTemp();
      avgHum = SensorManager::readAverageHumidity();
      avgLight = SensorManager::readAverageLight();

      action = Automation::evaluate(temp, hum, light, motion, avgTemp, avgHum, avgLight);
  }

  // cloud loop
  if (now - lastCloud >= CLOUD_INTERVAL) {
    lastCloud = now;

    CloudManager::sendHeartbeat();

    char payload[512];
    Logger::buildPayload(temp, avgTemp, hum, avgHum, light, avgLight, motion, action, payload, sizeof(payload));
    Logger::append(temp, avgTemp, hum, avgHum, light, avgLight, motion, action);

    Serial.print("Sending payload: ");
    Serial.println(payload);

    if (strlen(CLOUD_ENDPOINT) > 0) {
      CloudManager::sendEvent(String(payload));
    }
  }
  // optional other tasks could add OTA update, extra sensor checks, etc etc.
}

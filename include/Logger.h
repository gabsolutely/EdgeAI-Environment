// include/Logger.h

#pragma once
#include <Arduino.h>

class Logger {
public:
  static void init();
  static void clearCSV();
  static void append(float temp, float avgTemp, float hum, float avgHum, int light, int avgLight, int motion, int action);
  static void buildPayload( float temp, float avgTemp, float hum, float avgHum, int light, int avgLight, int motion, int action, char* out, size_t maxLen);

  static char payload[128];
};
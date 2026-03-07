// include/Automation.h

#pragma once

class Automation {
public:
  static void init();
  static void handleRGB(float temp, float hum, int light, int motion);
  static int evaluate(float temp, float hum, int light, int motion, float avgTemp, float avgHum, int avgLight);
};
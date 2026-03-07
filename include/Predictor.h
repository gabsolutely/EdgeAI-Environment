// include/Predictor.h

#pragma once

class Predictor {
public:
  static void init(int window = 20);
  static float predictTemp(float currentTemp, int stepsAhead = 1);
  static float predictHum(float currentHum, int stepsAhead = 1);
  static int predictLight(int currentLight, int stepsAhead = 1);
};
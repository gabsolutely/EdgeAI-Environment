// include/SensorManager.h

#pragma once
#include <DHT.h>

class SensorManager {
public:
    static void init();
    static float readTemp();
    static float readHumidity();
    static int readLight();
    static int readMotion();

    static void averageReadings(float temp, float hum, int light);
    static float readAverageTemp();
    static float readAverageHumidity();
    static float readAverageLight();
    static void resetAverages();

private:
    static const int BUFFER_SIZE = 10;
    static float tempBuf[BUFFER_SIZE];
    static float humBuf[BUFFER_SIZE];
    static int lightBuf[BUFFER_SIZE];
    static int index;
    static bool bufferFilled;
};
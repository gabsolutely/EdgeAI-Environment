// SensorManager.cpp

#include "SensorManager.h"
#include <DHT.h>
#include "config.h"
#include <Arduino.h>

DHT dht(DHT_PIN, DHT_TYPE);

float SensorManager::tempBuf[BUFFER_SIZE];
float SensorManager::humBuf[BUFFER_SIZE];
int SensorManager::lightBuf[BUFFER_SIZE];
int SensorManager::index = 0;
bool SensorManager::bufferFilled = false;

void SensorManager::init() {
    dht.begin();
    pinMode(LIGHT_PIN, INPUT);
    pinMode(OBS_PIN, INPUT);
    resetAverages();
}

// read sensor functions
float SensorManager::readTemp() {
    float t = dht.readTemperature();
    return isnan(t) ? NAN : t;
}

float SensorManager::readHumidity() {
    float h = dht.readHumidity();
    return isnan(h) ? NAN : h;
}

int SensorManager::readLight() {
    return analogRead(LIGHT_PIN);
}

int SensorManager::readMotion() {
    return digitalRead(OBS_PIN) == HIGH ? 0 : 1;
}

// averaging functions
void SensorManager::averageReadings(float temp, float hum, int light) {
    tempBuf[index]  = temp;
    humBuf[index]   = hum;
    lightBuf[index] = light;

    index = (index + 1) % BUFFER_SIZE;
    if (index == 0) bufferFilled = true;
}

float SensorManager::readAverageTemp() {
    int limit = bufferFilled ? BUFFER_SIZE : index;
    float sum = 0.0f;
    int valid = 0;

    for (int i = 0; i < limit; i++) {
        if (!isnan(tempBuf[i])) {
            sum += tempBuf[i];
            valid++;
        }
    }
    return valid ? sum / valid : 0.0f;
}

float SensorManager::readAverageHumidity() {
    int limit = bufferFilled ? BUFFER_SIZE : index;
    float sum = 0.0f;
    int valid = 0;

    for (int i = 0; i < limit; i++) {
        if (!isnan(humBuf[i])) {
            sum += humBuf[i];
            valid++;
        }
    }
    return valid ? sum / valid : 0.0f;
}

float SensorManager::readAverageLight() {
    int limit = bufferFilled ? BUFFER_SIZE : index;
    if (limit == 0) return 0.0f;

    float sum = 0.0f;
    for (int i = 0; i < limit; i++) {
        sum += lightBuf[i];
    }
    return sum / limit;
}

void SensorManager::resetAverages() {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tempBuf[i] = NAN;
        humBuf[i]  = NAN;
        lightBuf[i] = 0;
    }
    index = 0;
    bufferFilled = false;
}

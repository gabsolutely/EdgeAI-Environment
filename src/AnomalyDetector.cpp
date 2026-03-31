// AnomalyDetector.cpp

#include "AnomalyDetector.h"
#include "config.h"
#include <Arduino.h>
#include <math.h>

// declare buffers and variables
static const int MAX_BUF = 60;
static int N = 30;
static float tBuf[MAX_BUF];
static float hBuf[MAX_BUF];
static float lBuf[MAX_BUF];

static int idx = 0;
static bool full = false;

void AnomalyDetector::init(int bufLen) {
    // reset buffers
    N = (bufLen > 4 && bufLen < MAX_BUF) ? bufLen : 10;
    idx = 0;
    full = false;

    for (int i=0; i<N; i++) {
        tBuf[i] = 0;
        hBuf[i] = 0;
        lBuf[i] = 0;
    }
}

// add new readings to buffers
void AnomalyDetector::update(float t, float h, int l) {
    tBuf[idx] = t;
    hBuf[idx] = h;
    lBuf[idx] = l;

    idx++;
    if (idx >= N) {
        idx = 0;
        full = true;
    }
}

// helper functions (for statistics calculations)
static float mean(const float *arr, int count) {
    float s = 0;
    for (int i=0; i<count; i++) s += arr[i];
    return s / count;
}

static float variance(const float *arr, int count, float m) {
    float v = 0;
    for (int i=0; i<count; i++) {
        float d = arr[i] - m;
        v += d*d;
    }
    return v / count;
}

float AnomalyDetector::tempDeviation(float temp) {
    int c = full ? N : idx;
    if (c < 6) return 0;

    float m = mean(tBuf, c);
    float var = variance(tBuf, c, m);
    float sd = sqrt(var);
    if (sd < 0.0001f) sd = 0.0001f;

    return (temp - m) / sd;
}

float AnomalyDetector::humDeviation(float hum) {
    int c = full ? N : idx;
    if (c < 6) return 0;

    float m = mean(hBuf, c);
    float var = variance(hBuf, c, m);
    float sd = sqrt(var);
    if (sd < 0.0001f) sd = 0.0001f;

    return (hum - m) / sd;
}

float AnomalyDetector::lightDeviation(int light) {
    int c = full ? N : idx;
    if (c < 6) return 0;

    float lf[MAX_BUF];
    for (int i=0; i<c; i++) lf[i] = lBuf[i];

    float m = mean(lf, c);
    float var = variance(lf, c, m);
    float sd = sqrt(var);
    if (sd < 1.0f) sd = 1.0f;

    return ((float)light - m) / sd;
}

// detection functions (fabs means absolute value)
// Z limits can be configured in config.h
bool AnomalyDetector::detectTemp(float temp) {
    return fabs(tempDeviation(temp)) > ANOMALY_Z_LIMIT;
}
bool AnomalyDetector::detectHum(float hum) {
    return fabs(humDeviation(hum)) > ANOMALY_Z_LIMIT;
}
bool AnomalyDetector::detectLight(int light) {
    return fabs(lightDeviation(light)) > ANOMALY_Z_LIMIT;
}

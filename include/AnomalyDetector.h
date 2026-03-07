// include/AnomalyDetector.h

#pragma once

class AnomalyDetector {
public:
    static void init(int bufLen);
    static void update(float temp, float hum, int light);

    static float tempDeviation(float temp);
    static float humDeviation(float hum);
    static float lightDeviation(int light);

    static bool detectTemp(float temp);
    static bool detectHum(float hum);
    static bool detectLight(int light);
};

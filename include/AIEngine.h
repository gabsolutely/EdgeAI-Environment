// include/AIEngine.h

#pragma once

class AIEngine {
public:
    static void init();
    static void setManualOverride(int action, unsigned long durationMs);
    static int decideAction(float temp, float hum, int light, int motion,
                            float predTemp, float predHum, int predLight,
                            float anomalyTemp, float anomalyHum, float anomalyLight);
};

// action bitmasks
#define ACTION_NONE 0x00
#define ACTION_LED1 (1 << 0)  // 0x01
#define ACTION_LED2 (1 << 1)  // 0x02
#define ACTION_BUZZ (1 << 2)  // 0x04
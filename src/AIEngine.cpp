// AIEngine.cpp

#include "AIEngine.h"
#include "AnomalyDetector.h"
#include "Predictor.h"
#include "ActivityRecognizer.h"
#include "model.h"
#include "config.h"
#include <Arduino.h>

bool USE_AI = true; // configure as default, can be changed at runtime

extern int runAIModel(
    float temp, float hum, int light, int motion,
    float predTemp, float predHum, int predLight,
    float anomalyTemp, float anomalyHum, float anomalyLight
);

static int manualOverride = -1;
static unsigned long manualExpiry = 0;
static ActivityRecognizer activity(10);

void AIEngine::init() {
    manualOverride = -1;
    manualExpiry = 0;
}

void AIEngine::setManualOverride(int action, unsigned long durationMs) {
    if (action == -1) manualOverride = -1;
    // if (action == 0) action = ACTION_NONE;
    if (action == 1) action = ACTION_LED1;
    if (action == 2) action = ACTION_LED2;
    if (action == 3) action = ACTION_BUZZ;

    manualOverride = action;
    manualExpiry = millis() + durationMs;
}

int AIEngine::decideAction(float temp, float hum, int light, int motion,
                           float predTemp, float predHum, int predLight,
                           float anomalyTemp, float anomalyHum, float anomalyLight) {
    int action = ACTION_NONE;

    // check for manual override
    if (manualOverride >= 0) {
        if ((long)(millis() - manualExpiry) <= 0)
            return manualOverride;
        manualOverride = -1;
    }

    // determine activity
    bool active = (activity.detect(motion) == ActivityRecognizer::ACTIVITY);

    // fallback output
    int fallback = ACTION_NONE;
    if (light > LIGHT_THRESHOLD ||
         Predictor::predictLight(light) > LIGHT_PREDICT_MIN)
        fallback |= ACTION_LED1;
    if (active)
        fallback |= ACTION_LED2;
    if (temp > TEMP_THRESHOLD ||
        Predictor::predictTemp(temp) > TEMP_PREDICT_LIMIT)
        fallback |= ACTION_BUZZ;

    // run AI model
    int aiMask = -1;
    if (USE_AI) {
        aiMask = runAIModel(
            temp, hum, light, motion,
            predTemp, predHum, predLight,
            anomalyTemp, anomalyHum, anomalyLight
        );
    }

    // run AI output
    if (!USE_AI || aiMask < 0) {
        action = fallback;
    } else {
        action = 0;
        if (aiMask & 0x01) action |= ACTION_LED1;
        if (aiMask & 0x02) action |= ACTION_LED2;
        if (aiMask & 0x04) action |= ACTION_BUZZ;
    }

    // apply safety masks if not using AI (fallback mode)
    if (!USE_AI || aiMask < 0) {
        if (!(temp > TEMP_THRESHOLD ||
              Predictor::predictTemp(temp) > TEMP_PREDICT_LIMIT))
            action &= ~ACTION_BUZZ;

        if (!active)
            action &= ~ACTION_LED2;
    }

    // debugging
    Serial.printf(
        "[AI] raw=%d | L1=%d L2=%d BZ=%d | final=%d\n",
        aiMask,
        !!(action & ACTION_LED1),
        !!(action & ACTION_LED2),
        !!(action & ACTION_BUZZ),
        action
    );

    return action;
}
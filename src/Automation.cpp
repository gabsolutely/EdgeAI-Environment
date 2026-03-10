// Automation.cpp

#include "Automation.h"
#include "AIEngine.h"
#include "IOManager.h"
#include "SensorManager.h"
#include "Logger.h"
#include "AnomalyDetector.h"
#include "ActivityRecognizer.h"
#include "Predictor.h"
#include "CloudManager.h"
#include "config.h"
#include <Arduino.h>

void Automation::init() {
    SensorManager::init();
}

// RGB color handling based on anomalies and predictions
void Automation::handleRGB(float temp, float hum, int light, int motion) {
    int r = (AnomalyDetector::detectTemp(temp) ? 255 : 0) +
            (Predictor::predictTemp(temp, TEMP_PREDICT_AHEAD) > TEMP_PREDICT_LIMIT ? 128 : 0);

    static ActivityRecognizer activity(10);
    int g = (activity.detect(motion) == ActivityRecognizer::ACTIVITY) ? 255 : 0;

    // optional hum version
    /* int g = (AnomalyDetector::detectHum(hum) ? 255 : 0) +
             (Predictor::predictHum(hum, HUM_PREDICT_AHEAD) > HUM_PREDICT_LIMIT ? 128 : 0); */

    int b = (AnomalyDetector::detectLight(light) ? 255 : 0) +
            (Predictor::predictLight(light, LIGHT_PREDICT_AHEAD) > LIGHT_PREDICT_MIN ? 128 : 0);

    IOManager::setRGBColor(constrain(r, 0, 255), /*constrain(g, 0, 255)*/ g, constrain(b, 0, 255));
}

int Automation::evaluate(float temp, float hum, int light, int motion,
                         float avgTemp, float avgHum, int avgLight) {

    // predictions/anomalies
    float predTemp = Predictor::predictTemp(temp, TEMP_PREDICT_AHEAD);
    float predHum  = Predictor::predictHum(hum, HUM_PREDICT_AHEAD);
    int predLight = Predictor::predictLight(light, LIGHT_PREDICT_AHEAD);

    float anomTemp  = AnomalyDetector::detectTemp(temp);
    float anomHum   = AnomalyDetector::detectHum(hum);
    float anomLight = AnomalyDetector::detectLight(light);

    // get action from AI engine
    int action = AIEngine::decideAction(
        temp, hum, light, motion,
        predTemp, predHum, predLight,
        anomTemp, anomHum, anomLight
    );

    // perform actions
    IOManager::setLed(action & ACTION_LED1);
    IOManager::setLed2(action & ACTION_LED2);
    IOManager::buzz(action & ACTION_BUZZ);

    handleRGB(temp, hum, light, motion);

    // debugging
    Serial.printf(
        "[Automation] action=%d T=%.2f avgT=%.2f H=%.2f avgH=%.2f L=%d avgL=%d M=%d\n",
        action, temp, avgTemp, hum, avgHum, light, avgLight, motion
    );

    return action;
}

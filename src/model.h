#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void score_led1(double * input, double * output);
void score_led2(double * input, double * output);
void score_buzz(double * input, double * output);

int runAIModel(
    float temp, float hum, int light, int motion,
    float predTemp, float predHum, int predLight,
    float anomalyTemp, float anomalyHum, float anomalyLight
);

#ifdef __cplusplus
}
#endif

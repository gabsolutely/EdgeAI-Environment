// ActivityRecognizer.cpp

#include "ActivityRecognizer.h"
#include <Arduino.h>

// constructor
ActivityRecognizer::ActivityRecognizer(int w) {
    window = w < 3 ? 3 : w;
    buf = (int*) malloc(sizeof(int) * window);
    for (int i = 0; i < window; i++) buf[i] = 0;
    idx = 0;
    filled = 0;
}

void ActivityRecognizer::push(int value) {
    buf[idx] = value ? 1 : 0;
    idx = (idx + 1) % window;
    if (filled < window) filled++;
}

int ActivityRecognizer::sum() const {
    int s = 0;
    for (int i = 0; i < filled; i++) s += buf[i];
    return s;
}

// detect activity based on motion input
ActivityRecognizer::Activity ActivityRecognizer::detect(int motion) {
    push(motion);

    if (filled < window)
        return NONE;

    int s = sum();

    // require at least half the window to be active
    if (s >= (window / 2))
        return ACTIVITY;

    return NONE;
}

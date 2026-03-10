// Logger.cpp

#include "Logger.h"
#include <SPIFFS.h>
#include <FS.h>
#include <time.h>
#include "config.h"

static time_t getEpoch() {
    time_t now;
    time(&now);
    if (now < 100000) { // NTP not synced
        now = millis() / 1000 + EPOCH_OFFSET; // adjust offset to a close default
    }
    return now;
}

void Logger::clearCSV() {
    if (SPIFFS.exists(CSV_PATH)) {
        SPIFFS.remove(CSV_PATH);
        Serial.println("[LOGGER] CSV cleared");
    }
}

void Logger::init() {
    // check SPIFFS mount
    if (!SPIFFS.begin(true)) {
        Serial.println("[LOGGER] SPIFFS mount failed");
        return;
    }

    if (!SPIFFS.exists("/data")) {
        SPIFFS.mkdir("/data");
    }

    // clear CSV on boot if configured
    if (CLEAR_CSV_ON_BOOT && SPIFFS.exists(CSV_PATH)) {
        Logger::clearCSV();
    }

    File f = SPIFFS.open(CSV_PATH, FILE_APPEND);
    if (!f) {
        Serial.println("[LOGGER] CSV open failed");
        return;
    }

    // write CSV header if file is empty
    if (f.size() == 0) {
        f.println("ts,temp,avgTemp,humidity,avgHumidity,light,avgLight,motion,action");
    }

    // close and list files
    f.close();
    Serial.println("[LOGGER] Ready");
    Serial.println("[LOGGER] SPIFFS File List: ");
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
    root.close();
}


void Logger::append(float temp, float avgTemp, float hum, float avgHum,
                    int light, int avgLight, int motion, int action) {

    File f = SPIFFS.open(CSV_PATH, FILE_APPEND);
    if (!f) {
        Serial.println("[LOGGER] Append failed");
        return;
    }

    if (f.size() > CSV_FILE_LIMIT) {
        Serial.println("[LOGGER] CSV file size limit exceeded, logging stopped");
        f.close();
        return;
    }

    time_t ts = getEpoch();
    struct tm timeinfo;
    localtime_r(&ts, &timeinfo);

    char timestr[9];
    strftime(timestr, sizeof(timestr), "%H:%M:%S", &timeinfo);

    f.printf(
        "%s,%.2f,%.2f,%.2f,%.2f,%d,%d,%d,%d\n",
        timestr,
        temp, avgTemp,
        hum, avgHum,
        light, avgLight,
        motion, action
    );

    f.flush();
    f.close();
}

void Logger::buildPayload(float temp, float avgTemp, float hum, float avgHum,
                          int light, int avgLight, int motion, int action, char* out, size_t maxLen) {
    // build JSON payload
    snprintf(
        out, maxLen,
        "{\"ts\":%lu,\"temp\":%.2f,\"avgTemp\":%.2f,"
        "\"humidity\":%.2f,\"avgHumidity\":%.2f,"
        "\"light\":%d,\"avgLight\":%d,"
        "\"motion\":%d,\"action\":%d}",
        getEpoch(),
        temp, avgTemp,
        hum, avgHum,
        light, avgLight,
        motion, action
    );
}
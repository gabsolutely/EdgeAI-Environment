// CloudManager.cpp

#include "CloudManager.h"
#include "config.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Arduino.h>

static WiFiClientSecure client;
static HTTPClient http;

// global states
static String pendingPayload = "";
static SemaphoreHandle_t lock;

// free RTOS task handle
static TaskHandle_t cloudTaskHandle = nullptr;

// state machine for cloud task
enum CloudState {
    CLOUD_IDLE,    // waiting for something to send
    CLOUD_BEGIN,   // preparing the HTTP client
    CLOUD_SEND,    // sending the payload
    CLOUD_WAIT,    // waiting for the response
    CLOUD_FINISH   // cleaning up
};

static CloudState state = CLOUD_IDLE;

void CloudManager::init() {
    client.setInsecure();
    lock = xSemaphoreCreateMutex(); // binary semaphore (for locking one task at a time)

    // non blocking cloud task
    xTaskCreatePinnedToCore([](void*){
        for (;;) {
            // run every 10ms
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (WiFi.status() != WL_CONNECTED) continue;
            xSemaphoreTake(lock, portMAX_DELAY);

            // state machine (cloud POST handling)
            switch (state) {
            case CLOUD_IDLE:
                xSemaphoreGive(lock);
                continue;
            case CLOUD_BEGIN:
                if (!http.begin(client, CLOUD_ENDPOINT)) {
                    state = CLOUD_IDLE;
                } else {
                    http.addHeader("Content-Type", "application/json");
                    state = CLOUD_SEND;
                }
                break;
            case CLOUD_SEND:
                http.POST(pendingPayload);
                state = CLOUD_WAIT;
                break;
            case CLOUD_WAIT: {
                int code = http.getSize();
                if (code != HTTPC_ERROR_CONNECTION_REFUSED &&
                    code != HTTPC_ERROR_SEND_HEADER_FAILED &&
                    code != HTTPC_ERROR_READ_TIMEOUT) {

                    Serial.printf("Cloud POST finished: %d\n", code);
                    state = CLOUD_FINISH;
                }
                break;
            }
            case CLOUD_FINISH:
                http.end();
                client.stop();
                pendingPayload = "";
                state = CLOUD_IDLE;
                break;
            }

            xSemaphoreGive(lock);
        }
    },
    "cloudTask", 6000, NULL, 1, &cloudTaskHandle, 1 // name, stack, priority, handle, core
    // the main loop is on core 0, so we pin this to core 1
    );
}

bool CloudManager::sendEvent(const String &jsonPayload) {
    if (xSemaphoreTake(lock, 0) == pdFALSE) return false; // busy
    if (state != CLOUD_IDLE) {
        xSemaphoreGive(lock);
        return false;
    }
    pendingPayload = jsonPayload;
    state = CLOUD_BEGIN;

    xSemaphoreGive(lock);
    return true;
}

// timestamped heartbeat
bool CloudManager::sendHeartbeat() {
    String hb = "{\"type\":\"heartbeat\",\"ts\":" + String(millis()) + "}";
    return sendEvent(hb);
}